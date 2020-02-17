// Copyright (C) 2019,2020 Tomoyuki Fujimori <moyu@dromozoa.com>
//
// This file is part of dromozoa-primitives.
//
// dromozoa-primitives is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dromozoa-primitives is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dromozoa-primitives.  If not, see <http://www.gnu.org/licenses/>.

#include "common.hpp"

namespace dromozoa {
  void initialize_time(lua_State*);
  void initialize_uint64(lua_State*);

  void initialize(lua_State* L) {
    initialize_time(L);
    initialize_uint64(L);
  }
}

extern "C" int luaopen_dromozoa_primitives(lua_State* L) {
  lua_newtable(L);
  dromozoa::initialize(L);
  return 1;
}
