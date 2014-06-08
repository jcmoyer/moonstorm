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

#include "common.h"

#ifdef _WIN32
#include <windows.h>
#endif

const char* storm_errstr(DWORD e, char* buffer, DWORD size) {
#ifdef _WIN32
  FormatMessage(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
    NULL,
    e,
    0,
    buffer,
    size,
    NULL
    );
  return buffer;
#else
#define errstr(s) strncpy(buffer, s, size)
  switch (e) {
  // StormPort.h
  case ERROR_SUCCESS:
    return errstr("success");
  case ERROR_FILE_NOT_FOUND:
    return errstr("file not found");
  case ERROR_ACCESS_DENIED:
    return errstr("access denied");
  case ERROR_INVALID_HANDLE:
    return errstr("invalid handle");
  case ERROR_NOT_ENOUGH_MEMORY:
    return errstr("not enough memory");
  case ERROR_NOT_SUPPORTED:
    return errstr("not supported");
  case ERROR_INVALID_PARAMETER:
    return errstr("invalid parameter");
  case ERROR_DISK_FULL:
    return errstr("disk full");
  case ERROR_ALREADY_EXISTS:
    return errstr("already exists");
  case ERROR_INSUFFICIENT_BUFFER:
    return errstr("insufficient buffer");
  // StormLib.h - these probably don't belong here
  case ERROR_AVI_FILE:
    return errstr("not an MPQ file, but an AVI file");
  case ERROR_UNKNOWN_FILE_KEY:
    return errstr("read: can't find file key");
  case ERROR_CHECKSUM_ERROR:
    return errstr("read: sector CRC doesn't match");
  case ERROR_INTERNAL_FILE:
    return errstr("operation not allowed on internal file");
  case ERROR_BASE_FILE_MISSING:
    return errstr("file present as incremental patch file, but base file is missing");
  case ERROR_MARKED_FOR_DELETE:
    return errstr("file marked for deletion");
  case ERROR_FILE_INCOMPLETE:
    return errstr("missing required file part");
  case ERROR_UNKNOWN_FILE_NAMES:
    return errstr("at least one file name is unknown");
  default:
    return errstr("unknown error");
  }
#undef errstr
#endif
}

void ms_push_errstr(lua_State* L, DWORD e) {
  char buffer[1024];
  storm_errstr(e, buffer, sizeof(buffer));
  lua_pushstring(L, buffer);
}

int ms_push_last_err(lua_State* L) {
  DWORD e = GetLastError();
  lua_pushnil(L);
  ms_push_errstr(L, e);
  lua_pushinteger(L, e);
  return 3;
}
