/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright 2011-2018 Dominik Charousset                                     *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#ifndef CAF_IO_BASP_ROUTING_TABLE_HPP
#define CAF_IO_BASP_ROUTING_TABLE_HPP

#include <unordered_map>
#include <unordered_set>

#include "caf/node_id.hpp"
#include "caf/callback.hpp"

#include "caf/io/visitors.hpp"
#include "caf/io/abstract_broker.hpp"

#include "caf/io/basp/buffer_type.hpp"

#include "caf/io/network/interfaces.hpp"

namespace std {

template<>
struct hash<caf::variant<caf::io::connection_handle,caf::io::datagram_handle>> {
  size_t operator()(const caf::variant<caf::io::connection_handle,
                                       caf::io::datagram_handle>& hdl) const {
    return caf::visit(caf::io::hash_visitor{}, hdl);
  }
};

} // namespace std

namespace caf {
namespace io {
namespace basp {

/// @addtogroup BASP

/// Stores routing information for a single broker participating as
/// BASP peer and provides both direct and indirect paths.
class routing_table {
public:
  using endpoint_handle = variant<connection_handle, datagram_handle>;

  explicit routing_table(abstract_broker* parent);

  virtual ~routing_table();

  /// Describes the communication state for a remote endpoint.
  enum communication {
    unavailable,
    pending,
    established,
  };

  /// Describes a function object for erase operations that
  /// is called for each indirectly lost connection.
  using erase_callback = callback<const node_id&>;

  /// Returns the ID of the peer reachable via `hdl` or
  /// `none` if `hdl` is unknown.
  node_id lookup(const endpoint_handle& hdl) const;

  /// Returns the handle for communication with `nid` or
  /// `none` if `nid` is unknown.
  optional<endpoint_handle> lookup(const node_id& nid) const;

  /// Adds a new direct route to the table.
  /// @pre `hdl != invalid_connection_handle && nid != none`
  void add(const endpoint_handle& hdl, const node_id& nid);

  /// Removes a direct connection and calls `cb` for any node
  /// that became unreachable as a result of this operation,
  /// including the node that is assigned as direct path for `hdl`.
  void erase(const endpoint_handle& hdl, erase_callback& cb);

  /// Queries whether `dest` is reachable.
  bool reachable(const node_id& dest);

  /// Returns the parent broker.
  inline abstract_broker* parent() {
    return parent_;
  }

  /// Set the communication state of node with `nid`.
  bool status(const node_id& nid, communication new_status);

  /// Get the communication state of node with `nid`.
  optional<communication> status(const node_id& nid);

  /// Set the forwarding node that first mentioned `hdl`.
  bool forwarder(const node_id& nid, endpoint_handle hdl);

  /// Get the forwarding node that first mentioned `hdl`.
  optional<endpoint_handle> forwarder(const node_id& nid);

  /// Add `addrs` to the addresses to reach `nid`.
  bool addresses(const node_id& nid, network::address_listing addrs);

  /// Get the addresses to reach `nid`.
  optional<const network::address_listing&> addresses(const node_id& nid);

public:
  /// Entry to bundle information for a remote endpoint.
  struct node_info {
    // Tracks the state to determine if we can sent messages or have to buffer.
    communication stat;
    // Interfaces of the nodes for sharing with neighbors.
    network::address_listing addrs;
    // The endpoint who told us about the node.
    optional<endpoint_handle> hdl;
  };

  template <class Map, class Fallback>
  typename Map::mapped_type
  get_opt(const Map& m, const typename Map::key_type& k, Fallback&& x) const {
    auto i = m.find(k);
    if (i != m.end())
      return i->second;
    return std::forward<Fallback>(x);
  }

  abstract_broker* parent_;
  std::unordered_map<endpoint_handle, node_id> direct_by_hdl_;
  // TODO: do we need a list as a second argument as there could be
  //       multiple handles for different technologies?
  std::unordered_map<node_id, endpoint_handle> direct_by_nid_;

  std::unordered_map<node_id, node_info> node_information_base_;
};

/// @}

} // namespace basp
} // namespace io
} // namespace caf

#endif // CAF_IO_BASP_ROUTING_TABLE_HPP
