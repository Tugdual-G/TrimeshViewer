##
# Mesh viewer examples
#
# @file
# @version 0.1

CCPP = g++
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS='-Wl,-rpath,$$ORIGIN/../src/'
# Targets
all: demo little_cube

demo: demo.cpp ../src/mesh_print.o ../src/plyfile_parse.o ../src/mesh_structure.o ../src/mesh_operators.o ../src/libtrimesh_render.so
	+$(MAKE) -C ../src
	$(CCPP) $(CFLAGS) $(LDFLAGS) -o $@ $^

little_cube: little_cube.cpp ../src/plyfile_parse.o ../src/mesh_structure.o ../src/mesh_operators.o ../src/libtrimesh_render.so
	+$(MAKE) -C ../src
	$(CCPP) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f main *.o *.so

.PHONY: clean

# end


# end
