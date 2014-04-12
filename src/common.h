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
#include <lua.h>

lua_Integer make_long_lua_int(DWORD high, DWORD low);
void extract_long_lua_int(lua_Integer i, DWORD* high, DWORD* low);

const char* storm_errstr(DWORD e, char* buffer, DWORD size);
void moonstorm_push_errstr(lua_State* L, DWORD e);
int moonstorm_push_last_err(lua_State* L);

#endif
