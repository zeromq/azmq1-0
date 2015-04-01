/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of azmq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AZMQ_DETAIL_CONFIG_THREAD_HPP_
#define AZMQ_DETAIL_CONFIG_THREAD_HPP_

#if !defined(AZMQ_HAS_STD_THREAD)
#if !defined(AZMQ_DISABLE_STD_THREAD)
#include <thread>
#define AZMQ_HAS_STD_THREAD 1
    namespace azmq { namespace detail {
        using thread_t = std::thread;
    } }
#else // defined(AZMQ_DISABLE_STD_THREAD)
#include <boost/thread/thread.hpp>
    namespace azmq { namespace detail {
        using thread_t = boost::thread;
    } }
#endif // !defined(AZMQ_DISABLE_STD_THREAD)
#endif // !defined(AZMQ_HAS_STD_THREAD)
#endif // AZMQ_DETAIL_CONFIG_THREAD_HPP_
