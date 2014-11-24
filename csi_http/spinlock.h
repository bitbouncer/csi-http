//
// spinlock.h
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright 2014 Svante Karlsson CSI AB (svante.karlsson at csi dot se)
//
// this is just a "public" version of the internal spinlock that's in the shared_ptr implementation
// http://boost.2283326.n4.nabble.com/shared-ptr-thread-spinlock-initialization-td4636282.html
//
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <boost/noncopyable.hpp>
#include <boost/smart_ptr/detail/spinlock.hpp>

namespace csi
{
    class spinlock : public boost::noncopyable
    {
    public:
        spinlock();
        inline bool try_lock() { return _sl.try_lock(); }
        inline void lock()     { _sl.lock(); }
        inline void unlock()   { _sl.unlock(); }

        class scoped_lock
        {
        public:
            inline explicit scoped_lock(spinlock & sp) : _sl(sp)  { _sl.lock(); }
            inline ~scoped_lock()                                 { _sl.unlock(); }
        private:
            scoped_lock(scoped_lock const &);
            scoped_lock & operator=(scoped_lock const &);
            spinlock& _sl;
        };
    private:
        boost::detail::spinlock _sl;
    };
};
