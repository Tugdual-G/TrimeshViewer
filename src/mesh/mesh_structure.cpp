#include "mesh.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

static void normalize(double *w);
static void vector_prod(const double *u, const double *v, double *w);

void Mesh::set_one_ring() {
  /* Sets the one-ring (adjacent vertices / link) for each vertice in the
   * "vertices" vector.
   * The one-ring is stored as a contiguous list of sublist.
   * Each sublist starts with the number of elements in the ring,
   * followed by the vertices indices.
   * */
  if (n_adja_faces_max == 0) {
    set_vertex_adjacent_faces();
  }
  int adja_array_idx{0};    // global position in the array
  int onering_array_idx{0}; // global position in the one-ring array
  int triangle_vert_idx{0}; // index of the vertices in the face [0, 1, 2]
  int face{0};              // face index in faces
  int n_adja{0};            // number of adjacent faces for the current vertice

  one_ring.resize(n_vertices * (1 + n_adja_faces_max));
  for (unsigned int i = 0; i < (unsigned int)n_vertices; ++i) {
    n_adja = vertex_adjacent_faces.at(adja_array_idx);
    for (int j = 0; j < n_adja; ++j) {
      triangle_vert_idx = 0;
      face = vertex_adjacent_faces.at(adja_array_idx + j + 1);

      // finding the vertex i in the face vertices
      while (faces.at(face * 3 + triangle_vert_idx) != i) {
        ++triangle_vert_idx;
      }
      one_ring.at(onering_array_idx + 1 + j) =
          faces.at(face * 3 + (triangle_vert_idx + 1) % 3);
    }

    if (one_ring.at(onering_array_idx + 1) !=
        faces.at(face * 3 + (triangle_vert_idx + 2) % 3)) {
      one_ring.at(onering_array_idx) = 0;
      onering_array_idx += 1;
      std::cout << "\n Warning, the shape is open at vertice  " << i << ".\n";
    } else {
      one_ring.at(onering_array_idx) = n_adja;
      onering_array_idx += n_adja + 1; // next vertice
    }
    adja_array_idx += n_adja + 1; // next vertice
  }
  one_ring.resize(onering_array_idx);
}

void Mesh::order_adjacent_faces() {
  /* Orders the adjacent faces in counter clockwise order arround the central
   * vertice. */
  std::vector<int> unordered_faces(n_adja_faces_max, -1);

  // edges oposite to the current vertice [[vert0, vert1],...,[vert0, vert1]]
  std::vector<int> oposite_edge(n_adja_faces_max * 2, -1);

  int adja_array_idx = 0;    // global position in the array
  int triangle_vert_idx = 0; // index of the vertices in the face [0, 1, 2]
  int face = 0;              // face index in faces
  int n_adja = 0;            // number of adjacent faces for the current vertice
  int vert_0_fidx;
  int vert_1_fidx; // oposite edges vertices index in faces array
  int j_next;
  int j_current; // adjacent faces idx in unordered_faces

  for (unsigned int i = 0; i < (unsigned int)n_vertices; ++i) {
    n_adja = vertex_adjacent_faces.at(adja_array_idx);
    // std::cout << "n adja" << n_adja << " \n";
    for (int j = 0; j < n_adja; ++j) {
      //++adja_array_idx;
      triangle_vert_idx = 0;
      face = vertex_adjacent_faces.at(adja_array_idx + j + 1);
      unordered_faces.at(j) = face;

      // std::cout << "face " << face << " \n";
      while (faces.at(face * 3 + triangle_vert_idx) != i) {
        ++triangle_vert_idx;
      }
      /*  trigonometric orientation

              i
              /\
             /  \
            /    \
       i+1 /______\ i+2

      */
      vert_0_fidx = face * 3 + (triangle_vert_idx + 1) % 3;
      vert_1_fidx = face * 3 + (triangle_vert_idx + 2) % 3;

      // std::cout << i << " triangle vert idx " << triangle_vert_idx <<
      // std::endl; std::cout << i << " triangle edge vert idx "
      //           << (triangle_vert_idx + 1) % 3 << " , "
      //           << (triangle_vert_idx + 2) % 3 << "\n";
      oposite_edge.at(j * 2) = faces.at(vert_0_fidx);
      oposite_edge.at(j * 2 + 1) = faces.at(vert_1_fidx);
      // std::cout << i << " oposite " << faces.at(vert_0_fidx) << " , "
      //           << faces.at(vert_1_fidx) << "\n";
    }
    triangle_vert_idx = 0;
    j_current = 0;
    for (int j = 0; j < n_adja - 1; ++j) {
      j_next = 0;
      while (oposite_edge.at(j_next * 2) !=
             oposite_edge.at(j_current * 2 + 1)) {
        ++j_next;
        // j_next %= n_adja;
        // std::cout << "ok4:" << j_current << " , " << j_next << " \n";
      }

      // std::cout << "ok5:" << j_current << " , " << j_next << " \n";
      j_current = j_next;
      vertex_adjacent_faces.at(adja_array_idx + j + 2) =
          unordered_faces.at(j_next);
    }

    // std::cout << "ok6 \n";
    adja_array_idx += n_adja + 1; // next vertice
  }
}

