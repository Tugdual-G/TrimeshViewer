##
# MeshViewer
#
# @file
# @version 0.1
CCPP = g++
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS='-Wl,-rpath,$$ORIGIN'
# Targets
all: test quatern_transform.o axis_cross.o libtrimesh_render.so

test: test.cpp libtrimesh_render.so
	$(CCPP) $(CFLAGS) $(LDFLAGS) -o $@ $^

libtrimesh_render.so:  glad/glad.c trimesh_render.cpp compileShader.c quatern_transform.cpp axis_cross.cpp
	$(CC) $(CFLAGS) -I render -shared -o $@ $^ -lGL -lglfw -fPIC

quatern_transform.o:quatern_transform.cpp
	$(CCPP) $(CFLAGS) -c $<

axis_cross.o:axis_cross.cpp
	$(CCPP) $(CFLAGS) -c $<

clean:
	rm -f main *.o *.so

.PHONY: clean

# end
