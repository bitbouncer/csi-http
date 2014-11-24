//
// http_client.cpp
// 
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <chrono>
#include <future>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include "http_client.h"

namespace csi
{
    size_t http_client::call_context::s_context_count = 0;
    csi::spinlock http_client::call_context::s_spinlock;

    http_client::http_client(boost::asio::io_service& io_service) :
        _io_service(io_service),
        _timer(_io_service),
        _keepalive_timer(_io_service)
    {
        _multi = curl_multi_init();
        curl_multi_setopt(_multi, CURLMOPT_SOCKETFUNCTION, _sock_cb);
        curl_multi_setopt(_multi, CURLMOPT_SOCKETDATA, this);
        curl_multi_setopt(_multi, CURLMOPT_TIMERFUNCTION, _multi_timer_cb);
        curl_multi_setopt(_multi, CURLMOPT_TIMERDATA, this);
        _keepalive_timer.expires_from_now(boost::chrono::milliseconds(1000));
        _keepalive_timer.async_wait(boost::bind(&http_client::keepalivetimer_cb, this, _1));
    }

    http_client::~http_client()
    {
        close();
        curl_multi_cleanup(_multi);
    }

    void http_client::close()
    {
        _keepalive_timer.cancel();
        _timer.cancel();
    }

    int http_client::_multi_timer_cb(CURLM *multi, long timeout_ms, void *userp)
    {
        return ((http_client*)userp)->multi_timer_cb(multi, timeout_ms);
    }

    /* Update the event timer after curl_multi library calls */
    int http_client::multi_timer_cb(CURLM* multi, long timeout_ms)
    {
        BOOST_LOG_TRIVIAL(trace) << "http_client::multi_timer_cb: timeout_ms " << timeout_ms;

        /* cancel running timer */
        _timer.cancel();

        if (timeout_ms > 0)
        {
            _timer.expires_from_now(boost::chrono::milliseconds(timeout_ms));
            _timer.async_wait(boost::bind(&http_client::timer_cb, this, _1));
        }
        else
        {
            /* call timeout function immediately */
            boost::system::error_code error; /*success*/
            timer_cb(error);
        }
        return 0;
    }
    
    void http_client::socket_rx_cb(const boost::system::error_code& e, boost::asio::ip::tcp::socket * tcp_socket, call_context::handle context)
    {
        if (!e && !context->_curl_done)
        {
            //BOOST_LOG_TRIVIAL(debug) << "socket_rx_cb " << e << " bytes " << tcp_socket->available();
            CURLMcode rc = curl_multi_socket_action(_multi, tcp_socket->native_handle(), CURL_CSELECT_IN, &_still_running);
            if (!context->_curl_done)
                tcp_socket->async_read_some(boost::asio::null_buffers(), boost::bind(&http_client::socket_rx_cb, this, boost::asio::placeholders::error, tcp_socket, context));
        }
    }

    void http_client::socket_tx_cb(const boost::system::error_code& e, boost::asio::ip::tcp::socket * tcp_socket, call_context::handle context)
    {
        if (!e)
            CURLMcode rc = curl_multi_socket_action(_multi, tcp_socket->native_handle(), CURL_CSELECT_OUT, &_still_running);
    }

    /* Called by asio when our timeout expires */
    void http_client::timer_cb(const boost::system::error_code& e)
    {
        if (!e)
        {
            BOOST_LOG_TRIVIAL(trace) << "http_client::timer_cb:";
            // CURL_SOCKET_TIMEOUT, 0 is corrent on timeouts http://curl.haxx.se/libcurl/c/curl_multi_socket_action.html
            CURLMcode rc = curl_multi_socket_action(_multi, CURL_SOCKET_TIMEOUT, 0, &_still_running);
            //check_multi_info(); //TBD kolla om denna ska vara här
        }
    }

    void http_client::keepalivetimer_cb(const boost::system::error_code & error)
    {
        if (!error)
        {
            _keepalive_timer.expires_from_now(boost::chrono::milliseconds(1000));
            _keepalive_timer.async_wait(boost::bind(&http_client::keepalivetimer_cb, this, _1));
        }
    }

    /*
    *   user_data            : this set using                     curl_multi_setopt(_multi, CURLMOPT_SOCKETDATA, this);
    *   per_socket_user_data : async_context set on_connect using curl_multi_assign(_multi, ....
    */
    int http_client::_sock_cb(CURL *e, curl_socket_t s, int what, void *user_data, void* per_socket_user_data)
    {
        return ((http_client*)user_data)->sock_cb(e, s, what, per_socket_user_data);
    }

