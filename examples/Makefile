##
# Mesh viewer examples
#
# @file
# @version 0.1

CCPP = g++
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS='-Wl,-rpath,$$ORIGIN/../src/render/' -L../src/render -ltrimesh_render
# Targets
all: demo

demo: demo.cpp ../src/mesh_primitives.o ../src/mesh_print.o ../src/plyfile_parse.o ../src/mesh_structure.o ../src/mesh_operators.o
	+$(MAKE) -C ../src
	$(CCPP) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f demo little_cube *.o *.so

.PHONY: clean

# end


# end
