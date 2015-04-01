/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of azmq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AZMQ_DETAIL_CONFIG_HPP_
#define AZMQ_DETAIL_CONFIG_HPP_

#if !defined BOOST_NO_CXX11_INLINE_NAMESPACES
    #define AZMQ_V1_INLINE_NAMESPACE_BEGIN inline namespace v1 {
    #define AZMQ_V1_INLINE_NAMESPACE_END }
#else
    #define AZMQ_V1_INLINE_NAMESPACE_BEGIN
    #define AZMQ_V1_INLINE_NAMESPACE_END
#endif

#endif // AZMQ_DETAIL_CONFIG_HPP_
