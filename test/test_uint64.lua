-- Copyright (C) 2019 Tomoyuki Fujimori <moyu@dromozoa.com>
--
-- This file is part of dromozoa-primitives.
--
-- dromozoa-primitives is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- dromozoa-primitives is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with dromozoa-primitives.  If not, see <http://www.gnu.org/licenses/>.

local primitives = require "dromozoa.primitives"

local uint64 = primitives.uint64

local u = uint64()
assert(tostring(u) == "0")
local u = uint64(1)
assert(tostring(u) == "1")

local u = uint64(1, 1)
assert(tostring(u) == "4294967297")
local u = uint64(1, 3)
assert(tostring(u) == "12884901889")

local u = uint64(0xFFFFFFFF, 0x7FFFFFFF)
local v = uint64(0x00000001, 0x00000000)
local w = uint64(0x00000000, 0x80000000)
assert(tostring(u + v) == tostring(w))
assert(u + v == w)

local u = uint64(0xFFFFFFFF, 0xFFFFFFFF)
local v = uint64(0x00000001, 0x00000000)
local w = uint64(0x00000000, 0x00000000)
assert(tostring(u + v) == tostring(w))
assert(u ~= v)
assert(u + v == w)
assert(w < v)
assert(w <= v)
assert(u + 1 == w)

local u = uint64(42)
local v = uint64(17)
assert(u * v == uint64(714))
assert(u / v == uint64(2))
assert(u % v == uint64(8))

assert(uint64(0x56789ABC, 0x00001234):tonumber() == 0x123456789ABC)
assert(uint64(0x56789ABC, 0x00001234) == uint64(0x123456789ABC))

local lower, upper = uint64(0x9ABCDEF0, 0x12345678):word()
assert(lower == 0x9ABCDEF0)
assert(upper == 0x12345678)