    // must not be called within curl callbacks - post a asio message instead
    void http_client::_poll_remove(call_context::handle h)
    {
        BOOST_LOG_TRIVIAL(debug) << "curl_multi_remove_handle cleanup";
        curl_multi_remove_handle(_multi, h->_curl_easy);
    }

    int http_client::sock_cb(CURL *e, curl_socket_t s, int what, void* per_socket_user_data)
    {
        //BOOST_LOG_TRIVIAL(trace) << "http_client::sock_cb ";
        boost::asio::ip::tcp::socket* tcp_socket = (boost::asio::ip::tcp::socket*) per_socket_user_data;

        call_context* context = NULL;
        curl_easy_getinfo(e, CURLINFO_PRIVATE, &context);

        assert(context);

        if (!tcp_socket)
        {
            //we try to find the data in our own mapping
            //if we find it - register this to curl so we dont have to do this every time.
            {
                csi::spinlock::scoped_lock xxx(_spinlock);
                std::map<curl_socket_t, boost::asio::ip::tcp::socket *>::iterator it = _socket_map.find(s);
                if (it != _socket_map.end())
                {
                    tcp_socket = it->second;
                    curl_multi_assign(_multi, s, tcp_socket);
                }
            }

            if (!tcp_socket)
            {
                BOOST_LOG_TRIVIAL(trace) << "http_client::socket " << s << " is a c-ares socket, ignoring";
                return 0;
            }
        }

        switch (what)
        {
        case CURL_POLL_REMOVE:
        {
                                 //call_context* context = NULL;
                                 //curl_easy_getinfo(e, CURLINFO_PRIVATE, &context);
                                 long http_result = 0;
                                 curl_easy_getinfo(e, CURLINFO_RESPONSE_CODE, &http_result);
                                 context->_http_result = (csi::http::status_type) http_result;
                                 context->_end_ts = std::chrono::steady_clock::now();
                                 context->_curl_done = true;
                                 context->_transport_ok = (http_result > 0);

                                 BOOST_LOG_TRIVIAL(debug) << "http_client::CURL_POLL_REMOVE << http:" << to_string(context->_method) << " " << context->uri() << " res = " << http_result << " " << context->milliseconds() << " ms";
                                 call_context::handle h(context->curl_handle());

                                 if (context->_callback)
                                     context->_callback(h);

                                 context->curl_stop();
                                 curl_easy_setopt(context->_curl_easy, CURLOPT_PRIVATE, NULL);

                                 _io_service.post(boost::bind(&http_client::_poll_remove, this, h));
        }
            break;

        case CURL_POLL_IN:
            BOOST_LOG_TRIVIAL(debug) << "http_client::CURL_POLL_IN";
            tcp_socket->async_read_some(boost::asio::null_buffers(), boost::bind(&http_client::socket_rx_cb, this, boost::asio::placeholders::error, tcp_socket, context->curl_handle()));
            break;
        case CURL_POLL_OUT:
            BOOST_LOG_TRIVIAL(debug) << "http_client::CURL_POLL_OUT";
            tcp_socket->async_write_some(boost::asio::null_buffers(), boost::bind(&http_client::socket_tx_cb, this, boost::asio::placeholders::error, tcp_socket, context->curl_handle()));
            break;
        case CURL_POLL_INOUT:
            BOOST_LOG_TRIVIAL(debug) << "http_client::CURL_POLL_INOUT";
            tcp_socket->async_read_some(boost::asio::null_buffers(), boost::bind(&http_client::socket_rx_cb, this, boost::asio::placeholders::error, tcp_socket, context->curl_handle()));
            tcp_socket->async_write_some(boost::asio::null_buffers(), boost::bind(&http_client::socket_tx_cb, this, boost::asio::placeholders::error, tcp_socket, context->curl_handle()));
            break;
        };
        return 0;
    }

    /* STATIC CURLOPT_OPENSOCKETFUNCTION */
    curl_socket_t http_client::_opensocket_cb(void *clientp, curlsocktype purpose, struct curl_sockaddr *address)
    {
        return ((http_client*)clientp)->opensocket_cb(purpose, address);
    }

