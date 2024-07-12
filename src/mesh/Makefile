##
# Mesh
#
# @file
# @version 0.1
CC = g++
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -L. -lmesh

# Targets
all: libmesh.a

libmesh.a: mesh_print.o mesh_primitives.o mesh_operators.o mesh_structure.o
	ar rvs $@ $^

mesh_print.o: mesh_print.cpp
	$(CC) $(CFLAGS) -c $<

mesh_primitives.o: mesh_primitives.cpp
	$(CC) $(CFLAGS) -c $<

mesh_operators.o: mesh_operators.cpp
	$(CC) $(CFLAGS) -c $<

mesh_structure.o: mesh_structure.cpp
	$(CC) $(CFLAGS) -c $<

test:
	$(MAKE) -C tests/ clean
	$(MAKE) -C tests/

clean:
	rm -f *.a *.o *.so

.PHONY: clean

# end