void Mesh::set_vertex_adjacent_faces() {
  /* Finds the adjacent faces of each vertices. */
  vertex_adjacent_faces.reserve(vertices.size() * 10);
  int n_adja = 1;
  int n_adja_max = 0;
  long unsigned int total_size = 0;
  for (unsigned int i = 0; i < (unsigned int)n_vertices; ++i) {
    n_adja = 0;
    vertex_adjacent_faces.push_back(-1);
    ++total_size; // since we store the number of adj at the begining.
                  // TODO maybe vector can handle this with reserve
    for (unsigned int j = 0; j < (unsigned int)n_faces; ++j) {
      for (unsigned int k = 0; k < 3; ++k) {
        if (faces[j * 3 + k] == i) {
          vertex_adjacent_faces.push_back(j);
          ++n_adja;
          ++total_size;
        }
      }
    }
    vertex_adjacent_faces[total_size - n_adja - 1] = n_adja;
    n_adja_max = (n_adja > n_adja_max) ? n_adja : n_adja_max;
    if (n_adja == 0) {
      std::cout << "Error, vertex without an adjacent face.\n";
    }
  }
  vertex_adjacent_faces.resize(total_size);
  vertex_adjacent_faces.shrink_to_fit();
  n_adja_faces_max = n_adja_max;
  if (n_adja_max > 500) {
    std::cout << "\n Warning, one vertice has more than 500 adjacent faces \n";
  }
  order_adjacent_faces();
}

void Mesh::set_vertex_normals() {
  if (vertex_adjacent_faces.empty()) {
    set_vertex_adjacent_faces();
  }
  if (face_normals.empty()) {
    set_face_normals();
  }

  vertex_normals.resize(n_vertices * 3, 0);
  int n_adja = 0;
  int adja_array_idx = 0;
  int face = 0;
  for (int i = 0; i < n_vertices; ++i) {
    n_adja = vertex_adjacent_faces.at(adja_array_idx);
    for (int j = 0; j < n_adja; ++j) {
      ++adja_array_idx;
      face = vertex_adjacent_faces.at(adja_array_idx);
      for (int k = 0; k < 3; ++k) {
        vertex_normals.at(i * 3 + k) += face_normals.at(face * 3 + k);
      }
    }
    vertex_normals.at(i * 3) /= n_adja;
    vertex_normals.at(i * 3 + 1) /= n_adja;
    vertex_normals.at(i * 3 + 2) /= n_adja;

    ++adja_array_idx;
  }
}

void Mesh::set_face_normals() {
  /* Computes the normals for each triangular face .*/
  face_normals.resize(faces.size(), 0);
  unsigned int i{0};
  unsigned int j{0};
  unsigned int k{0};
  double e0[3];
  double e1[3];
  for (int face_idx = 0; face_idx < n_faces; ++face_idx) {
    i = faces.at(3 * face_idx);
    j = faces.at(3 * face_idx + 1);
    k = faces.at(3 * face_idx + 2);

    e0[0] = vertices[j * 3] - vertices[i * 3];
    e0[1] = vertices[j * 3 + 1] - vertices[i * 3 + 1];
    e0[2] = vertices[j * 3 + 2] - vertices[i * 3 + 2];

    e1[0] = vertices[k * 3] - vertices[i * 3];
    e1[1] = vertices[k * 3 + 1] - vertices[i * 3 + 1];
    e1[2] = vertices[k * 3 + 2] - vertices[i * 3 + 2];
    vector_prod(e0, e1, face_normals.data() + face_idx * 3);
    normalize(face_normals.data() + face_idx * 3);
  }
}

void normalize(double *w) {
  double norm = pow(pow(w[0], 2.0) + pow(w[1], 2.0) + pow(w[2], 2.0), 0.5);
  w[0] /= norm;
  w[1] /= norm;
  w[2] /= norm;
}

void vector_prod(const double *u, const double *v, double *w) {
  w[0] = u[1] * v[2] - u[2] * v[1];
  w[1] = u[2] * v[0] - u[0] * v[2];
  w[2] = u[0] * v[1] - u[1] * v[0];
}
