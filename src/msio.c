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
