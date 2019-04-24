// Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
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

#include <stdint.h>

#include <sstream>

namespace dromozoa {
  namespace {
    void new_uint64(lua_State* L, uint64_t value) {
      luaX_new<uint64_t>(L, value);
      luaX_set_metatable(L, "dromozoa.primitives.uint64");
    }

    uint64_t check_uint64(lua_State* L, int arg) {
      if (lua_isuserdata(L, arg)) {
        return *luaX_check_udata<uint64_t>(L, arg, "dromozoa.primitives.uint64");
      } else {
        return luaX_check_integer<uint64_t>(L, arg);
      }
    }

    void impl_call(lua_State* L) {
      uint64_t lower = luaX_opt_integer<uint64_t>(L, 2, 0);
      uint64_t upper = luaX_opt_integer<uint32_t>(L, 3, 0);
      new_uint64(L, upper << 32 | lower);
    }

    void impl_add(lua_State* L) {
      uint64_t u = check_uint64(L, 1);
      uint64_t v = check_uint64(L, 2);
      new_uint64(L, u + v);
    }

    void impl_sub(lua_State* L) {
      uint64_t u = check_uint64(L, 1);
      uint64_t v = check_uint64(L, 2);
      new_uint64(L, u - v);
    }

    void impl_mul(lua_State* L) {
      uint64_t u = check_uint64(L, 1);
      uint64_t v = check_uint64(L, 2);
      new_uint64(L, u * v);
    }

    void impl_div(lua_State* L) {
      uint64_t u = check_uint64(L, 1);
      uint64_t v = check_uint64(L, 2);
      new_uint64(L, u / v);
    }

    void impl_mod(lua_State* L) {
      uint64_t u = check_uint64(L, 1);
      uint64_t v = check_uint64(L, 2);
      new_uint64(L, u % v);
    }

    void impl_eq(lua_State* L) {
      uint64_t u = check_uint64(L, 1);
      uint64_t v = check_uint64(L, 2);
      luaX_push(L, u == v);
    }

    void impl_lt(lua_State* L) {
      uint64_t u = check_uint64(L, 1);
      uint64_t v = check_uint64(L, 2);
      luaX_push(L, u < v);
    }

    void impl_le(lua_State* L) {
      uint64_t u = check_uint64(L, 1);
      uint64_t v = check_uint64(L, 2);
      luaX_push(L, u <= v);
    }

    void impl_tostring(lua_State* L) {
      uint64_t u = check_uint64(L, 1);
      std::ostringstream out;
      out << u;
      luaX_push(L, out.str());
    }

    void impl_tonumber(lua_State* L) {
      uint64_t u = check_uint64(L, 1);
      luaX_push(L, u);
    }

    void impl_word(lua_State* L) {
      uint64_t self = check_uint64(L, 1);
      uint64_t upper = self >> 32;
      uint64_t lower = self & 0xFFFFFFFF;
      luaX_push(L, lower, upper);
    }
  }

  void initialize_uint64(lua_State* L) {
    lua_newtable(L);
    {
      luaL_newmetatable(L, "dromozoa.primitives.uint64");
      lua_pushvalue(L, -2);
      luaX_set_field(L, -2, "__index");
      luaX_set_field(L, -1, "__add", impl_add);
      luaX_set_field(L, -1, "__sub", impl_sub);
      luaX_set_field(L, -1, "__mul", impl_mul);
      luaX_set_field(L, -1, "__div", impl_div);
      luaX_set_field(L, -1, "__mod", impl_mod);
      luaX_set_field(L, -1, "__eq", impl_eq);
      luaX_set_field(L, -1, "__lt", impl_lt);
      luaX_set_field(L, -1, "__le", impl_le);
      luaX_set_field(L, -1, "__tostring", impl_tostring);
      lua_pop(L, 1);

      luaX_set_metafield(L, -1, "__call", impl_call);
      luaX_set_field(L, -1, "add", impl_add);
      luaX_set_field(L, -1, "sub", impl_sub);
      luaX_set_field(L, -1, "mul", impl_mul);
      luaX_set_field(L, -1, "div", impl_div);
      luaX_set_field(L, -1, "mod", impl_mod);
      luaX_set_field(L, -1, "eq", impl_eq);
      luaX_set_field(L, -1, "lt", impl_lt);
      luaX_set_field(L, -1, "le", impl_le);
      luaX_set_field(L, -1, "tostring", impl_tostring);
      luaX_set_field(L, -1, "tonumber", impl_tonumber);
      luaX_set_field(L, -1, "word", impl_word);
    }
    luaX_set_field(L, -2, "uint64");
  }
}
