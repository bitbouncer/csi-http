#include <boost/algorithm/string.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp> 
#include <boost/lexical_cast.hpp>
#include "request.h"

namespace csi
{
    namespace http
    {
        request_t::request_t()
        { 
            reset(); 
        }

        void request_t::reset()
        {
            _request_uuid = boost::uuids::nil_uuid();
            _request_id.clear();
            _content_length = 0;
            _url.clear();
            _query.clear();
            _headers.clear();
            _avro_rx_buffer = avro::memoryOutputStream();
            _avro_rx_buffer_stream_writer = std::auto_ptr<avro::StreamWriter>(new avro::StreamWriter(*_avro_rx_buffer));
        }

        std::string request_t::get_header(const std::string& h) const
        {
            for (std::vector<header_t>::const_iterator i = _headers.begin(); i != _headers.end(); ++i)
            {
                if (boost::iequals(h, i->name))
                    return i->value;
            }
            return "";
        }

        // this keeps existing string or existing or creates a new uuid in not existing
        const std::string& request_t::request_id(const std::string& request_id_header) const
        {
            if (_request_id.size())
                return _request_id;

            _request_id = get_header(request_id_header);

            if (_request_id.size())
            {
                try
                {
                    _request_uuid = boost::uuids::string_generator()(_request_id);
                    return _request_id;
                }
                catch (...)
                {
                    _request_uuid = boost::uuids::nil_uuid();
                    return _request_id;
                }
            }
            else
            {
                //it his better or worse??? TBD
                //boost::uuids::basic_random_generator<boost::mt19937> gen;
                //_request_uuid = gen();
                _request_uuid = boost::uuids::random_generator()();
                _request_id = to_string(_request_uuid);
            }
            return _request_id;
        }

        // this keeps existing uuid or creates a new if not existing (overwrites existing string if not uuid)
        const boost::uuids::uuid&  request_t::request_uuid(const std::string& request_id_header) const
        {
            if (!_request_uuid.is_nil())
                return _request_uuid;

            _request_id = get_header(request_id_header);

            if (_request_id.size())
            {
                try
                {
                    _request_uuid = boost::uuids::string_generator()(_request_id);
                    return _request_uuid;
                }
                catch (...)
                {
                    _request_uuid = boost::uuids::random_generator()();
                    _request_id = to_string(_request_uuid);
                    return _request_uuid;
                }
            }

            _request_uuid = boost::uuids::random_generator()();
             _request_id = to_string(_request_uuid);
            return _request_uuid;
        }
    };
};