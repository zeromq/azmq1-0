/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of azmq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AZMQ_SIGNAL_HPP_
#define AZMQ_SIGNAL_HPP_

#include "socket.hpp"

namespace azmq {
namespace signal {
AZMQ_V1_INLINE_NAMESPACE_BEGIN
/** \brief Send a signal over a socket. A signal is a short message carrying a
 *  success/failure code (by convention, 0 means OK). Signals are encoded to be
 *  distinguishable from "normal" messages.
 *  \param s socket& to signal on
 *  \param status uint8_t to send
 *  \param ec boost::system::error_code&
 *  \return boost::system::error_code
 */
boost::system::error_code send(socket & s, uint8_t status,
                               boost::system::error_code & ec) {
    uint64_t v = 0x77664433221100u + status;
    auto buf = boost::asio::buffer(&v, sizeof(v));
    s.send(buf, 0, ec);
    return ec;
}

/** \brief Send a signal over a socket. A signal is a short message carrying a
 *  success/failure code (by convention, 0 means OK). Signals are encoded to be
 *  distinguishable from "normal" messages.
 *  \param s socket& to signal on
 *  \param status uint8_t to send
 *  \throw boost::system::system_error
 */
void send(socket & s, uint8_t status) {
    boost::system::error_code ec;
    if (send(s, status, ec))
        throw boost::system::system_error(ec);
}

/** \brief Wait on a signal from a socket. Use this with signal() to coordiante
 *  over thread/actor pipes
 *  \param s socket& to receive signal from
 *  \param ec boost::system::error_code
 *  \return signal
 */
uint8_t wait(socket & s, boost::system::error_code & ec) {
    message msg;
    while (true) {
        auto sz = s.receive(msg, 0, ec);
        if (ec)
            return 0;
        if (sz == sizeof(uint64_t)) {
            auto v = msg.buffer_cast<uint64_t>();
            if ((v & 0xffffffffffff00u) == 0x77664433221100u)
                return v & 255;
        }
    }
}

/** \brief Wait on a signal from a socket. Use this with signal() to coordiante
 *  over thread/actor pipes
 *  \param s socket& to receive signal from
 *  \return signal
 *  \throw boost::system::system_error
 */
uint8_t wait(socket & s) {
    boost::system::error_code ec;
    auto res = wait(s, ec);
    if (ec)
        throw boost::system::system_error(ec);
    return res;
}

AZMQ_V1_INLINE_NAMESPACE_END
} // namespace signal
} // namespace azmq
#endif // AZMQ_SIGNAL_HPP_

