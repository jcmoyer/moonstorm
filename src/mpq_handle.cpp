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

// mpq:addlistfile(filename)
int moonstorm_mpq_addlistfile(lua_State* L) {
  HANDLE* h = moonstorm_checkmpqhandle(L, 1);
  const char* filename = luaL_checkstring(L, 2);
  int result = SFileAddListFile(*h, filename);
  if (result == ERROR_SUCCESS) {
    lua_pushboolean(L, true);
    return 1;
  } else {
    lua_pushnil(L);
    lua_pushinteger(L, result);
    return 2;
  }
}

// mpq:openfile(filename [, search_scope=0])
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

// mpq:create(filename, filesize, flags)
int moonstorm_mpq_createfile(lua_State* L) {
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

// mpq:flush()
int moonstorm_mpq_flush(lua_State* L) {
  HANDLE* h = moonstorm_checkmpqhandle(L, 1);
  if (!SFileFlushArchive(*h)) {
    return moonstorm_push_last_err(L);
  }
  return 0;
}

// mpq:close()
int moonstorm_mpq_close(lua_State* L) {
  HANDLE* h = moonstorm_checkmpqhandle(L, 1);
  if (!SFileCloseArchive(*h)) {
    return moonstorm_push_last_err(L);
  }
  return 0;
}

// mpq:hasfile(filename)
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

// mpq:setmaxfilecount(n)
int moonstorm_mpq_setmaxfilecount(lua_State* L) {
  HANDLE* h = moonstorm_checkmpqhandle(L, 1);
  DWORD maxfilecount = luaL_checkint(L, 2);
  if (!SFileSetMaxFileCount(*h, maxfilecount)) {
    return moonstorm_push_last_err(L);
  }
  return 0;
}

// docs say this takes LARGE_INTEGER* but source code says ULONGLONG
void WINAPI moonstorm_mpq_compact_cb(void* userdata, DWORD worktype,
  ULONGLONG processed, ULONGLONG total) {
  lua_State* L = static_cast<lua_State*>(userdata);

  // dup function
  lua_pushvalue(L, -1);

  lua_pushinteger(L, worktype);
  lua_pushinteger(L, processed);
  lua_pushinteger(L, total);

  int result = lua_pcall(L, 3, 0, 0);
  if (result != LUA_OK) {
    // TODO: better way to handle errors instead of swallowing them?
    lua_pop(L, 1);
  }
}

// mpq:compact([listfile] [, callback])
int moonstorm_mpq_compact(lua_State* L) {
  HANDLE* h = moonstorm_checkmpqhandle(L, 1);
  const char* listfile = NULL;
  int func = 0;

  if (lua_isstring(L, 2)) {
    listfile = lua_tostring(L, 2);
  } else if (lua_isfunction(L, 2)) {
    func = 2;
  }
  // if #2 is a string, try #3 for a function
  if (func == 0 && listfile && lua_isfunction(L, 3)) {
    func = 3;
  }

  if (func > 0) {
    // func is already on top of the stack so we'll just take it from there in
    // the callback
    SFileSetCompactCallback(*h, moonstorm_mpq_compact_cb, static_cast<void*>(L));
  }

  if (SFileCompactArchive(*h, listfile, false)) {
    return 0;
  } else {
    return moonstorm_push_last_err(L);
  }
}

static const struct luaL_Reg mpqhandle_lib[] = {
  {"addlistfile", moonstorm_mpq_addlistfile},
  {"openfile", moonstorm_mpq_openfile},
  {"createfile", moonstorm_mpq_createfile},
  {"flush", moonstorm_mpq_flush},
  {"close", moonstorm_mpq_close},
  {"hasfile", moonstorm_mpq_hasfile},
  {"setmaxfilecount", moonstorm_mpq_setmaxfilecount},
  {"compact", moonstorm_mpq_compact},
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
