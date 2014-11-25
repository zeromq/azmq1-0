/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of aziomq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AZMQ_THREAD_HPP_
#define AZMQ_THREAD_HPP_

#include "socket.hpp"
#include "detail/thread_service.hpp"

#include <boost/asio/io_service.hpp>

#include <functional>

namespace aziomq { namespace thread {
AZMQ_V1_INLINE_NAMESPACE_BEGIN

    using is_alive = detail::thread_service::is_alive;
    using detached = detail::thread_service::detached;
    using start = detail::thread_service::start;
    using last_error = detail::thread_service::last_error;

    /** \brief create a thread bound to one end of a pair of inproc sockets
     *  \param peer io_service to associate the peer (caller) end of the pipe
     *  \param f Function accepting socket& as the first parameter and a
     *           number of additional args
     *  \returns peer socket
     *
     *  \remark The newly created thread will be a std::thread, and will receive
     *  the 'server' end of the pipe as it's first argument.  This thread will be
     *  attached to the lifetime of the returned socket and will run until it is
     *  destroyed.
     *
     *  \remark Each forked thread has an associated io_service and the supplied
     *  socket will be created on this io_service. The thread function may access
     *  this by calling get_io_service() on the supplied socket.
     *
     *  \remark The associated io_service is configured to stop the spawned thread
     *  on SIG_KILL and SIG_TERM.
     *
     *  \remark Termination:
     *      well behaved threads should ultimately call run() on the io_service
     *      associated with the supplied socket. This allows the 'client' end of
     *      the socket's lifetime to cleanly signal termination of the thread. If
     *      for some reason, this is not possible, the caller should set the
     *      'detached' option on the 'client' socket. This detaches the associated
     *      thread from the client socket so that it will not be joined at
     *      destruction time. It is then up to the caller to work out the termination
     *      signal for the background thread; for instance by sending a termination
     *      message.
     *
     *      Also note, the default signal handling for the background thread is
     *      designed to call stop() on the associated io_service, so not calling
     *      run() in your handler means you are responsible for catching these
     *      signals in some other way.
     *
     *  \remark This is similar in concept to the CZMQ zthread_fork API, except
     *  that lifetime is controlled by the returned socket, not a separate zctx_t
     *  instance. Also, the notion of 'detached' differs from CZMQ's zthread API.
     *  Here a detached thread is simply abandoned rather than joined at socket
     *  destruction.
     */
    template<typename Function, typename... Args>
    socket fork(boost::asio::io_service & peer, bool defer_start, Function && f, Args&&... args) {
        auto& t = boost::asio::use_service<detail::thread_service>(peer);
        return t.make_pipe(defer_start, std::bind(std::forward<Function>(f),
                                                  std::placeholders::_1,
                                                  std::forward<Args>(args)...));
    }

    template<typename Function, typename... Args>
    socket fork(boost::asio::io_service & peer, Function && f, Args&&... args) {
        auto& t = boost::asio::use_service<detail::thread_service>(peer);
        return t.make_pipe(false, std::bind(std::forward<Function>(f),
                                            std::placeholders::_1,
                                            std::forward<Args>(args)...));
    }
AZMQ_V1_INLINE_NAMESPACE_END
} // namespace thread
} // namespace aziomq
#endif // AZMQ_THREAD_HPP_

