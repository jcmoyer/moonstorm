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

#include <StormLib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "file_handle.h"
#include "mpq_handle.h"
#include "common.h"

// moonstorm.open(filename, [flags=0])
int moonstorm_open(lua_State* L) {
  const char* filename = luaL_checkstring(L, 1);
  DWORD flags = luaL_optint(L, 2, 0);
  HANDLE mpq_handle;

  if (SFileOpenArchive(filename, 0, flags, &mpq_handle)) {
    moonstorm_newmpqhandle(L, mpq_handle);
    return 1;
  } else {
    return moonstorm_push_last_err(L);
  }
}

// moonstorm.create(filename, [flags=0,] max_file_count)
int moonstorm_create(lua_State* L) {
  const char* filename = luaL_checkstring(L, 1);
  DWORD flags, max_file_count;
  HANDLE mpq_handle;

  if (lua_gettop(L) <= 2) {
    max_file_count = luaL_checkint(L, 2);
  } else {
    flags = luaL_checkint(L, 2);
    max_file_count = luaL_checkint(L, 3);
  }

  if (SFileCreateArchive(filename, flags, max_file_count, &mpq_handle)) {
    moonstorm_newmpqhandle(L, mpq_handle);
    return 1;
  } else {
    return moonstorm_push_last_err(L);
  }
}

// moonstorm.setlocale(lcid)
int moonstorm_setlocale(lua_State* L) {
  LCID lcid = luaL_checkinteger(L, 1);
  lua_pushinteger(L, SFileSetLocale(lcid));
  return 1;
}

// moonstorm.getlocale()
int moonstorm_getlocale(lua_State* L) {
  LCID lcid = SFileGetLocale();
  lua_pushinteger(L, lcid);
  return 1;
}

static const struct luaL_Reg moonstorm_lib[] = {
  {"open", moonstorm_open},
  {"create", moonstorm_create},
  {"setlocale", moonstorm_setlocale},
  {"getlocale", moonstorm_getlocale},
  {NULL, NULL}
};

// C linkage required for lua to see this function
int luaopen_moonstorm(lua_State* L) {
  moonstorm_init_mpqhandle(L);
  moonstorm_init_filehandle(L);

  lua_newtable(L);
  luaL_setfuncs(L, moonstorm_lib, 0);

  return 1;
}
