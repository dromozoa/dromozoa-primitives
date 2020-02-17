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
local sequence_writer = require "dromozoa.commons.sequence_writer"
local string_reader = require "dromozoa.commons.string_reader"
local uint16 = require "dromozoa.commons.uint16"
local uint32 = require "dromozoa.commons.uint32"

local primitives = require "dromozoa.primitives"
-- local encode_dosattrib = primitives.
local uint64 = primitives.uint64
local datetime_to_timespec = primitives.time.datetime_to_timespec
local encode_dosattrib = primitives.time.encode_dosattrib

-- NTTIMEは1601-01-01からの100nsec単位での時間
local T = uint64(11644473600) -- 1601 to 1970

function timespec_to_nttime(sec, nsec)
  return ((uint64(sec) + T) * 10000000 + uint64(nsec) / 100):word()
end

function encode(attrib, create_time_sec, create_time_nsec)
  local out = sequence_writer()

  local attrib_hex = ("0x%x"):format(attrib)
  out:write(attrib_hex, "\0")

  local p = #attrib_hex + 1
  local m = p % 2
  for i = 1, m do
    out:write "\0"
    p = p + 1
  end

  -- version: 3
  out:write(uint16.char(3, "<"))
  out:write(uint16.char(3, "<"))

  local p = p + 4
  local m = p % 4
  for i = 1, m do
    out:write "\0"
    p = p + 1
  end

  -- valid_flags: XATTR_DOSINFO_ATTRIB | XATTR_DOSINFO_CREATE_TIME
  out:write(uint32.char(0x11, "<"))

  -- attrib
  out:write(uint32.char(attrib, "<"))

  -- ea_size: 0
  out:write(uint32.char(0, "<"))

  -- size: 0
  out:write(uint32.char(0, "<"))
  out:write(uint32.char(0, "<"))

  -- alloc_size: 0
  out:write(uint32.char(0, "<"))
  out:write(uint32.char(0, "<"))

  -- create_time
  local lower, upper = timespec_to_nttime(create_time_sec, create_time_nsec)
  out:write(uint32.char(lower, "<"))
  out:write(uint32.char(upper, "<"))

  -- change_time: 0
  out:write(uint32.char(0, "<"))
  out:write(uint32.char(0, "<"))

  return out:concat()
end

local function test(attrib, at)
  local sec, nsec = datetime_to_timespec(at)
  local a = encode(tonumber(attrib, "8"), sec, nsec)
  local b = encode_dosattrib(tonumber(attrib, "8"), sec, nsec)

  print(#a, #b)
  assert(a == b)
end
test("00000022", "2020-01-02T12:34:56.012345Z")

local timer = unix.timer()

local sec, nsec = datetime_to_timespec "2020-01-02T12:34:56.012345Z"

timer:start()
for i = 1, 10000 do
  encode(tonumber("00000022", 8), sec, nsec)
  -- encode_dosattrib(tonumber("00000022", 8), sec, nsec)
end
timer:stop()
print(timer:elapsed())
