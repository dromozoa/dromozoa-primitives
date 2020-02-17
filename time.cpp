// Copyright (C) 2020 Tomoyuki Fujimori <moyu@dromozoa.com>
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
#include <math.h>
#include <string.h>
#include <iostream>
#include <sstream>

namespace dromozoa {
  namespace {
    inline bool is_digit(char c) {
      return '0' <= c && c <= '9';
    }

    inline uint64_t encode_unix_time(int year, int month, int day, int hour, int min, int sec) {
      if (month < 3) {
        --year;
        month += 13;
      } else {
        ++month;
      }

      double A = floor(year / 100.0);
      double time = floor(365.25 * year) + floor(30.6001 * month) + day - 719591 - A + floor(A / 4);
      return time * 86400 + hour * 3600 + min * 60 + sec;
    }

    void impl_datetime_to_timespec(lua_State* L) {
      luaX_string_reference s = luaX_check_string(L, 1);

      int year = 0;
      int month = 0;
      int day = 0;
      int hour = 0;
      int min = 0;
      int sec = 0;
      uint32_t nsec = 0;
      size_t ncnt = 0;
      int offset = 0;
      bool offneg = false;

      int state = 1;
      for (size_t i = 0; i < s.size(); ++i) {
        char c = s.data()[i];
        switch (state) {
          case 1:
            if (c == '-') { state = 2; } else if (is_digit(c)) { year = year * 10 + (c - '0'); } else { state = 0; }
            break;
          case 2:
            if (c == '-') { state = 3; } else if (is_digit(c)) { month = month * 10 + (c - '0'); } else { state = 0; }
            break;
          case 3:
            if (c == 'T' || c == ' ') { state = 4; } else if (is_digit(c)) { day = day * 10 + (c - '0'); } else { state = 0; }
            break;
          case 4:
            if (c == ':') { state = 5; } else if (is_digit(c)) { hour = hour * 10 + (c - '0'); } else { state = 0; }
            break;
          case 5:
            if (c == ':') { state = 6; } else if (is_digit(c)) { min = min * 10 + (c - '0'); } else { state = 0; }
            break;
          case 6:
            if (c == '.') { state = 7; } else if (is_digit(c)) { sec = sec * 10 + (c - '0'); } else { state = 0; }
            break;
          case 7:
            if (c == 'Z') { state = 8; }
            else if (c == '+') { state = 9; }
            else if (c == '-') { state = 9; offneg = true; }
            else if (is_digit(c)) { nsec = nsec * 10 + (c - '0'); ++ncnt; }
            break;
          case 8:
            break;
          case 9:
            if (c == ':') { state = 10; } else if (is_digit(c)) { offset = offset * 10 + (c - '0'); } else { state = 0; }
            break;
          case 10:
            if (is_digit(c)) { offset = offset * 10 + (c - '0'); } else { state = 0; }
            break;
        }
        if (state == 8) {
          break;
        }
        if (state == 0) {
          // error
          luaX_push(L, luaX_nil, "parser error");
          return;
        }
      }

      for (size_t i = ncnt; i < 9; ++i) {
        nsec *= 10;
      }

      offset = (offset / 100) * 3600 + (offset % 100) * 60;
      if (offneg) {
        offset = -offset;
      }

      uint64_t result = encode_unix_time(year, month, day, hour, min, sec) - offset;
      luaX_push(L, result, nsec);
    }

    void write_uint16(std::ostream& out, uint16_t v) {
      uint8_t a = v & 0xFF;
      v >>= 8;
      uint8_t b = v & 0xFF;
      out << static_cast<char>(a)
          << static_cast<char>(b);
    }

    void write_uint32(std::ostream& out, uint32_t v) {
      uint8_t a = v & 0xFF;
      v >>= 8;
      uint8_t b = v & 0xFF;
      v >>= 8;
      uint8_t c = v & 0xFF;
      v >>= 8;
      uint8_t d = v & 0xFF;
      out << static_cast<char>(a)
          << static_cast<char>(b)
          << static_cast<char>(c)
          << static_cast<char>(d);
    }

    void write_uint64(std::ostream& out, uint64_t v) {
      uint8_t a = v & 0xFF;
      v >>= 8;
      uint8_t b = v & 0xFF;
      v >>= 8;
      uint8_t c = v & 0xFF;
      v >>= 8;
      uint8_t d = v & 0xFF;
      v >>= 8;
      uint8_t e = v & 0xFF;
      v >>= 8;
      uint8_t f = v & 0xFF;
      v >>= 8;
      uint8_t g = v & 0xFF;
      v >>= 8;
      uint8_t h = v & 0xFF;
      out << static_cast<char>(a)
          << static_cast<char>(b)
          << static_cast<char>(c)
          << static_cast<char>(d)
          << static_cast<char>(e)
          << static_cast<char>(f)
          << static_cast<char>(g)
          << static_cast<char>(h);
    }


    void impl_encode_dosattrib(lua_State* L) {
      uint32_t attribute = luaX_check_integer<uint32_t>(L, 1);
      uint64_t sec = luaX_check_integer<uint64_t>(L, 2);
      uint64_t nsec = luaX_opt_integer<uint64_t>(L, 3, 0);

      char buffer[256] = { '\0' };
      snprintf(buffer, 256, "0x%x", attribute);

      std::ostringstream out;
      out << buffer << '\0';

      size_t p = strlen(buffer) + 1;
      size_t m = p % 2;
      for (size_t i = 1; i <= m; ++i) {
        out << '\0';
        ++p;
      }

      write_uint16(out, 3);
      write_uint16(out, 3);

      p += 4;
      m = p % 4;
      for (size_t i = 1; i <= m; ++i) {
        out << '\0';
        ++p;
      }

      write_uint32(out, 0x11);
      write_uint32(out, attribute);
      write_uint32(out, 0);
      write_uint32(out, 0);
      write_uint32(out, 0);
      write_uint32(out, 0);
      write_uint32(out, 0);

      static const uint64_t T = 11644473600ull;
      uint64_t nttime = (sec + T) * 10000000 + nsec / 100;

      // write_uint32(out, nttime & 0xFFFFFFFF);
      // write_uint32(out, nttime >> 32);
      write_uint64(out, nttime);

      // std::cout
      //     << (nttime & 0xFFFFFFFF) << "\n"
      //     << (nttime >> 32) << "\n";


/*
-- NTTIMEは1601-01-01からの100nsec単位での時間
local T = uint64(11644473600) -- 1601 to 1970

function timespec_to_nttime(sec, nsec)
  return ((uint64(sec) + T) * 10000000 + uint64(nsec) / 100):word()
end
*/


      write_uint32(out, 0);
      write_uint32(out, 0);

      luaX_push(L, out.str());

/*
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
*/


    }
  }

  void initialize_time(lua_State* L) {
    lua_newtable(L);
    {
      luaX_set_field(L, -1, "datetime_to_timespec", impl_datetime_to_timespec);
      luaX_set_field(L, -1, "encode_dosattrib", impl_encode_dosattrib);
    }
    luaX_set_field(L, -2, "time");
  }
}
