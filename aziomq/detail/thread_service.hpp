/*
    Copyright (c) 2013-2014 Contributors as noted in the AUTHORS file

    This file is part of aziomq

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
#ifndef AZIOMQ_THREAD_SERVICE_HPP_
#define AZIOMQ_THREAD_SERVICE_HPP_

#include "../error.hpp"
#include "../socket.hpp"
#include "../option.hpp"

#include <boost/assert.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>

namespace aziomq {
namespace detail {
    struct thread_service : boost::asio::io_service::service {
        static boost::asio::io_service::id id;

        static std::string get_uri();

        thread_service(boost::asio::io_service & ios)
            : boost::asio::io_service::service(ios)
        { }

        void shutdown_service() override;

        using is_alive = opt::boolean<static_cast<int>(opt::limits::lib_thread_min)>;
        using detached = opt::boolean<static_cast<int>(opt::limits::lib_thread_min) + 1>;
        using start = opt::boolean<static_cast<int>(opt::limits::lib_thread_min) + 2>;
        using last_error = opt::exception_ptr<static_cast<int>(opt::limits::lib_thread_min) + 3>;

        template<typename T>
        socket make_pipe(bool defer_start, T&& data) {
            auto p = std::make_shared<model<T>>(std::forward<T>(data));
            auto res = p->peer_socket(get_io_service());
            res.associate_ext(handler(std::move(p), defer_start));
            return std::move(res);
        }

    private:
        struct concept : std::enable_shared_from_this<concept> {
            using ptr = std::shared_ptr<concept>;

            boost::asio::io_service io_service_;
            boost::asio::signal_set signals_;
            socket socket_;
            std::thread thread_;

            using lock_type = std::unique_lock<std::mutex>;
            mutable lock_type::mutex_type mutex_;
            mutable std::condition_variable cv_;
            bool ready_;
            bool stopped_;
            std::exception_ptr last_error_;

            concept()
                : socket_(io_service_, ZMQ_PAIR)
                , signals_(io_service_, SIGINT, SIGTERM)
                , ready_(false)
                , stopped_(true)
            {
                socket_.bind(get_uri());
            }

            virtual ~concept() = default;

            socket peer_socket(boost::asio::io_service & peer) {
                socket res(peer, ZMQ_PAIR);
                auto uri = socket_.endpoint();
                BOOST_ASSERT_MSG(!uri.empty(), "uri empty");
                res.connect(uri);
                return res;
            }

            bool joinable() const { return thread_.joinable(); }

            void stop() {
                if (!joinable()) return;
                io_service_.stop();
                thread_.join();
            }

            void stopped() {
                lock_type l{ mutex_ };
                stopped_ = true;
            }

            bool is_stopped() const {
                lock_type l{ mutex_ };
                return stopped_;
            }

            void ready() {
                {
                    lock_type l{ mutex_ };
                    ready_ = true;
                }
                cv_.notify_all();
            }

            void detach() {
                if (!joinable()) return; // already detached
                lock_type l { mutex_ };
                cv_.wait(l, [this] { return ready_; });
                thread_.detach();
            }

            void set_last_error(std::exception_ptr last_error) {
                lock_type l { mutex_ };
                last_error_ = last_error;
            }

            std::exception_ptr last_error() const {
                lock_type l { mutex_ };
                return last_error_;
            }

            virtual void run() = 0;

            static void run(ptr p) {
                lock_type l { p->mutex_ };
                p->signals_.async_wait([p](boost::system::error_code const&, int) {
                    p->io_service_.stop();
                });
                p->stopped_ = false;
                p->thread_ = std::thread([p] {
                    p->ready();
                    try {
                        p->run();
                    } catch (...) {
                        p->set_last_error(std::current_exception());
                    }
                    p->stopped();
                });
            }
        };

        template<typename Function>
        struct model : concept {
            Function data_;

            model(Function data)
                : data_(std::move(data))
            { }

            void run() override { data_(socket_); }
        };

        struct handler {
            concept::ptr p_;
            bool defer_start_;

            handler(concept::ptr p, bool defer_start)
                : p_(std::move(p))
            { }

            void on_install(boost::asio::io_service&, void*) {
                if (defer_start_) return;
                defer_start_ = false;
                concept::run(p_);
            }

            void on_remove() {
                if (defer_start_) return;
                p_->stop();
            }

            template<typename Option>
            boost::system::error_code set_option(Option const& opt,
                                                 boost::system::error_code & ec) {
                switch (opt.name()) {
                case is_alive::static_name::value :
                    ec = make_error_code(boost::system::errc::no_protocol_option);
                    break;
                case detached::static_name::value :
                    {
                        if (*static_cast<detached::value_t const*>(opt.data()))
                            p_->detach();
                    }
                    break;
                case start::static_name::value :
                    {
                        if (*static_cast<start::value_t const*>(opt.data()) && defer_start_) {
                            defer_start_ = false;
                            concept::run(p_);
                        }
                    }
                    break;
                case last_error::static_name::value :
                    ec = make_error_code(boost::system::errc::no_protocol_option);
                    break;
                default:
                    ec = make_error_code(boost::system::errc::not_supported);
                    break;
                }
                return ec;
            }

            template<typename Option>
            boost::system::error_code get_option(Option & opt,
                                                 boost::system::error_code & ec) {
                switch (opt.name()) {
                case is_alive::static_name::value :
                    {
                        auto v = static_cast<is_alive::value_t*>(opt.data());
                        *v = !p_->is_stopped();
                    }
                    break;
                case detached::static_name::value :
                    {
                        auto v = static_cast<detached::value_t*>(opt.data());
                        *v = !p_->joinable();
                    }
                    break;
                case start::static_name::value :
                    ec = make_error_code(boost::system::errc::no_protocol_option);
                    break;
                case last_error::static_name::value :
                    {
                        auto v = static_cast<last_error::value_t*>(opt.data());
                        *v = p_->last_error();
                    }
                    break;
                default:
                    ec = make_error_code(boost::system::errc::not_supported);
                    break;
                }
                return ec;
            }
        };
    };
} // namespace detail
} // namespace aziomq
#endif // AZIOMQ_THREAD_SERVICE_HPP_

