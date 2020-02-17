-- Copyright (C) 2020 Tomoyuki Fujimori <moyu@dromozoa.com>
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

local unix = require "dromozoa.unix"
local datetime = require "dromozoa.commons.datetime"
local unix_time = require "dromozoa.commons.unix_time"

local primitives = require "dromozoa.primitives"
local datetime_to_timespec = primitives.time.datetime_to_timespec

local function timespec(at)
  local d, offset = datetime.decode(at)
  local s = unix_time.encode(d, offset)
  local n = d.nsec
  if not n then
    n = 0
  end
  return s, n
  -- return {
  --   tv_sec = s;
  --   tv_nsec = n;
  -- }
end

local function test(at)
  local a, b = timespec(at)
  local c, d = datetime_to_timespec(at)
  print(a, b, c, d)
  assert(a == c)
  assert(b == d)
end
test "2020-01-02T12:34:56.012345Z"
test "2020-01-02 03:34:56.012345+0900"
test "2020-01-02 03:34:56.012345+09:00"
test "2020-01-02T02:34:56.012345-1030"

local timer = unix.timer()

timer:start()
for i = 1, 10000 do
  -- timespec "2020-01-02T12:34:56.012345Z"
  datetime_to_timespec "2020-01-02T12:34:56.012345Z"
end
timer:stop()
print(timer:elapsed())
