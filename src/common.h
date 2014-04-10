//  Copyright 2014 J.C. Moyer
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef COMMON_H
#define COMMON_H

#include <StormLib.h>

extern "C" {
  #include <lua.h>
}

template <typename T>
inline lua_Integer make_long_lua_int(T high, T low) {
  return (((lua_Integer)high) << 32) | (lua_Integer)low;
}

template <typename T>
inline void extract_long_lua_int(lua_Integer i, T& high, T& low) {
  low  = i & 0xFFFFFFFF;
  high = (i >> 32) & 0xFFFFFFFF;
}

const char* storm_errstr(DWORD e);
void moonstorm_push_errstr(lua_State* L, DWORD e);
int moonstorm_push_last_err(lua_State* L);

#endif
