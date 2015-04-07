#!/usr/bin/env lua
local ms = require('moonstorm')

-- take filename and dest from command line arguments
local filename = arg[1]
local dest = arg[2]

local function usage()
  print('usage: ' .. arg[0] .. ' <filename> <destination>')
end

-- ensure we have the required arguments
if not filename then
  print('no filename provided')
  return usage()
end

if not dest then
  print('no destination provided')
  return usage()
end

-- open the MPQ file
local mpq, err = ms.open(filename)

if not mpq then
  print('error opening MPQ: ' .. err)
  return
end

-- iterate over each of the files in the archive and extract them to the
-- destination folder
for ent in mpq:files() do
  local nameout = dest .. '/' .. ent.filename
  print('extracting ' .. ent.filename .. ' to ' .. nameout)

  -- open the file in the archive for reading
  local f
  f, err = mpq:openfile(ent.filename)

  -- write it out
  if f then
    local of = io.open(nameout, 'wb')
    of:write(f:read())
    of:close()
    f:close()
  else
    print('error opening file: ' .. err)
  end
end

mpq:close()

