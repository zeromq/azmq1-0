/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of azmq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AZMQ_DETAIL_CONFIG_CONDITION_VARIABLE_HPP_
#define AZMQ_DETAIL_CONFIG_CONDITION_VARIABLE_HPP_

#if !defined(AZMQ_DISABLE_STD_CONDITION_VARIABLE)
#   include <condition_variable>
#   define AZMQ_HAS_STD_CONDITION_VARIABLE 1
    namespace azmq { namespace detail {
        using condition_variable_t = std::condition_variable;
    } }
#else // defined(AZMQ_DISABLE_STD_CONDITION_VARIABLE)
#   include <boost/thread/condition_variable.hpp>
    namespace azmq { namespace detail {
        using condition_variable_t = boost::condition_variable;
    } }
#   endif // !defined(AZMQ_DISABLE_STD_CONDITION_VARIABLE)
#endif // !defined(AZMQ_HAS_STD_CONDITION_VARIABLE)


