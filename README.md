# moonstorm
## Overview
[Lua **5.3**](http://www.lua.org/versions.html#5.3) bindings for [StormLib](https://github.com/stormlib/StormLib), a library for reading and writing MPQ files. Currently under development and by no means stable. You have been warned!

Lua 5.3 is required for 64-bit integer support and bitwise operators.

## Example
````lua
-- load moonstorm module
local moonstorm = require('moonstorm')

-- open a Warcraft III map and print its script to stdout
local map = moonstorm.open('mymap.w3x')
local f = map:openfile('war3map.j')
print(f:read())

-- cleanup
f:close()
map:close()
````

## Documentation
Coming soon!

## License
[Apache License](http://www.apache.org/licenses/LICENSE-2.0.html)
