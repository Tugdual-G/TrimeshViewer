##
# MeshViewer
#
# @file
# @version 0.1
CCPP = g++
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS='-Wl,-rpath,$$ORIGIN/src/render/' -Lrender -ltrimesh_render
# Targets
all: meshviewer.cpp mesh_primitives.o mesh_print.o plyfile_parse.o mesh_structure.o mesh_operators.o mesh_primitives.o libtrimesh_render.so

../meshviewer: meshviewer.cpp mesh_print.o plyfile_parse.o mesh_structure.o mesh_operators.o
	$(CCPP) $(CFLAGS) $(LDFLAGS) -o $@ $^

plyfile_parse.o: ply/plyfile_parse.cpp
	$(CCPP) $(CFLAGS) -c $<

mesh_print.o: mesh/mesh_print.cpp
	$(CCPP) $(CFLAGS) -c $<

mesh_primitives.o: mesh/mesh_primitives.cpp
	$(CCPP) $(CFLAGS) -c $<

mesh_operators.o: mesh/mesh_operators.cpp
	$(CCPP) $(CFLAGS) -c $<

mesh_structure.o: mesh/mesh_structure.cpp
	$(CCPP) $(CFLAGS) -c $<

libtrimesh_render.so:
	+$(MAKE) -C render/

clean:
	rm -f main *.o *.so

.PHONY: clean

# end
