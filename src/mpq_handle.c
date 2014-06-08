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
#include "common.h"
#include "file_handle.h"
#include "msio.h"

// name of the mpqhandle metatable
static const char* MPQHANDLE_UDNAME = "mpqhandle";

// converts a HANDLE to an mpqhandle userdata and pushes it onto the stack
void ms_newmpqhandle(lua_State* L, HANDLE h) {
  ms_newhandle(L, h);
  luaL_setmetatable(L, MPQHANDLE_UDNAME);
}

// tests whether or not the given userdata has the proper metatable
bool ms_ismpqhandle(lua_State* L, int index) {
  return luaL_testudata(L, index, MPQHANDLE_UDNAME) != NULL;
}

ms_handle* ms_tompqhandle(lua_State* L, int index) {
  if (ms_ismpqhandle(L, index)) {
    return lua_touserdata(L, index);
  } else {
    return NULL;
  }
}

ms_handle* ms_checkmpqhandle(lua_State* L, int arg) {
  // ensure proper metatable
  luaL_checkudata(L, arg, MPQHANDLE_UDNAME);
  // ensure handle is valid
  return ms_checkhandle(L, arg);
}

// mpq:addlistfile(filename)
int ms_mpq_addlistfile(lua_State* L) {
  ms_handle* h = ms_checkmpqhandle(L, 1);
  const char* filename = luaL_checkstring(L, 2);
  int result = SFileAddListFile(h->handle, filename);
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
int ms_mpq_openfile(lua_State* L) {
  ms_handle* h = ms_checkmpqhandle(L, 1);
  const char* filename = luaL_checkstring(L, 2);
  DWORD search_scope = luaL_optint(L, 3, 0);
  HANDLE file_handle;

  if (SFileOpenFileEx(h->handle, filename, search_scope, &file_handle)) {
    ms_handle* f = ms_newfilehandle(L, file_handle);
    f->parent = h;
    return 1;
  } else {
    return ms_push_last_err(L);
  }
}

// mpq:create(filename, filesize, flags)
int ms_mpq_createfile(lua_State* L) {
  ms_handle* h = ms_checkmpqhandle(L, 1);
  const char* filename = luaL_checkstring(L, 2);
  ULONGLONG filetime = 0;
  DWORD filesize = luaL_checkint(L, 3);
  LCID locale = 0;
  DWORD flags = luaL_checkint(L, 4);
  HANDLE file_handle;

  if (SFileCreateFile(h->handle, filename, filetime, filesize, locale, flags, &file_handle)) {
    ms_handle* f = ms_newfilehandle(L, file_handle);
    f->parent = h;
    return 1;
  } else {
    return ms_push_last_err(L);
  }
}

// mpq:flush()
int ms_mpq_flush(lua_State* L) {
  ms_handle* h = ms_checkmpqhandle(L, 1);
  if (SFileFlushArchive(h->handle)) {
    lua_pushboolean(L, true);
    return 1;
  } else {
    return ms_push_last_err(L);
  }
}

// mpq:close()
int ms_mpq_close(lua_State* L) {
  ms_handle* h = ms_checkmpqhandle(L, 1);
  if (SFileCloseArchive(h->handle)) {
    h->status = MSH_CLOSED;
    lua_pushboolean(L, true);
    return 1;
  } else {
    return ms_push_last_err(L);
  }
}

// mpq:hasfile(filename)
int ms_mpq_hasfile(lua_State* L) {
  ms_handle* h = ms_checkmpqhandle(L, 1);
  const char* filename = luaL_checkstring(L, 2);
  if (SFileHasFile(h->handle, filename)) {
    lua_pushboolean(L, true);
  } else if (GetLastError() == ERROR_FILE_NOT_FOUND) {
    lua_pushboolean(L, false);
  } else {
    return ms_push_last_err(L);
  }
  return 1;
}

// mpq:setmaxfilecount(n)
int ms_mpq_setmaxfilecount(lua_State* L) {
  ms_handle* h = ms_checkmpqhandle(L, 1);
  DWORD maxfilecount = luaL_checkint(L, 2);
  if (SFileSetMaxFileCount(h->handle, maxfilecount)) {
    lua_pushboolean(L, true);
    return 1;
  } else {
    return ms_push_last_err(L);
  }
}

// docs say this takes LARGE_INTEGER* but source code says ULONGLONG
static void WINAPI ms_mpq_compact_cb(void* userdata, DWORD worktype,
  ULONGLONG processed, ULONGLONG total) {
  lua_State* L = userdata;

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
int ms_mpq_compact(lua_State* L) {
  ms_handle* h = ms_checkmpqhandle(L, 1);
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
    SFileSetCompactCallback(h->handle, ms_mpq_compact_cb, L);
  }

  if (SFileCompactArchive(h->handle, listfile, false)) {
    lua_pushboolean(L, true);
    return 1;
  } else {
    return ms_push_last_err(L);
  }
}

// mpq:addpatch(filename, prefix)
int ms_mpq_addpatch(lua_State* L) {
  ms_handle* h = ms_checkmpqhandle(L, 1);
  const char* filename = luaL_checkstring(L, 2);
  const char* prefix = luaL_checkstring(L, 3);
  if (SFileOpenPatchArchive(h->handle, filename, prefix, 0)) {
    lua_pushboolean(L, true);
    return 1;
  } else {
    return ms_push_last_err(L);
  }
}

// mpq:ispatched()
int ms_mpq_ispatched(lua_State* L) {
  ms_handle* h = ms_checkmpqhandle(L, 1);
  lua_pushboolean(L, SFileIsPatchedArchive(h->handle));
  return 1;
}

static int ms_mpqhandle_gc(lua_State* L) {
  ms_handle* handle = ms_tompqhandle(L, 1);
  if (ms_isopen(handle)) {
    return ms_mpq_close(L);
  } else {
    return 0;
  }
}

static const struct luaL_Reg mpqhandle_lib[] = {
  {"addlistfile", ms_mpq_addlistfile},
  {"openfile", ms_mpq_openfile},
  {"createfile", ms_mpq_createfile},
  {"flush", ms_mpq_flush},
  {"close", ms_mpq_close},
  {"hasfile", ms_mpq_hasfile},
  {"setmaxfilecount", ms_mpq_setmaxfilecount},
  {"compact", ms_mpq_compact},
  {"addpatch", ms_mpq_addpatch},
  {"ispatched", ms_mpq_ispatched},
  {NULL, NULL}
};

void ms_init_mpqhandle(lua_State* L) {
  // set up mt for mpqhandles
  luaL_newmetatable(L, MPQHANDLE_UDNAME);
  lua_newtable(L);
  luaL_setfuncs(L, mpqhandle_lib, 0);
  lua_setfield(L, -2, "__index");
  lua_pushcfunction(L, ms_mpqhandle_gc);
  lua_setfield(L, -2, "__gc");
  lua_pop(L, 1);
}
