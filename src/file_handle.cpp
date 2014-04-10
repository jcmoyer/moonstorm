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

#include "file_handle.h"
#include "common.h"

// name of the file handle (obtained through an MPQ file) metatable
const char* FILEHANDLE_UDNAME = "mpqfilehandle";

void moonstorm_newfilehandle(lua_State* L, HANDLE h) {
  HANDLE* udhandle = static_cast<HANDLE*>(lua_newuserdata(L, sizeof(HANDLE)));
  *udhandle = h;
  luaL_setmetatable(L, FILEHANDLE_UDNAME);
}

// ensures that the value on top of the stack is a file handle userdata
HANDLE* moonstorm_checkfilehandle(lua_State* L, int arg) {
  return static_cast<HANDLE*>(luaL_checkudata(L, arg, FILEHANDLE_UDNAME));
}

// helper function that calls moonstorm_mpq_file_size on a handle that isn't
// located on top of the stack
int moonstorm_mpq_file_size_at(lua_State* L, int handle) {
  int size = 0;
  lua_pushvalue(L, handle);
  int nresults = moonstorm_mpq_file_size(L);
  if (nresults != 1) {
    const char* errstr = lua_tostring(L, -2);
    luaL_error(L, "moonstorm_mpq_file_size_at: %s", errstr);
  }
  size = lua_tointeger(L, -1);
  lua_pop(L, 2);
}

int moonstorm_mpq_file_size(lua_State* L) {
  HANDLE* h = moonstorm_checkfilehandle(L, 1);
  DWORD high, low;

  low = SFileGetFileSize(*h, &high);
  if (low != SFILE_INVALID_SIZE) {
    lua_pushinteger(L, make_long_lua_int(high, low));
    return 1;
  } else {
    return moonstorm_push_last_err(L);
  }
}

// file:seek([whence [, offset]])
int moonstorm_mpq_file_seek(lua_State* L) {
  HANDLE* h = moonstorm_checkfilehandle(L, 1);
  const char* whence = luaL_optstring(L, 2, "cur");
  lua_Integer offset = luaL_optinteger(L, 3, 0);

  LONG high, low;
  extract_long_lua_int(offset, high, low);

  // convert to stormlib move method
  DWORD method;
  if (strcmp(whence, "set") == 0) {
    method = FILE_BEGIN;
  } else if (strcmp(whence, "cur") == 0) {
    method = FILE_CURRENT;
  } else if (strcmp(whence, "end") == 0) {
    method = FILE_END;
  }

  low = SFileSetFilePointer(*h, low, &high, method);
  if (low != SFILE_INVALID_SIZE) {
    lua_pushinteger(L, make_long_lua_int(high, low));
    return 1;
  } else {
    return moonstorm_push_last_err(L);
  }
}

int moonstorm_mpq_file_name(lua_State* L) {
  HANDLE* h = moonstorm_checkfilehandle(L, 1);
  char filename[MAX_PATH];

  if (SFileGetFileName(*h, filename)) {
    lua_pushstring(L, filename);
    return 1;
  } else {
    return moonstorm_push_last_err(L);
  }
}

int moonstorm_mpq_file_read(lua_State* L) {
  HANDLE* h = moonstorm_checkfilehandle(L, 1);
  int bufsize;

  if (lua_isinteger(L, 2)) {
    bufsize = lua_tointeger(L, 2);
  } else if (lua_isstring(L, 2)) {
    const char* what = lua_tostring(L, 2);
    if (strcmp(what, "*all") == 0) {
      bufsize = moonstorm_mpq_file_size_at(L, 1);
    } else {
      luaL_error(L, "bad argument #1 to 'read' (invalid option)");
    }
  } else if (lua_isnoneornil(L, 2)) {
    // default to "*all" behavior
    bufsize = moonstorm_mpq_file_size_at(L, 1);
  }

  luaL_Buffer buf;
  char* dst = luaL_buffinitsize(L, &buf, bufsize);
  DWORD nread;
  
  if (SFileReadFile(*h, static_cast<void*>(dst), bufsize, &nread, NULL)) {
    luaL_pushresultsize(&buf, nread);
    return 1;
  } else {
    return moonstorm_push_last_err(L);
  }
}

int moonstorm_mpq_file_write(lua_State* L) {
  HANDLE* h = moonstorm_checkfilehandle(L, 1);
  const char* s = luaL_checkstring(L, 2);
  DWORD comp = luaL_optinteger(L, 3, 0);

  lua_Integer sz = luaL_len(L, 2);
  
  if (SFileWriteFile(*h, static_cast<const void*>(s), sz, comp)) {
    lua_pushvalue(L, 1);
    return 1;
  } else {
    return moonstorm_push_last_err(L);
  }
}

int moonstorm_mpq_file_close(lua_State* L) {
  HANDLE* h = moonstorm_checkfilehandle(L, 1);
  if (!SFileCloseFile(*h)) {
    return moonstorm_push_last_err(L);
  }
  return 0;
}

static const struct luaL_Reg mpqfilehandle_lib[] = {
  {"size", moonstorm_mpq_file_size},
  {"seek", moonstorm_mpq_file_seek},
  {"name", moonstorm_mpq_file_name},
  {"read", moonstorm_mpq_file_read},
  {"write", moonstorm_mpq_file_write},
  {"close", moonstorm_mpq_file_close},
  {NULL, NULL}
};

void moonstorm_init_filehandle(lua_State* L) {
  luaL_newmetatable(L, FILEHANDLE_UDNAME);
  lua_newtable(L);
  luaL_setfuncs(L, mpqfilehandle_lib, 0);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);
}
