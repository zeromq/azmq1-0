/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of azmq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#include <azmq/error.hpp>

#include <zmq.h>

namespace azmq {
AZMQ_V1_INLINE_NAMESPACE_BEGIN
const char* error_category::name() const BOOST_SYSTEM_NOEXCEPT {
    return "ZeroMQ";
}

std::string error_category::message(int ev) const {
    return std::string(zmq_strerror(ev));
}

boost::system::error_code make_error_code(int ev) {
    static error_category cat;

    return boost::system::error_code(ev, cat);
}
AZMQ_V1_INLINE_NAMESPACE_END
} // namespace azmq

