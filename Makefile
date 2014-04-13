#   Copyright 2014 J.C. Moyer
# 
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
# 
#       http://www.apache.org/licenses/LICENSE-2.0
# 
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

CC     = gcc
LFLAGS = -llua53 -lStormLib -shared
override CFLAGS += -Wall -Wextra -std=c99

ifeq ($(OS),Windows_NT)
	LIBRARY = moonstorm.dll
else
	LIBRARY = libmoonstorm.so
endif

OBJ = src/common.o src/msio.o src/file_handle.o src/mpq_handle.o src/moonstorm.o

all: $(LIBRARY)

$(OBJ): %.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

$(LIBRARY): $(OBJ)
	$(CC) $(CFLAGS) -o $(LIBRARY) $(OBJ) $(LFLAGS)

clean:
	rm -f $(LIBRARY)
	rm -f $(OBJ)

.PHONY: clean
