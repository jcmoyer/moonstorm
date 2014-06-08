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

#ifndef MSIO_H
#define MSIO_H

#include <StormLib.h>
#include <lua.h>

typedef enum {
  MSH_OPEN,
  MSH_CLOSED
} ms_handle_stat;

typedef struct ms_handle {
  HANDLE            handle;
  ms_handle_stat    status;
  struct ms_handle* parent;
} ms_handle;

// converts a HANDLE to an ms_handle and pushes it onto the stack as a full
// userdata
ms_handle* ms_newhandle(lua_State* L, HANDLE handle);
ms_handle* ms_tohandle(lua_State* L, int arg);
ms_handle* ms_checkhandle(lua_State* L, int arg);

bool ms_isopen(ms_handle* handle);

#endif