    /* CURLOPT_OPENSOCKETFUNCTION */
    curl_socket_t http_client::opensocket_cb(curlsocktype purpose, struct curl_sockaddr *address)
    {
        /* restrict to ipv4 */
        if (purpose == CURLSOCKTYPE_IPCXN && address->family == AF_INET)
        {
            /* create a tcp socket object */
            boost::asio::ip::tcp::socket *tcp_socket = new boost::asio::ip::tcp::socket(_io_service);

            /* open it and get the native handle*/
            boost::system::error_code ec;
            tcp_socket->open(boost::asio::ip::tcp::v4(), ec);

            if (ec)
            {
                BOOST_LOG_TRIVIAL(error) << "http_client::Couldn't open socket [" << ec << "][" << ec.message() << "]";
                delete tcp_socket;
                return CURL_SOCKET_BAD;
            }

            curl_socket_t sockfd = tcp_socket->native_handle();
            /* save it for monitoring */
            {
                csi::spinlock::scoped_lock xxx(_spinlock);
                _socket_map.insert(std::pair<curl_socket_t, boost::asio::ip::tcp::socket *>(sockfd, tcp_socket));
            }
            BOOST_LOG_TRIVIAL(debug) << "http_client::opened socket " << sockfd;
            return sockfd;
        }
        return CURL_SOCKET_BAD;
    }

    /* CURLOPT_CLOSESOCKETFUNCTION */
    int http_client::_closesocket_cb(void *clientp, curl_socket_t item)
    {
        return ((http_client*)clientp)->closesocket_cb(item);
    }

    /* CURLOPT_CLOSESOCKETFUNCTION */
    int http_client::closesocket_cb(curl_socket_t item)
    {
        BOOST_LOG_TRIVIAL(debug) << "http_client::closesocket_cb : " << item;

        {
            csi::spinlock::scoped_lock xxx(_spinlock);
            std::map<curl_socket_t, boost::asio::ip::tcp::socket*>::iterator it = _socket_map.find(item);
            if (it != _socket_map.end())
            {
                boost::system::error_code ec;
                it->second->cancel(ec);
                it->second->close(ec);
                _io_service.post(boost::bind(&http_client::_asio_closesocket_cb, this, item));
            }
        }
        return 0;
    }

    /* Called from asio a bit later than above */
    void http_client::_asio_closesocket_cb(curl_socket_t item)
    {
        BOOST_LOG_TRIVIAL(debug) << "http_client::_asio_closesocket_cb : " << item;
        {
            csi::spinlock::scoped_lock xxx(_spinlock);
            std::map<curl_socket_t, boost::asio::ip::tcp::socket*>::iterator it = _socket_map.find(item);
            if (it != _socket_map.end())
            {
                delete it->second;
                _socket_map.erase(it);
            }
        }
    }

    static size_t write_callback_avro_stream(void *ptr, size_t size, size_t nmemb, avro::StreamWriter* stream)
    {
        size_t sz = size*nmemb;
        stream->writeBytes((const uint8_t*)ptr, sz);
        stream->flush();
        return sz;
    }

    static size_t read_callback_avro_stream(void *ptr, size_t size, size_t nmemb, avro::StreamReader* stream)
    {
        size_t sz = csi::readBytes(stream, (uint8_t*)ptr, size*nmemb);
        return sz;
    }

    static size_t parse_headers(void *buffer, size_t size, size_t nmemb, std::vector<std::string>* v)
    {
        const char *d = (const char*)buffer;
        size_t result = 0;
        if (v)
        {
            std::string s = "";
            s.append(d, size * nmemb);
            v->push_back(s);
            result = size * nmemb;
        }
        return result;
    }

    static void wait_for_completion(http_client::call_context::handle request, std::promise<bool>* promise)
    {
        bool res = request->transport_result() && request->http_result() >= 200 && request->http_result() < 300;
        promise->set_value(res);
    }

    csi::http_client::call_context::handle http_client::perform(call_context::handle request)
    {
        std::promise<bool> promise; // just a dummy value - we use the request handle anyway as result
        std::future<bool> future = promise.get_future();
        perform_async(request, boost::bind(wait_for_completion, _1, &promise));
        future.wait();
        bool res = future.get();
        return request;
    }

    void http_client::perform_async(call_context::handle request, call_context::callback cb)
    {
        request->_callback = cb;
        _io_service.post(boost::bind(&http_client::_perform, this, request));
    }

