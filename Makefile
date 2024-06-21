##
# ply
#
# @file
# @version 0.1
CCPP = g++
CC = gcc
CFLAGS = -Wall -Wextra
# Targets
all: main

main: main.cpp plymesh.cpp trimesh_render.so
	$(CCPP) $(CFLAGS) -o $@ $^

#trimesh_render.so: opengl/trimesh_render.cpp display_window.o compileShader.o
#$(CCPP) $(CFLAGS) -I opengl -lGL -lglfw -shared -o $@ $^
#
trimesh_render.so:  opengl/src/glad.c opengl/trimesh_render.cpp opengl/display_window.c opengl/compileShader.c
	$(CC) $(CFLAGS) -I opengl -shared -o $@ $^ -lGL -lglfw -fPIC

#display_window.o: opengl/display_window.c
#	$(CC) $(CFLAGS) -c $^
#
#compileShader.o: opengl/compileShader.c
#	$(CC) $(CFLAGS) -c $^
#
clean:
	rm -f main *.o *.so

.PHONY: clean

# end
