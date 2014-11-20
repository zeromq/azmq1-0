/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of aziomq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#include <aziomq/thread.hpp>
#include <aziomq/util/scope_guard.hpp>

#define BOOST_ENABLE_ASSERT_HANDLER
#include <boost/assert.hpp>
#include <boost/asio/buffer.hpp>

#include <array>
#include <thread>
#include <iostream>

#include "../assert.ipp"

std::array<boost::asio::const_buffer, 2> snd_bufs = {{
    boost::asio::buffer("A"),
    boost::asio::buffer("B")
}};

std::string subj(const char* name) {
    return std::string("inproc://") + name;
}

void test_send_receive_async_threads() {

    boost::system::error_code ecc;
    size_t btc = 0;

    boost::system::error_code ecb;
    size_t btb = 0;
    {
        std::array<char, 2> a;
        std::array<char, 2> b;

        std::array<boost::asio::mutable_buffer, 2> rcv_bufs = {{
            boost::asio::buffer(a),
            boost::asio::buffer(b)
        }};

        boost::asio::io_service ios;
        auto s = aziomq::thread::fork(ios, [&](aziomq::socket & ss) {
            ss.async_receive(rcv_bufs, [&](boost::system::error_code const& ec, size_t bytes_transferred) {
                ecb = ec;
                btb = bytes_transferred;
                ios.stop();
            }, ZMQ_RCVMORE);
            ss.get_io_service().run();
        });

        s.async_send(snd_bufs, [&] (boost::system::error_code const& ec, size_t bytes_transferred) {
            ecc = ec;
            btc = bytes_transferred;
        }, ZMQ_SNDMORE);

        boost::asio::io_service::work w(ios);
        ios.run();
    }

    BOOST_ASSERT_MSG(!ecc, "!ecc");
    BOOST_ASSERT_MSG(btc == 4, "btc != 4");
    BOOST_ASSERT_MSG(!ecb, "!ecb");
    BOOST_ASSERT_MSG(btb == 4, "btb != 4");
}

int main(int argc, char **argv) {
    std::cout << "Testing thread operations...";
    try {
        test_send_receive_async_threads();
    } catch (std::exception const& e) {
        std::cout << "Failure\n" << e.what() << std::endl;
        return 1;
    }
    std::cout << "Success" << std::endl;
    return 0;
}

