/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of azmq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AZMQ_DETAIL_CONFIG_LOCK_GUARD_HPP_
#define AZMQ_DETAIL_CONFIG_LOCK_GUARD_HPP_

#if !defined(AZMQ_DISABLE_STD_LOCK_GUARD)
#   include <mutex>
#   define AZMQ_HAS_STD_LOCK_GUARD 1
    namespace azmq { namespace detail {
        template<typename T>
        using lock_guard_t = std::lock_guard<T>;
    } }
#else // defined(AZMQ_DISABLE_STD_LOCK_GUARD)
#   include <boost/thread/lock_guard.hpp>
    namespace azmq { namespace detail {
        template<typename T>
        using lock_guard_t = boost::lock_guard<T>;
    } }
#   endif // !defined(AZMQ_DISABLE_STD_LOCK_GUARD)
#endif // !defined(AZMQ_HAS_STD_LOCK_GUARD)


