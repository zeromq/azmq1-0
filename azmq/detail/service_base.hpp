/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of azmq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AZMQ_DETAIL_SERVICE_BASE_HPP_
#define AZMQ_DETAIL_SERVICE_BASE_HPP_

#include <boost/asio/io_service.hpp>

namespace azmq {
namespace detail {
    template <typename T>
    class service_id
        : public boost::asio::io_service::id
    { };

    template<typename T>
    class service_base
        : public boost::asio::io_service::service {
    public :
        static azmq::detail::service_id<T> id;

        // Constructor.
        service_base(boost::asio::io_service& io_service)
            : boost::asio::io_service::service(io_service)
        { }
    };

    template <typename T>
    azmq::detail::service_id<T> service_base<T>::id;
} // namespace detail
} // namespace azmq
#endif // AZMQ_DETAIL_SERVICE_BASE_HPP_

