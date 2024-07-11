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

demo: demo.cpp ../src/plyfile_parse.o ../src/mesh/libmesh.a ../src/render/libtrimesh_render.so
	$(CCPP) $(CFLAGS) $(LDFLAGS) -o $@ $^

../src/plyfile_parse.o: ../src/ply/*.cpp ../src/ply/*.hpp
	+$(MAKE) -C ../src

../src/mesh/libmesh.a: ../src/mesh/*.cpp ../src/mesh/*.hpp
	+$(MAKE) -C ../src/mesh

../src/render/libtrimesh_render.so: ../src/render/*.cpp ../src/render/*.hpp
	+$(MAKE) -C ../src/render

clean:
	rm -f demo *.o *.so

.PHONY: clean

# end
