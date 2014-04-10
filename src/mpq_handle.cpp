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

extern "C" {
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>
}

#include "common.h"
#include "file_handle.h"

// name of the mpqhandle metatable
const char* MPQHANDLE_UDNAME = "mpqhandle";

// converts a HANDLE to an mpqhandle userdata and pushes it onto the stack
void moonstorm_newmpqhandle(lua_State* L, HANDLE h) {
  HANDLE* udhandle = static_cast<HANDLE*>(lua_newuserdata(L, sizeof(HANDLE)));
  *udhandle = h;
  luaL_setmetatable(L, MPQHANDLE_UDNAME);
}

// ensures that the value on top of the stack is a mpqhandle userdata
HANDLE* moonstorm_checkmpqhandle(lua_State* L, int arg) {
  return static_cast<HANDLE*>(luaL_checkudata(L, arg, MPQHANDLE_UDNAME));
}

int moonstorm_mpq_openfile(lua_State* L) {
  HANDLE* h = moonstorm_checkmpqhandle(L, 1);
  const char* filename = luaL_checkstring(L, 2);
  DWORD search_scope = luaL_optint(L, 3, 0);
  HANDLE file_handle;

  if (SFileOpenFileEx(*h, filename, search_scope, &file_handle)) {
    moonstorm_newfilehandle(L, file_handle);
    return 1;
  } else {
    return moonstorm_push_last_err(L);
  }
}

int moonstorm_mpq_create(lua_State* L) {
  HANDLE* h = moonstorm_checkmpqhandle(L, 1);
  const char* filename = luaL_checkstring(L, 2);
  ULONGLONG filetime = 0;
  DWORD filesize = luaL_checkint(L, 3);
  LCID locale = 0;
  DWORD flags = luaL_checkint(L, 4);
  HANDLE file_handle;

  if (SFileCreateFile(*h, filename, filetime, filesize, locale, flags, &file_handle)) {
    moonstorm_newfilehandle(L, file_handle);
    return 1;
  } else {
    return moonstorm_push_last_err(L);
  }
}

int moonstorm_mpq_flush(lua_State* L) {
  HANDLE* h = moonstorm_checkmpqhandle(L, 1);
  if (!SFileFlushArchive(*h)) {
    return moonstorm_push_last_err(L);
  }
  return 0;
}

int moonstorm_mpq_close(lua_State* L) {
  HANDLE* h = moonstorm_checkmpqhandle(L, 1);
  if (!SFileCloseArchive(*h)) {
    return moonstorm_push_last_err(L);
  }
  return 0;
}

int moonstorm_mpq_hasfile(lua_State* L) {
  HANDLE* h = moonstorm_checkmpqhandle(L, 1);
  const char* filename = luaL_checkstring(L, 2);
  if (SFileHasFile(*h, filename)) {
    lua_pushboolean(L, true);
  } else if (GetLastError() == ERROR_FILE_NOT_FOUND) {
    lua_pushboolean(L, false);
  } else {
    return moonstorm_push_last_err(L);
  }
  return 1;
}

static const struct luaL_Reg mpqhandle_lib[] = {
  {"openfile", moonstorm_mpq_openfile},
  {"createfile", moonstorm_mpq_create},
  {"flush", moonstorm_mpq_flush},
  {"close", moonstorm_mpq_close},
  {"hasfile", moonstorm_mpq_hasfile},
  {NULL, NULL}
};

void moonstorm_init_mpqhandle(lua_State* L) {
  // set up mt for mpqhandles
  luaL_newmetatable(L, MPQHANDLE_UDNAME);
  lua_newtable(L);
  luaL_setfuncs(L, mpqhandle_lib, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}