    void http_client::_perform(call_context::handle request)
    {
        request->curl_start(request); // increments usage count and kjeeps object around untilk curl thinks its done.

        //ugly init of avro memory stream that seems to take a snapshot of what's in the buffer att creation time
        request->_avro_tx_stream = avro::memoryInputStream(*request->_avro_tx_buffer.get());
        request->_avro_tx_stream_reader.reset(*request->_avro_tx_stream.get());

        curl_easy_setopt(request->_curl_easy, CURLOPT_OPENSOCKETFUNCTION, _opensocket_cb);
        curl_easy_setopt(request->_curl_easy, CURLOPT_OPENSOCKETDATA, this);

        curl_easy_setopt(request->_curl_easy, CURLOPT_CLOSESOCKETFUNCTION, _closesocket_cb);
        curl_easy_setopt(request->_curl_easy, CURLOPT_CLOSESOCKETDATA, this);

        //SSL OPTIONS
        //set up curls cerfificate check
        //curl_easy_setopt(m_hcURL, CURLOPT_SSL_VERIFYPEER, 1)); 
        //curl_easy_setopt(m_hcURL, CURLOPT_CAINFO,"curl-ca-bundle.cer")); 

        // for now skip ca check
        //curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0); 
        // retrieve cert info
        //curl_easy_setopt(_curl, CURLOPT_CERTINFO, 1); 

        /*for debugging */
        if (request->_verbose)
            curl_easy_setopt(request->_curl_easy, CURLOPT_VERBOSE, 1L);

        switch (request->_method)
        {
        case csi::http::GET:
            curl_easy_setopt(request->_curl_easy, CURLOPT_HTTPGET, 1);
            break;

        case csi::http::PUT:
            curl_easy_setopt(request->_curl_easy, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(request->_curl_easy, CURLOPT_INFILESIZE_LARGE, (curl_off_t)request->tx_content_length());
            break;

        case csi::http::POST:
            curl_easy_setopt(request->_curl_easy, CURLOPT_POST, 1);
            curl_easy_setopt(request->_curl_easy, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)request->tx_content_length()); // must be different in post and put???
            break;

        default:
            assert(false);
        };

        curl_easy_setopt(request->_curl_easy, CURLOPT_URL, request->_uri.c_str());

        /* the request */
        curl_easy_setopt(request->_curl_easy, CURLOPT_READFUNCTION, read_callback_avro_stream);
        curl_easy_setopt(request->_curl_easy, CURLOPT_READDATA, &request->_avro_tx_stream_reader);

        /* the reply */
        curl_easy_setopt(request->_curl_easy, CURLOPT_WRITEFUNCTION, write_callback_avro_stream);
        curl_easy_setopt(request->_curl_easy, CURLOPT_WRITEDATA, &request->_avro_rx_buffer_writer);

        curl_easy_setopt(request->_curl_easy, CURLOPT_PRIVATE, request.get());
        curl_easy_setopt(request->_curl_easy, CURLOPT_LOW_SPEED_TIME, 3L);
        curl_easy_setopt(request->_curl_easy, CURLOPT_LOW_SPEED_LIMIT, 10L);

        // if this is a resend we have to clear this before using it again
        if (request->_curl_headerlist)
        {
            assert(false); // is this really nessessary??
            curl_slist_free_all(request->_curl_headerlist);
            request->_curl_headerlist = NULL;
        }

        static const char buf[] = "Expect:";
        /* initalize custom header list (stating that Expect: 100-continue is not wanted */
        request->_curl_headerlist = curl_slist_append(request->_curl_headerlist, buf);
        request->_curl_headerlist = curl_slist_append(request->_curl_headerlist, "User-Agent:csi-http/0.0.1");
        for (std::vector<std::string>::const_iterator i = request->_tx_headers.begin(); i != request->_tx_headers.end(); ++i)
            request->_curl_headerlist = curl_slist_append(request->_curl_headerlist, i->c_str());
        curl_easy_setopt(request->_curl_easy, CURLOPT_HTTPHEADER, request->_curl_headerlist);
        curl_easy_setopt(request->_curl_easy, CURLOPT_TIMEOUT_MS, request->_timeoutX.count());
        curl_easy_setopt(request->_curl_easy, CURLOPT_HEADERFUNCTION, parse_headers);
        curl_easy_setopt(request->_curl_easy, CURLOPT_WRITEHEADER, &request->_rx_headers);

        //SSL OPTIONS
        // for now skip ca check
        curl_easy_setopt(request->_curl_easy, CURLOPT_SSL_VERIFYPEER, 0);
        // retrieve cert info
        curl_easy_setopt(request->_curl_easy, CURLOPT_CERTINFO, 1);
        request->_start_ts = std::chrono::steady_clock::now();
        BOOST_LOG_TRIVIAL(debug) << "http_client::perform >> http:" << to_string(request->_method) << " " << request->_uri;
        CURLMcode rc = curl_multi_add_handle(_multi, request->_curl_easy);
    }
};