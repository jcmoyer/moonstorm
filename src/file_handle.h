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

#ifndef FILE_HANDLE_H
#define FILE_HANDLE_H

#include <StormLib.h>
#include <lua.h>

void moonstorm_newfilehandle(lua_State* L, HANDLE h);
int moonstorm_mpq_file_size(lua_State* L);
int moonstorm_mpq_file_name(lua_State* L);
int moonstorm_mpq_file_read(lua_State* L);
int moonstorm_mpq_file_write(lua_State* L);
int moonstorm_mpq_file_close(lua_State* L);
void moonstorm_init_filehandle(lua_State* L);

#endif
