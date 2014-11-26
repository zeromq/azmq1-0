/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of azmq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#include <azmq/socket.hpp>
#include <azmq/util/scope_guard.hpp>

#define BOOST_ENABLE_ASSERT_HANDLER
#include <boost/assert.hpp>
#include <boost/asio/buffer.hpp>

#include <array>
#include <thread>
#include <iostream>
#include <vector>
#include <cstdint>
#include <memory>

#include "../assert.ipp"

std::array<boost::asio::const_buffer, 2> snd_bufs = {{
    boost::asio::buffer("A"),
    boost::asio::buffer("B")
}};

std::string subj(const char* name) {
    return std::string("inproc://") + name;
}

void test_set_get_options() {
    boost::asio::io_service ios;

    azmq::socket s(ios, ZMQ_ROUTER);

    // set/get_option are generic, works for one and all...
    azmq::socket::rcv_hwm in_hwm(42);
    s.set_option(in_hwm);

    azmq::socket::rcv_hwm out_hwm;
    s.get_option(out_hwm);
    BOOST_ASSERT_MSG(in_hwm.value() == out_hwm.value(), "in_hwm != out_hwm");
}

void test_send_receive_sync() {
    boost::asio::io_service ios;

    azmq::socket sb(ios, ZMQ_ROUTER);
    sb.bind(subj(BOOST_CURRENT_FUNCTION));

    azmq::socket sc(ios, ZMQ_DEALER);
    sc.connect(subj(BOOST_CURRENT_FUNCTION));

    sc.send(snd_bufs, ZMQ_SNDMORE);

    azmq::message msg;
    auto size = sb.receive(msg);

    BOOST_ASSERT_MSG(msg.more(), "more");

    size = sb.receive(msg, 0);
    BOOST_ASSERT_MSG(size == boost::asio::buffer_size(snd_bufs[0]), "buffer size");
    BOOST_ASSERT_MSG(msg.more(), "more");

    size = sb.receive(msg, 0);
    BOOST_ASSERT_MSG(size == boost::asio::buffer_size(snd_bufs[1]), "buffer size");
    BOOST_ASSERT_MSG(!msg.more(), "!more");

    sc.send(snd_bufs, ZMQ_SNDMORE);

    std::array<char, 5> ident;
    std::array<char, 2> a;
    std::array<char, 2> b;

    std::array<boost::asio::mutable_buffer, 3> rcv_bufs = {{
        boost::asio::buffer(ident),
        boost::asio::buffer(a),
        boost::asio::buffer(b)
    }};

    size = sb.receive(rcv_bufs, ZMQ_RCVMORE);
    BOOST_ASSERT_MSG(size == 9, "buffer size");
}

void test_send_receive_async(bool is_speculative) {
    boost::asio::io_service ios_b;
    boost::asio::io_service ios_c;

    azmq::socket sb(ios_b, ZMQ_ROUTER);
    sb.set_option(azmq::socket::allow_speculative(is_speculative));
    sb.bind(subj(BOOST_CURRENT_FUNCTION));

    azmq::socket sc(ios_c, ZMQ_DEALER);
    sc.set_option(azmq::socket::allow_speculative(is_speculative));
    sc.connect(subj(BOOST_CURRENT_FUNCTION));

    boost::system::error_code ecc;
    size_t btc = 0;
    sc.async_send(snd_bufs, [&] (boost::system::error_code const& ec, size_t bytes_transferred) {
        SCOPE_EXIT { ios_c.stop(); };
        ecc = ec;
        btc = bytes_transferred;
    }, ZMQ_SNDMORE);

    std::array<char, 5> ident;
    std::array<char, 2> a;
    std::array<char, 2> b;

    std::array<boost::asio::mutable_buffer, 3> rcv_bufs = {{
        boost::asio::buffer(ident),
        boost::asio::buffer(a),
        boost::asio::buffer(b)
    }};

    boost::system::error_code ecb;
    size_t btb = 0;
    sb.async_receive(rcv_bufs, [&](boost::system::error_code const& ec, size_t bytes_transferred) {
        SCOPE_EXIT { ios_b.stop(); };
        ecb = ec;
        btb = bytes_transferred;
    }, ZMQ_RCVMORE);

    ios_c.run();
    ios_b.run();

    BOOST_ASSERT_MSG(!ecc, "!ecc");
    BOOST_ASSERT_MSG(btc == 4, "btc != 4");
    BOOST_ASSERT_MSG(!ecb, "!ecb");
    BOOST_ASSERT_MSG(btb == 9, "btb != 9");
}

