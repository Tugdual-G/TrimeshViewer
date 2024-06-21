#ifndef PLYMESH_H_
#define PLYMESH_H_
#include <fstream>
#include <string>
#include <variant>
#include <vector>

enum Types {
  NONE,
  FLOAT,
  DOUBLE,
};

class PlyMesh {
  int file_data_offset = -1;
  Types vertex_type = NONE;
  Types normal_type = NONE;
  int n_vertice_elements;
  std::vector<int> vertices_elements_sizes;
  // int element_faces_bin_size;
  int parse_header(std::ifstream *file);
  int load_data(std::ifstream *file);
  int from_file(const char *fname);

public:
  int n_dim = 3;
  int vertices_per_face = 3;
  int n_edges = 0;
  std::vector<double> vertices;
  std::vector<unsigned int> faces;
  int n_vertices = 0;
  int n_faces = 0;
  std::vector<double> normals;
  std::vector<double> faces_normals;
  std::vector<unsigned int> edges;
  std::vector<unsigned int> one_ring;

  PlyMesh(std::vector<double> ivertices, std::vector<unsigned int> ifaces)
      : vertices(ivertices), faces(ifaces), n_vertices(ivertices.size() / 3),
        n_faces(ifaces.size() / 3) {
    set_face_normals();
  }

  PlyMesh(const char *fname) {
    from_file(fname);
    set_face_normals();
  }

  void set_face_normals();
  void print();
  void print_faces();
  void print_vertices();
  void print_normals();
};
#endif // PLYMESH_H_
