#ifndef MESH_H_
#define MESH_H_
#include <fstream>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

class Mesh {
  // int n_vertice_elements;
  int n_adja_faces_max{0};
  // std::vector<int> vertices_elements_sizes;
  //  int element_faces_bin_size;
  void order_adjacent_faces();

public:
  int n_dim{3};
  int vertices_per_face{3};
  int n_edges{0};
  std::vector<double> vertices;
  std::vector<unsigned int> faces;
  int n_vertices{0};
  int n_faces{0};
  // std::vector<double> normals;
  std::vector<double> face_normals;
  std::vector<double> vertex_normals;
  std::vector<unsigned int> edges;
  std::vector<unsigned int> one_ring;
  std::vector<unsigned int> vertex_adjacent_faces;
  std::vector<double> mean_curvature;
  Mesh(){};

  Mesh(std::vector<double> &ivertices, std::vector<unsigned int> &ifaces)
      : vertices(ivertices), faces(ifaces), n_vertices(ivertices.size() / 3),
        n_faces(ifaces.size() / 3) {
    set_face_normals();
    set_vertex_adjacent_faces();
    set_vertex_normals();
  }

  void init(std::vector<double> &ivertices, std::vector<unsigned int> &ifaces) {
    vertices = ivertices;
    faces = ifaces;
    n_vertices = ivertices.size() / 3;
    n_faces = ifaces.size() / 3;
    set_face_normals();
    set_vertex_adjacent_faces();
    set_vertex_normals();
    // set_one_ring(); // Migth trow errors with open meshes
  }

  void set_one_ring();
  void set_vertex_adjacent_faces();
  void set_face_normals();
  void set_vertex_normals();
  void set_mean_curvature();
  void scalar_mean_curvature(std::vector<double> &k);
  // void print();
  // void print_faces();
  // void print_vertices();
  // void print_vertex_adjacent_face();
  // void print_one_ring();
  // void print_vertex_normals();
  // void print_face_normals();
};

// void Mesh::print_vertices() {
//   for (int i = 0; i < n_vertices; ++i) {
//     for (int j = 0; j < 3; ++j) {
//       std::cout << vertices.at(i * 3 + j) << " ";
//     }
//     std::cout << "\n";
//   }
// }

// void Mesh::print_faces() {
//   for (int i = 0; i < n_faces; ++i) {
//     std::cout << "face " << i << " : ";
//     for (int j = 0; j < 3; ++j) {
//       std::cout << faces.at(i * 3 + j) << " ";
//     }
//     std::cout << "\n";
//   }
// }

// void Mesh::print_face_normals() {
//   for (int i = 0; i < n_faces; ++i) {
//     for (int j = 0; j < 3; ++j) {
//       std::cout << face_normals.at(i * 3 + j) << " ";
//     }
//     std::cout << "\n";
//   }
// }

// void Mesh::print_vertex_adjacent_face() {
//   int n_adja = 0;
//   int adja_array_idx = 0;
//   int face;
//   for (int i = 0; i < n_vertices; ++i) {
//     n_adja = vertex_adjacent_faces.at(adja_array_idx);
//     std::cout << "vert  " << i << " : ";
//     for (int j = 0; j < n_adja; ++j) {
//       ++adja_array_idx;
//       face = vertex_adjacent_faces.at(adja_array_idx);
//       std::cout << face << " ";
//     }
//     std::cout << std::endl;
//     ++adja_array_idx;
//   }
// }

// void Mesh::print_one_ring() {
//   int n_adja = 0;
//   int one_ring_array_idx = 0;
//   int vert;
//   for (int i = 0; i < n_vertices; ++i) {
//     n_adja = one_ring.at(one_ring_array_idx);
//     std::cout << "vert  " << i << " : ";
//     for (int j = 0; j < n_adja; ++j) {
//       ++one_ring_array_idx;
//       vert = one_ring.at(one_ring_array_idx);
//       std::cout << vert << " ";
//     }
//     std::cout << std::endl;
//     ++one_ring_array_idx;
//   }
// }

// void Mesh::print_vertex_normals() {
//   for (int i = 0; i < n_vertices; ++i) {
//     for (int j = 0; j < 3; ++j) {
//       std::cout << vertex_normals.at(i * 3 + j) << " ";
//     }
//     std::cout << std::endl;
//   }
// }
#endif // MESH_H_
