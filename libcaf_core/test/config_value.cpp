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

#include "caf/config.hpp"

#define CAF_SUITE config_value
#include "caf/test/unit_test.hpp"

#include <string>

#include "caf/none.hpp"
#include "caf/variant.hpp"
#include "caf/actor_system.hpp"
#include "caf/deep_to_string.hpp"
#include "caf/binary_serializer.hpp"
#include "caf/binary_deserializer.hpp"
#include "caf/actor_system_config.hpp"

using namespace std;
using namespace caf;

struct tostring_visitor : static_visitor<string> {
  template <class T>
  inline string operator()(const T& value) {
    return to_string(value);
  }
};

CAF_TEST(default_constructed) {
  config_value x;
  CAF_CHECK_EQUAL(holds_alternative<int64_t>(x), true);
  CAF_CHECK_EQUAL(get<int64_t>(x), 0);
}

