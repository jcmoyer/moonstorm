#ifndef MSIO_H
#define MSIO_H

#include <StormLib.h>
#include <lua.h>

typedef enum {
  msh_open,
  msh_closed
} ms_handle_stat;

typedef struct {
  HANDLE         handle;
  ms_handle_stat status;
} ms_handle;

// converts a HANDLE to an ms_handle and pushes it onto the stack as a full
// userdata
ms_handle* ms_newhandle(lua_State* L, HANDLE handle);
ms_handle* ms_checkhandle(lua_State* L, int arg);

#endif
