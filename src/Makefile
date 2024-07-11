##
# MeshViewer
#
# @file
# @version 0.1
CCPP = g++
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS='-Wl,-rpath,$$ORIGIN/src/render/' -Lmesh -Lrender -ltrimesh_render -lmesh
# Targets
all: ../meshviewer plyfile_parse.o mesh/libmesh.a libtrimesh_render.so

../meshviewer: meshviewer.cpp plyfile_parse.o mesh/libmesh.a
	$(CCPP) $(CFLAGS) $(LDFLAGS) -o $@ $^

plyfile_parse.o: ply/*.cpp ply/*.hpp
	$(CCPP) $(CFLAGS) -c $<

mesh/libmesh.a: mesh/*.cpp mesh/*.hpp
	+$(MAKE) -C mesh/

libtrimesh_render.so: render/*.cpp render/*.hpp
	+$(MAKE) -C render/

clean:
	rm -f main *.o *.so

.PHONY: clean

# end
