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

#ifndef MPQ_HANDLE_H
#define MPQ_HANDLE_H

#include <StormLib.h>
#include <lua.h>

void moonstorm_newmpqhandle(lua_State* L, HANDLE h);
HANDLE* moonstorm_checkmpqhandle(lua_State* L, int arg);

int moonstorm_mpq_open(lua_State* L);
int moonstorm_mpq_create(lua_State* L);
int moonstorm_mpq_flush(lua_State* L);
int moonstorm_mpq_close(lua_State* L);
int moonstorm_mpq_hasfile(lua_State* L);

void moonstorm_init_mpqhandle(lua_State* L);

#endif
