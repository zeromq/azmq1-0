#include <azmq/detail/context_ops.hpp>
#include <azmq/detail/socket_ops.hpp>

#include <boost/asio/buffer.hpp>

#include <array>
#include <iostream>

#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

auto ctx = azmq::detail::context_ops::get_context();

std::array<boost::asio::const_buffer, 2> snd_bufs = {{
    boost::asio::buffer("A"),
    boost::asio::buffer("B")
}};

std::string subj(const char* name) {
    return std::string("inproc://") + name;
}

TEST_CASE( "Inproc Send/Receive discrete calls", "[socket_ops]" ) {
    boost::system::error_code ec;
    auto sb = azmq::detail::socket_ops::create_socket(ctx, ZMQ_ROUTER, ec);
    REQUIRE(ec == boost::system::error_code());
    azmq::detail::socket_ops::bind(sb, subj(BOOST_CURRENT_FUNCTION), ec);
    REQUIRE(ec == boost::system::error_code());

    auto sc = azmq::detail::socket_ops::create_socket(ctx, ZMQ_DEALER, ec);
    REQUIRE(ec == boost::system::error_code());
    azmq::detail::socket_ops::connect(sc, subj(BOOST_CURRENT_FUNCTION), ec);
    REQUIRE(ec == boost::system::error_code());

    // Send multipart message
    azmq::detail::socket_ops::send(snd_bufs, sc, 0, ec);
    REQUIRE(ec == boost::system::error_code());

    azmq::message msg;
    // Identity comes first
    azmq::detail::socket_ops::receive(msg, sb, 0, ec);
    REQUIRE(ec == boost::system::error_code());
    REQUIRE(msg.more() == true);

    // Then first part
    azmq::detail::socket_ops::receive(msg, sb, 0, ec);
    REQUIRE(ec == boost::system::error_code());
    REQUIRE(msg.more() == true);

    // Finally second part
    azmq::detail::socket_ops::receive(msg, sb, 0, ec);
    REQUIRE(ec == boost::system::error_code());
    REQUIRE(msg.more() == false);
}

TEST_CASE( "Inproc Send/Receive Buffer Sequence", "[socket_ops]" ) {
    boost::system::error_code ec;
    auto sb = azmq::detail::socket_ops::create_socket(ctx, ZMQ_ROUTER, ec);
    REQUIRE(ec == boost::system::error_code());
    azmq::detail::socket_ops::bind(sb, subj(BOOST_CURRENT_FUNCTION), ec);
    REQUIRE(ec == boost::system::error_code());

    auto sc = azmq::detail::socket_ops::create_socket(ctx, ZMQ_DEALER, ec);
    REQUIRE(ec == boost::system::error_code());
    azmq::detail::socket_ops::connect(sc, subj(BOOST_CURRENT_FUNCTION), ec);
    REQUIRE(ec == boost::system::error_code());

    // Send and receive all message parts as a mutable buffer sequence
    azmq::detail::socket_ops::send(snd_bufs, sc, 0, ec);
    REQUIRE(ec == boost::system::error_code());

    std::array<char, 5> ident;
    std::array<char, 2> part_A;
    std::array<char, 2> part_B;

    std::array<boost::asio::mutable_buffer, 3> rcv_msg_seq = {{
        boost::asio::buffer(ident),
        boost::asio::buffer(part_A),
        boost::asio::buffer(part_B)
    }};

    azmq::detail::socket_ops::receive(rcv_msg_seq, sb, 0, ec);
    REQUIRE(ec == boost::system::error_code());
    REQUIRE('A' == part_A[0]);
    REQUIRE('B' == part_B[0]);
}

TEST_CASE( "Inproc Send/Receive message vector", "[socket_ops]" ) {
    boost::system::error_code ec;
    auto sb = azmq::detail::socket_ops::create_socket(ctx, ZMQ_ROUTER, ec);
    REQUIRE(ec == boost::system::error_code());
    azmq::detail::socket_ops::bind(sb, subj(BOOST_CURRENT_FUNCTION), ec);
    REQUIRE(ec == boost::system::error_code());

    auto sc = azmq::detail::socket_ops::create_socket(ctx, ZMQ_DEALER, ec);
    REQUIRE(ec == boost::system::error_code());
    azmq::detail::socket_ops::connect(sc, subj(BOOST_CURRENT_FUNCTION), ec);
    REQUIRE(ec == boost::system::error_code());

    // Send and receive all message parts as a vector
    azmq::detail::socket_ops::send(snd_bufs, sc, 0, ec);
    REQUIRE(ec == boost::system::error_code());

    azmq::message_vector rcv_msgs;
    azmq::detail::socket_ops::receive_more(rcv_msgs, sb, 0, ec);
    REQUIRE(ec == boost::system::error_code());
    REQUIRE(rcv_msgs.size() == 3);
}

TEST_CASE( "Inproc Send/Receive not enough buffers", "[socket_ops]" ) {
    boost::system::error_code ec;
    auto sb = azmq::detail::socket_ops::create_socket(ctx, ZMQ_ROUTER, ec);
    REQUIRE(ec == boost::system::error_code());
    azmq::detail::socket_ops::bind(sb, subj(BOOST_CURRENT_FUNCTION), ec);
    REQUIRE(ec == boost::system::error_code());

    auto sc = azmq::detail::socket_ops::create_socket(ctx, ZMQ_DEALER, ec);
    REQUIRE(ec == boost::system::error_code());
    azmq::detail::socket_ops::connect(sc, subj(BOOST_CURRENT_FUNCTION), ec);
    REQUIRE(ec == boost::system::error_code());

    // Verify that we get an error on multipart with too few bufs in seq
    azmq::detail::socket_ops::send(snd_bufs, sc, 0, ec);
    REQUIRE(ec == boost::system::error_code());

    std::array<char, 5> ident;
    std::array<char, 2> part_A;

    std::array<boost::asio::mutable_buffer, 2> rcv_msg_seq_2 = {{
        boost::asio::buffer(ident),
        boost::asio::buffer(part_A)
    }};
    azmq::detail::socket_ops::receive(rcv_msg_seq_2, sb, 0, ec);
    REQUIRE(ec != boost::system::error_code());
}
