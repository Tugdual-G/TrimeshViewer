##
# MeshViewer
#
# @file
# @version 0.1
CCPP = g++
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS='-Wl,-rpath,$$ORIGIN/src/'
# Targets
all: meshviewer.cpp mesh_print.o plyfile_parse.o mesh_structure.o mesh_operators.o libtrimesh_render.so

../meshviewer: meshviewer.cpp mesh_print.o plyfile_parse.o mesh_structure.o mesh_operators.o libtrimesh_render.so
	$(CCPP) $(CFLAGS) $(LDFLAGS) -o $@ $^

plyfile_parse.o: ply/plyfile_parse.cpp
	$(CCPP) $(CFLAGS) -c $<

mesh_print.o: mesh/mesh_print.cpp
	$(CCPP) $(CFLAGS) -c $<

mesh_operators.o: mesh/mesh_operators.cpp
	$(CCPP) $(CFLAGS) -c $<

mesh_structure.o: mesh/mesh_structure.cpp
	$(CCPP) $(CFLAGS) -c $<

libtrimesh_render.so:  render/glad/glad.c render/trimesh_render.cpp render/compileShader.c render/quatern_transform.cpp render/axis_cross.cpp
	$(CC) $(CFLAGS) -I render -shared -o $@ $^ -lGL -lglfw -fPIC

quatern_transform.o:render/quatern_transform.cpp
	$(CCPP) $(CFLAGS) -c $<

axis_cross.o:render/axis_cross.cpp
	$(CCPP) $(CFLAGS) -c $<

clean:
	rm -f main *.o *.so

.PHONY: clean

# end
