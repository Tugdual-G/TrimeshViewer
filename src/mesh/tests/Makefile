##
# test Mesh
#
# @file
# @version 0.1
CC = g++
CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS = -L../ -lmesh
TESTS = edges subdivide face_normals vertex_normals face_areas curvature one_ring vertex_adjacent_faces
# Targets
all: ../libmesh.a $(TESTS)

%: test_%.cpp ../libmesh.a
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	./$@ | diff --color $@.ref -

../libmesh.a: ../mesh*.cpp ../mesh*.hpp
	+$(MAKE) -C ../

clean:
	rm -f *.a *.o *.so $(TESTS)

.PHONY: clean

# end
