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

#include "msio.h"
#include <lauxlib.h>

ms_handle* ms_newhandle(lua_State* L, HANDLE handle) {
  ms_handle* wrapper = lua_newuserdata(L, sizeof(ms_handle));
  wrapper->handle = handle;
  wrapper->status = msh_open;
  wrapper->parent = NULL;
  return wrapper;
}

ms_handle* ms_checkhandle(lua_State* L, int arg) {
  ms_handle* h = lua_touserdata(L, arg);
  ms_handle* which = h;
  while (which != NULL) {
    if (which->status != msh_open) {
      luaL_error(L, "attempt to use a closed handle or parent");
    }
    which = which->parent;
  }
  return h;
}
