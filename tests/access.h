//
// Copyright (c) 2020 Ivan Baidakou (basiliscos) (the dot dmol at gmail dot com)
//
// Distributed under the MIT Software License
//

#pragma once

#include "rotor.hpp"

namespace rotor::test {

namespace {
namespace to {
struct get_plugin {};
struct state {};
struct internal_infos {};
struct mine_handlers {};
struct actors_map {};
struct points {};
struct locality_leader {};
struct parent_supervisor {};
struct registry {};
struct queue {};
struct own_subscriptions {};
struct request_map {};
struct resources {};
struct last_req_id {};
} // namespace to
} // namespace

bool empty(rotor::subscription_t &subs) noexcept;

} // namespace rotor::test

namespace rotor {

template <> inline auto &actor_base_t::access<test::to::state>() noexcept { return state; }
template <> inline auto &actor_base_t::access<test::to::resources>() noexcept { return resources; }

template <> inline auto rotor::actor_base_t::access<test::to::get_plugin, const void *>(const void *identity) noexcept {
    return get_plugin(identity);
}

template <> inline auto &rotor::subscription_t::access<test::to::internal_infos>() noexcept { return internal_infos; }
template <> inline auto &rotor::subscription_t::access<test::to::mine_handlers>() noexcept { return mine_handlers; }
template <> inline auto &rotor::plugin::plugin_base_t::access<test::to::own_subscriptions>() noexcept {
    return own_subscriptions;
}
template <> inline auto &plugin::child_manager_plugin_t::access<test::to::actors_map>() noexcept { return actors_map; }
template <> inline auto &plugin::lifetime_plugin_t::access<test::to::points>() noexcept { return points; }
template <> inline auto &plugin::resources_plugin_t::access<test::to::resources>() noexcept { return resources; }
template <> inline auto &rotor::supervisor_t::access<test::to::locality_leader>() noexcept { return locality_leader; }
template <> inline auto &rotor::supervisor_t::access<test::to::parent_supervisor>() noexcept { return parent; }
template <> inline auto &rotor::supervisor_t::access<test::to::registry>() noexcept { return registry_address; }
template <> inline auto &rotor::supervisor_t::access<test::to::queue>() noexcept { return queue; }
template <> inline auto &rotor::supervisor_t::access<test::to::request_map>() noexcept { return request_map; }
template <> inline auto &rotor::supervisor_t::access<test::to::last_req_id>() noexcept { return last_req_id; }

} // namespace rotor
