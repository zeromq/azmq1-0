/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of azmq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#include <azmq/detail/thread_service.hpp>

#include <string>
#include <sstream>
#include <atomic>

namespace azmq {
namespace detail {
boost::asio::io_service::id thread_service::id;

void thread_service::shutdown_service() {
    // TODO Implement
}

std::string thread_service::get_uri(const char* pfx) {
    static std::atomic_ulong id{ 0 };
    std::ostringstream stm;
    stm << "inproc://azmq-" << pfx << "-" << id++;
    return stm.str();
}

} // namesapce detail
} // namespace azmq

