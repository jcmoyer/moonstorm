CPP     = g++
CFLAGS  = -Wall
LFLAGS  = -llua53 -lStormLib -shared

ifeq ($(OS),Windows_NT)
	LIBRARY = moonstorm.dll
else
	LIBRARY = libmoonstorm.so
endif

OBJ = src/common.o src/file_handle.o src/mpq_handle.o src/moonstorm.o

all: $(LIBRARY)

$(OBJ): %.o: %.cpp
	$(CPP) -o $@ $(CFLAGS) -c $<

$(LIBRARY): $(OBJ)
	$(CPP) $(CFLAGS) -o $(LIBRARY) $(OBJ) $(LFLAGS)

clean:
	rm -f $(LIBRARY)
	rm -f $(OBJ)

.PHONY: clean
