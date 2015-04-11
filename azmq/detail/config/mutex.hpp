/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of azmq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AZMQ_DETAIL_CONFIG_MUTEX_HPP_
#define AZMQ_DETAIL_CONFIG_MUTEX_HPP_

#if !defined(AZMQ_DISABLE_STD_MUTEX)
#   include <mutex>
#   define AZMQ_HAS_STD_MUTEX 1
    namespace azmq { namespace detail {
        using mutex_t = std::mutex;
    } }
#else // defined(AZMQ_DISABLE_STD_MUTEX)
#   include <boost/thread/mutex.hpp>
    namespace azmq { namespace detail {
        using mutex_t = boost::mutex;
    } }
#   endif // !defined(AZMQ_DISABLE_STD_MUTEX)
#endif // !defined(AZMQ_HAS_STD_MUTEX)

