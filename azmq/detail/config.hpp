/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of azmq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AZMQ_DETAIL_CONFIG_HPP_
#define AZMQ_DETAIL_CONFIG_HPP_

#if !defined AZMQ_USE_STANDALONE_ASIO
#   include <boost/config.hpp>
#   if !defined BOOST_NO_CXX11_INLINE_NAMESPACES
#       define AZMQ_V1_INLINE_NAMESPACE_BEGIN inline namespace v1 {
#       define AZMQ_V1_INLINE_NAMESPACE_END }
#   else
#       define AZMQ_V1_INLINE_NAMESPACE_BEGIN
#       define AZMQ_V1_INLINE_NAMESPACE_END
#   endif
#else // AZMQ_USE_STANDALONG_ASIO
// Assume a competent C++11 implementation
#       define ASIO_STANDALONE 1
#       define AZMQ_V1_INLINE_NAMESPACE_BEGIN inline namespace v1 {
#       define AZMQ_V1_INLINE_NAMESPACE_END }
#endif //!defined AZMQ_USE_STANDALONE_ASIO


#endif // AZMQ_DETAIL_CONFIG_HPP_
