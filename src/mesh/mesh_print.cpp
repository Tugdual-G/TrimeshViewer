#include "mesh.hpp"
#include <iostream>
#include <vector>

void Mesh::print_vertices() {
  for (int i = 0; i < n_vertices; ++i) {
    for (int j = 0; j < 3; ++j) {
      std::cout << vertices.at(i * 3 + j) << " , ";
    }
    std::cout << "\n";
  }
}

void Mesh::print_faces() {
  for (int i = 0; i < n_faces; ++i) {
    std::cout << "face " << i << " : ";
    for (int j = 0; j < 3; ++j) {
      std::cout << faces.at(i * 3 + j) << " , ";
    }
    std::cout << "\n";
  }
}

void Mesh::print_face_normals() {
  for (int i = 0; i < n_faces; ++i) {
    for (int j = 0; j < 3; ++j) {
      std::cout << face_normals.at(i * 3 + j) << " ";
    }
    std::cout << "\n";
  }
}

void Mesh::print_vertex_adjacent_face() {
  int n_adja = 0;
  int adja_array_idx = 0;
  int face = 0;
  for (int i = 0; i < n_vertices; ++i) {
    n_adja = vertex_adjacent_faces.at(adja_array_idx);
    std::cout << "vert  " << i << " : ";
    for (int j = 0; j < n_adja; ++j) {
      ++adja_array_idx;
      face = vertex_adjacent_faces.at(adja_array_idx);
      std::cout << face << " ";
    }
    std::cout << '\n';
    ++adja_array_idx;
  }
}

void Mesh::print_one_ring() {
  int n_adja = 0;
  int one_ring_array_idx = 0;
  int vert = 0;
  for (int i = 0; i < n_vertices; ++i) {
    n_adja = one_ring.at(one_ring_array_idx);
    std::cout << "vert  " << i << " : ";
    for (int j = 0; j < n_adja; ++j) {
      ++one_ring_array_idx;
      vert = one_ring.at(one_ring_array_idx);
      std::cout << vert << " ";
    }
    std::cout << '\n';
    ++one_ring_array_idx;
  }
}

void Mesh::print_vertex_normals() {
  for (int i = 0; i < n_vertices; ++i) {
    for (int j = 0; j < 3; ++j) {
      std::cout << vertex_normals.at(i * 3 + j) << " ";
    }
    std::cout << '\n';
  }
}
