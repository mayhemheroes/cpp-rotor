#pragma once

//
// Copyright (c) 2019 Ivan Baidakou (basiliscos) (the dot dmol at gmail dot com)
//
// Distributed under the MIT Software License
//

#include "rotor/actor_base.h"
#include "rotor/asio/supervisor_asio.h"
#include <boost/asio.hpp>
#include <tuple>

namespace rotor {

namespace asio {

namespace asio = boost::asio;

/** \brief return `strand` of the boost::asio aware actor */
template <typename Actor> inline boost::asio::io_context::strand &get_strand(Actor &actor);

template <typename Actor, typename Handler, typename ErrHandler = void> struct forwarder_t;
/**
 * \brief dispatches single boost::asio callback with `error_code` and non-error results
 * into two differnt functions of the `actor`. After the invocation, actor's supervisor
 * `do_process` method is called to process message queue.
 *
 * The invocation is `strand`-aware.
 *
 */
template <typename Actor, typename Handler, typename ErrHandler> struct forwarder_t {
    /** \brief alias for intrusive pointer for an actor type */
    using typed_actor_ptr_t = intrusive_ptr_t<Actor>;

    /** \brief alias for boost::asio supervior */
    using typed_sup_t = supervisor_asio_t;

    /** \brief constructs forwarder for the actor from it's member functions: positive
     * result handler and error handler functions.
     */
    forwarder_t(Actor &actor_, Handler &&handler_, ErrHandler &&err_handler_)
        : typed_actor{&actor_}, handler{std::move(handler_)}, err_handler{std::move(err_handler_)} {}

    /** \brief mimics boost::asio handler, which will be forwarded/decomposed into
     * two different methods of the actor
     */
    template <typename T = void> inline void operator()(const boost::system::error_code &ec) noexcept {
        auto &sup = static_cast<typed_sup_t &>(typed_actor->get_supervisor());
        auto &strand = get_strand(sup);
        if (ec) {
            asio::defer(strand, [actor = typed_actor, handler = std::move(err_handler), ec = ec]() {
                ((*actor).*handler)(ec);
                actor->get_supervisor().do_process();
            });
        } else {
            asio::defer(strand, [actor = typed_actor, handler = std::move(handler)]() {
                ((*actor).*handler)();
                actor->get_supervisor().do_process();
            });
        }
    }

    /** \brief mimics boost::asio handler, which will be forwarded/decomposed into
     * two different methods of the actor
     */
    template <typename T> inline void operator()(const boost::system::error_code &ec, T arg) noexcept {
        auto &sup = static_cast<typed_sup_t &>(typed_actor->get_supervisor());
        auto &strand = get_strand(sup);
        if (ec) {
            asio::defer(strand, [actor = typed_actor, handler = std::move(err_handler), ec = ec]() {
                ((*actor).*handler)(ec);
                actor->get_supervisor().do_process();
            });
        } else {
            asio::defer(strand, [actor = typed_actor, handler = std::move(handler), arg = arg]() {
                ((*actor).*handler)(arg);
                actor->get_supervisor().do_process();
            });
        }
    }

    /** intrusive pointer to the actor */
    typed_actor_ptr_t typed_actor;

    /** pointer-to-member function of the actor to handle positive case result */
    Handler handler;

    /** pointer-to-member function of the actor to handle negative case result, i.e. `error_code` */
    ErrHandler err_handler;
};

/** \brief forwarder specialization, when error case is not possible  */
template <typename Actor, typename Handler> struct forwarder_t<Actor, Handler, void> {
    /** \brief alias for intrusive pointer for an actor type */
    using typed_actor_ptr_t = intrusive_ptr_t<Actor>;

    /** \brief alias for boost::asio supervior */
    using typed_sup_t = supervisor_asio_t;

    /** \brief constructs forwarder for the actor from it handler function */
    forwarder_t(Actor &actor_, Handler &&handler_) : typed_actor{&actor_}, handler{std::move(handler_)} {}

    /** \brief mimics boost::asio handler, which will be forwarded to the appropriate
     * actor's member method.
     */
    template <typename T> inline void operator()(T arg) noexcept {
        auto &sup = static_cast<typed_sup_t &>(typed_actor->get_supervisor());
        auto &strand = get_strand(sup);
        asio::defer(strand, [actor = typed_actor, handler = std::move(handler), arg = arg]() {
            ((*actor).*handler)(arg);
            actor->get_supervisor().do_process();
        });
    }

    /** \brief mimics boost::asio handler, which will be forwarded/decomposed into
     * two different methods of the actor
     */
    template <typename T = void> inline void operator()() noexcept {
        auto &sup = static_cast<typed_sup_t &>(typed_actor->get_supervisor());
        auto &strand = get_strand(sup);
        asio::defer(strand, [actor = typed_actor, handler = std::move(handler)]() {
            ((*actor).*handler)();
            actor->get_supervisor().do_process();
        });
    }

    /** intrusive pointer to the actor */
    typed_actor_ptr_t typed_actor;

    /** pointer-to-member function of the actor to handle positive case result */
    Handler handler;
};

/** construtor deduction guide for forwarder */
template <typename Actor, typename Handler>
forwarder_t(Actor &actor_, Handler &&handler_)->forwarder_t<Actor, Handler, void>;

} // namespace asio

} // namespace rotor