void test_send_receive_async_threads(bool optimize_single_threaded) {
    boost::asio::io_service ios_b;
    azmq::socket sb(ios_b, ZMQ_ROUTER, optimize_single_threaded);
    sb.bind(subj(BOOST_CURRENT_FUNCTION));

    boost::asio::io_service ios_c;
    azmq::socket sc(ios_c, ZMQ_DEALER, optimize_single_threaded);
    sc.connect(subj(BOOST_CURRENT_FUNCTION));

    boost::system::error_code ecc;
    size_t btc = 0;
    std::thread tc([&] {
        sc.async_send(snd_bufs, [&] (boost::system::error_code const& ec, size_t bytes_transferred) {
            SCOPE_EXIT { ios_c.stop(); };
            ecc = ec;
            btc = bytes_transferred;
        }, ZMQ_SNDMORE);
        ios_c.run();
    });

    boost::system::error_code ecb;
    size_t btb = 0;
    std::thread tb([&] {
        std::array<char, 5> ident;
        std::array<char, 2> a;
        std::array<char, 2> b;

        std::array<boost::asio::mutable_buffer, 3> rcv_bufs = {{
            boost::asio::buffer(ident),
            boost::asio::buffer(a),
            boost::asio::buffer(b)
        }};

        sb.async_receive(rcv_bufs, [&](boost::system::error_code const& ec, size_t bytes_transferred) {
            SCOPE_EXIT { ios_b.stop(); };
            ecb = ec;
            btb = bytes_transferred;
        }, ZMQ_RCVMORE);
        ios_b.run();
    });

    tc.join();
    tb.join();
    BOOST_ASSERT_MSG(!ecc, "!ecc");
    BOOST_ASSERT_MSG(btc == 4, "btc != 4");
    BOOST_ASSERT_MSG(!ecb, "!ecb");
    BOOST_ASSERT_MSG(btb == 9, "btb != 9");
}

void test_send_receive_message_async() {
    boost::asio::io_service ios_b;
    boost::asio::io_service ios_c;

    azmq::socket sb(ios_b, ZMQ_ROUTER);
    sb.bind(subj(BOOST_CURRENT_FUNCTION));

    azmq::socket sc(ios_c, ZMQ_DEALER);
    sc.connect(subj(BOOST_CURRENT_FUNCTION));

    boost::system::error_code ecc;
    size_t btc = 0;
    sc.async_send(snd_bufs, [&] (boost::system::error_code const& ec, size_t bytes_transferred) {
        SCOPE_EXIT { ios_c.stop(); };
        ecc = ec;
        btc = bytes_transferred;
    }, ZMQ_SNDMORE);

    std::array<char, 5> ident;
    std::array<char, 2> a;
    std::array<char, 2> b;

    boost::system::error_code ecb;
    size_t btb = 0;
    sb.async_receive([&](boost::system::error_code const& ec, azmq::message & msg, size_t bytes_transferred) {
        SCOPE_EXIT { ios_b.stop(); };
        ecb = ec;
        if (ecb)
            return;
        btb += bytes_transferred;
        msg.buffer_copy(boost::asio::buffer(ident));

        if (msg.more()) {
            btb += sb.receive(msg, ZMQ_RCVMORE, ecb);
            if (ecb)
                return;
            msg.buffer_copy(boost::asio::buffer(a));
        }

        if (msg.more()) {
            btb += sb.receive(msg, 0, ecb);
            if (ecb)
                return;
            msg.buffer_copy(boost::asio::buffer(b));
        }
    });

    ios_c.run();
    ios_b.run();

    BOOST_ASSERT_MSG(!ecc, "!ecc");
    BOOST_ASSERT_MSG(btc == 4, "btc != 4");
    BOOST_ASSERT_MSG(!ecb, "!ecb");
    BOOST_ASSERT_MSG(btb == 9, "btb != 9");
}

void test_send_receive_message_more_async() {
    boost::asio::io_service ios_b;
    boost::asio::io_service ios_c;

    azmq::socket sb(ios_b, ZMQ_ROUTER);
    sb.bind(subj(BOOST_CURRENT_FUNCTION));

    azmq::socket sc(ios_c, ZMQ_DEALER);
    sc.connect(subj(BOOST_CURRENT_FUNCTION));

    boost::system::error_code ecc;
    size_t btc = 0;
    sc.async_send(snd_bufs, [&] (boost::system::error_code const& ec, size_t bytes_transferred) {
        SCOPE_EXIT { ios_c.stop(); };
        ecc = ec;
        btc = bytes_transferred;
    }, ZMQ_SNDMORE);

    std::array<char, 5> ident;
    std::array<char, 2> a;
    std::array<char, 2> b;

    std::array<boost::asio::mutable_buffer, 2> rcv_bufs = {{
        boost::asio::buffer(a),
        boost::asio::buffer(b)
    }};

    boost::system::error_code ecb;
    size_t btb = 0;
    sb.async_receive([&](boost::system::error_code const& ec, azmq::message & msg, size_t bytes_transferred) {
        SCOPE_EXIT { ios_b.stop(); };
        ecb = ec;
        if (ecb)
            return;
        btb += bytes_transferred;
        msg.buffer_copy(boost::asio::buffer(ident));

        if (!msg.more())
            return;

        azmq::message_vector v;
        btb += sb.receive_more(v, 0, ecb);
        if (ecb)
            return;
        auto it = std::begin(v);
        for (auto&& buf : rcv_bufs)
            (*it++).buffer_copy(buf);
    });

    ios_c.run();
    ios_b.run();

    BOOST_ASSERT_MSG(!ecc, "!ecc");
    BOOST_ASSERT_MSG(btc == 4, "btc != 4");
    BOOST_ASSERT_MSG(!ecb, "!ecb");
    BOOST_ASSERT_MSG(btb == 9, "btb != 9");
}

