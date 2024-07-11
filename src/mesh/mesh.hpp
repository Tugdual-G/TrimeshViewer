#ifndef MESH_H_
#define MESH_H_
#include <vector>

class Mesh {
  // maximum number of adjacent faces to a vertice
  unsigned int n_adja_faces_max{0};
  int n_dim{3};
  int vertices_per_face{3};

  // set anti-clockwise order for faces-vertices adjacency vector
  void order_adjacent_faces();

public:
  int n_vertices{0};
  int n_faces{0};
  std::vector<double> vertices;
  std::vector<unsigned int> faces;
  std::vector<double> face_normals;
  std::vector<double> vertex_normals;
  std::vector<unsigned int> vertex_adjacent_faces;
  std::vector<unsigned int> one_ring;

  Mesh() = default;

  Mesh(std::vector<double> &ivertices, std::vector<unsigned int> &ifaces)
      : n_vertices((int)ivertices.size() / 3), n_faces((int)ifaces.size() / 3),
        vertices(ivertices), faces(ifaces) {}

  void init(std::vector<double> &ivertices, std::vector<unsigned int> &ifaces) {
    n_vertices = (int)ivertices.size() / 3;
    n_faces = (int)ifaces.size() / 3;
    vertices = ivertices;
    faces = ifaces;
  }

  void set_one_ring();
  void set_vertex_adjacent_faces(); // ordered faces
  void set_face_normals();
  void set_vertex_normals();

  // Takes one_ring as an argument to make explicit that the
  // method depends on the one-ring.
  auto get_mean_curvature(std::vector<unsigned int> &one_ring)
      -> std::vector<double>;

  auto get_scalar_mean_curvature(std::vector<double> &mean_curvature)
      -> std::vector<double>;

  void print();
  void print_faces();
  void print_vertices();
  void print_vertex_adjacent_face();
  void print_one_ring();
  void print_vertex_normals();
  void print_face_normals();
};

namespace Primitives {
auto cube() -> Mesh;
auto isocahedron() -> Mesh;
auto tetrahedron() -> Mesh;
} // namespace Primitives

#endif // MESH_H_
