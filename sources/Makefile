##
# ply
#
# @file
# @version 0.1
CCPP = g++
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS='-Wl,-rpath,$$ORIGIN/sources/'
# Targets
all: ../meshviewer

../meshviewer: meshviewer.cpp mesh_print.o plyfile_parse.o mesh_structure.o mesh_operators.o libtrimesh_render.so
	$(CCPP) $(CFLAGS) $(LDFLAGS) -o $@ $^

../axiscross: ../axiscross.cpp mesh_print.o plyfile_parse.o mesh_structure.o mesh_operators.o libtrimesh_render.so
	$(CCPP) $(CFLAGS) $(LDFLAGS) -o $@ $^

../little_cube: ../little_cube.cpp plymesh_parse.o mesh_structure.o mesh_operators.o libtrimesh_render.so
	$(CCPP) $(CFLAGS) $(LDFLAGS) -o $@ $^

plyfile_parse.o: plyfile_parse.cpp
	$(CCPP) $(CFLAGS) -c $<

# libmesh.so: mesh_structure.o mesh_operators.o mesh_print.o
# 	$(CC) $(CFLAGS) -shared -o $@ $^ -fPIC

mesh_print.o: mesh_print.cpp
	$(CCPP) $(CFLAGS) -c $<

mesh_operators.o: mesh_operators.cpp
	$(CCPP) $(CFLAGS) -c $<

mesh_structure.o: mesh_structure.cpp
	$(CCPP) $(CFLAGS) -c $<

libtrimesh_render.so:  opengl/glad/glad.c opengl/trimesh_render.cpp opengl/display_window.c opengl/compileShader.c opengl/quatern_transform.cpp
	$(CC) $(CFLAGS) -I opengl -shared -o $@ $^ -lGL -lglfw -fPIC

test_quatern: opengl/test_quatern.cpp
	$(CCPP) $(CFLAGS) $^ -o $@

clean:
	rm -f main *.o *.so

.PHONY: clean

# end