struct monitor_handler {
    using ptr = std::shared_ptr<monitor_handler>;

#if defined BOOST_MSVC
#pragma pack(push, 1)
    struct event_t
    {
        uint16_t e;
        uint32_t i;
    };
#pragma pack(pop)
#else
    struct event_t
    {
        uint16_t e;
        uint32_t i;
    } __attribute__((packed));
#endif

    azmq::socket socket_;
    std::string role_;
    event_t event_;
    std::vector<event_t> events_;

    monitor_handler(boost::asio::io_service & ios, azmq::socket& s, std::string role)
        : socket_(s.monitor(ios, ZMQ_EVENT_ALL))
        , role_(std::move(role))
    { }

    void start()
    {
        socket_.async_receive(boost::asio::buffer(&event_, sizeof(event_t)),
            [this](boost::system::error_code const& ec, size_t) {
                if (ec)
                    return;
                azmq::message msg;
                socket_.receive(msg, ZMQ_RCVMORE);
                events_.push_back(event_);
                start();
            });
    }

    void cancel()
    {
        socket_.cancel();
    }
};

void bounce(azmq::socket & server, azmq::socket & client) {
    const char *content = "12345678ABCDEFGH12345678abcdefgh";
    std::array<boost::asio::const_buffer, 2> snd_bufs = {{
        boost::asio::buffer(content, 32),
        boost::asio::buffer(content, 32)
    }};

    std::array<char, 32> buf0;
    std::array<char, 32> buf1;

    std::array<boost::asio::mutable_buffer, 2> rcv_bufs = {{
        boost::asio::buffer(buf0),
        boost::asio::buffer(buf1)
    }};
    client.send(snd_bufs, ZMQ_SNDMORE);
    server.receive(rcv_bufs, ZMQ_RCVMORE);
    server.send(snd_bufs, ZMQ_SNDMORE);
    client.receive(rcv_bufs, ZMQ_RCVMORE);
}

void test_socket_monitor() {
    boost::asio::io_service ios;
    boost::asio::io_service ios_m;

    using socket_ptr = std::unique_ptr<azmq::socket>;
    socket_ptr client(new azmq::socket(ios, ZMQ_DEALER));
    socket_ptr server(new azmq::socket(ios, ZMQ_DEALER));

    monitor_handler client_monitor(ios_m, *client, "client");
    monitor_handler server_monitor(ios_m, *server, "server");

    client_monitor.start();
    server_monitor.start();

    std::thread t([&] {
        ios_m.run();
    });

    server->bind("tcp://127.0.0.1:9998");
    client->connect("tcp://127.0.0.1:9998");

    bounce(*client, *server);

    ios_m.stop();
    t.join();

    BOOST_ASSERT_MSG(client_monitor.events_.size() == 2, "wrong count of client_monitor events");
    BOOST_ASSERT_MSG(server_monitor.events_.size() == 2, "wrong count of server_monitor events");
    BOOST_ASSERT_MSG(client_monitor.events_.at(0).e == ZMQ_EVENT_CONNECT_DELAYED, "wrong client_monitor events[0]");
    BOOST_ASSERT_MSG(client_monitor.events_.at(1).e == ZMQ_EVENT_CONNECTED, "wrong client_monitor events[1]");
    BOOST_ASSERT_MSG(server_monitor.events_.at(0).e == ZMQ_EVENT_LISTENING, "wrong server_monitor events[0]");
    BOOST_ASSERT_MSG(server_monitor.events_.at(1).e == ZMQ_EVENT_ACCEPTED, "wrong server_monitor events[1]");
}

int main(int argc, char **argv) {
    std::cout << "Testing socket operations...";
    std::cout.flush();
    try {
        test_set_get_options();
        test_send_receive_sync();
        test_send_receive_async(true);
        test_send_receive_async(false);
        for (auto i = 0; i < 100; i++)
            test_send_receive_async_threads(true);
        for (auto i = 0; i < 100; i++)
            test_send_receive_async_threads(false);
        test_send_receive_message_async();
        test_send_receive_message_more_async();
        test_socket_monitor();
    } catch (std::exception const& e) {
        std::cout << "Failure\n" << e.what() << std::endl;
        return 1;
    }
    std::cout << "Success" << std::endl;
    return 0;
}

