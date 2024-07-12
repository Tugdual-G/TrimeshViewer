#include "mesh.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

constexpr unsigned int maxuint{~(0U)};

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
  for (int i = 0; i < n_vertices; ++i) {
    n_adja = (int)vertex_adjacent_faces.at(adja_array_idx);
    for (int j = 0; j < n_adja; ++j) {
      triangle_vert_idx = 0;
      face = (int)vertex_adjacent_faces.at(adja_array_idx + j + 1);

      // finding the vertex i in the face vertices
      while (faces.at(face * 3 + triangle_vert_idx) != (unsigned int)i) {
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

  int adja_array_idx{0};    // global position in the array
  int triangle_vert_idx{0}; // index of the vertices in the face [0, 1, 2]
  int face{-1};             // face index in faces
  int n_adja{-1};           // number of adjacent faces for the current vertice
  int vert_0_face_idx{-1};
  int vert_1_face_idx{-1}; // oposite edges vertices index in faces array
  int j_next{-1};
  int j_current{-1}; // adjacent faces idx in unordered_faces

  for (int i = 0; i < n_vertices; ++i) {
    n_adja = (int)vertex_adjacent_faces.at(adja_array_idx);
    // std::cout << "n adja" << n_adja << " \n";
    for (int j = 0; j < n_adja; ++j) {
      //++adja_array_idx;
      triangle_vert_idx = 0;
      face = (int)vertex_adjacent_faces.at(adja_array_idx + j + 1);
      unordered_faces.at(j) = face;

      // std::cout << "face " << face << " \n";
      while (faces.at(face * 3 + triangle_vert_idx) != (unsigned int)i) {
        ++triangle_vert_idx;
      }
      /*  trigonometric orientation

              i
              /\
             /  \
            /    \
       i+1 /______\ i+2

      */
      vert_0_face_idx = face * 3 + (triangle_vert_idx + 1) % 3;
      vert_1_face_idx = face * 3 + (triangle_vert_idx + 2) % 3;

      // std::cout << i << " triangle vert idx " << triangle_vert_idx <<
      // std::endl; std::cout << i << " triangle edge vert idx "
      //           << (triangle_vert_idx + 1) % 3 << " , "
      //           << (triangle_vert_idx + 2) % 3 << "\n";
      oposite_edge.at(j * 2) = (int)faces.at(vert_0_face_idx);
      oposite_edge.at(j * 2 + 1) = (int)faces.at(vert_1_face_idx);
      // std::cout << i << " oposite " << faces.at(vert_0_face_idx) << " , "
      //           << faces.at(vert_1_face_idx) << "\n";
    }
    triangle_vert_idx = 0;
    j_current = 0;
    for (int j = 0; j < n_adja - 1; ++j) {
      j_next = 0;
      while (oposite_edge.at(j_next * 2) !=
             oposite_edge.at(j_current * 2 + 1)) {
        ++j_next;
      }

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
  long int total_size = 0;
  for (int i = 0; i < n_vertices; ++i) {
    n_adja = 0;
    vertex_adjacent_faces.push_back(-1);
    ++total_size; // since we store the number of adj at the begining.
                  // TODO maybe vector can handle this with reserve
    for (int j = 0; j < n_faces; ++j) {
      for (int k = 0; k < 3; ++k) {
        if (faces[j * 3 + k] == (unsigned int)i) {
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
    n_adja = (int)vertex_adjacent_faces.at(adja_array_idx);
    for (int j = 0; j < n_adja; ++j) {
      ++adja_array_idx;
      face = (int)vertex_adjacent_faces.at(adja_array_idx);
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

void Mesh::set_edges() {
  /* Finds the lis of uniques edges of the mesh.
   * The hashing function is :
   *  min vertex idx | max vertex idx
   * [     32 bits   |     32 bits   ]
   * [      unsigned long long       ]
   * */
  std::set<unsigned long long int> edges_set;
  unsigned int v[] = {0, 0, 0};
  unsigned int v_min{0};
  unsigned int v_max{0};
  unsigned long long e{0};
  unsigned long long mask{0};
  mask = ~mask;
  mask <<= 32;
  mask = ~mask;

  for (int i = 0; i < n_faces; ++i) {
    v[0] = faces.at(i * 3);
    v[1] = faces.at(i * 3 + 1);
    v[2] = faces.at(i * 3 + 2);

    v_min = v[0] > v[1] ? v[1] : v[0];
    v_max = v[0] > v[1] ? v[0] : v[1];
    e = v_min;
    e <<= 32;
    e += v_max;
    edges_set.insert(e);

    v_min = v[0] > v[2] ? v[2] : v[0];
    v_max = v[0] > v[2] ? v[0] : v[2];
    e = v_min;
    e <<= 32;
    e += v_max;
    edges_set.insert(e);

    v_min = v[2] > v[1] ? v[1] : v[2];
    v_max = v[2] > v[1] ? v[2] : v[1];
    e = v_min;
    e <<= 32;
    e += v_max;
    edges_set.insert(e);
  }
  edges.resize(edges_set.size() * 2);
  unsigned int i{0};
  for (const auto &e_set : edges_set) {
    edges.at(i) = e_set >> 32;
    edges.at(i + 1) = e_set & mask;
    i += 2;
  }
}

struct SplitVertice {
  // Represent old vertices
  // and vertices obtained by splitting.
  // old vertices have attribute i equals their current
  // index and element j equal max unsigned int;
  // Vertices obtained by splitting are represented as edges.
  unsigned int i{0};
  unsigned int j{0};
  auto operator==(const SplitVertice &other) const -> bool {
    return (i == other.i && j == other.j);
  }
};

struct Hash {
  auto operator()(const SplitVertice sv) const -> size_t {
    size_t h = sv.j;
    h <<= 32;
    h += sv.i;
    return h;
  }
};

auto splitvertice_map_insert(
    std::unordered_map<SplitVertice, unsigned int, Hash> &sv_map,
    SplitVertice &new_key) -> unsigned int {
  /* Insert a new vertice key in the split vertices map.
   * If this vertices is already in the map the key is not inserted
   * and its value unchanged.
   * Return the corresponding value in the map, this value is an index.
   * */
  auto key_iter = sv_map.find(new_key);
  if (key_iter == sv_map.end()) {
    key_iter = sv_map.insert(std::make_pair(new_key, sv_map.size())).first;
  }
  return key_iter->second;
}

void split_triangle(
    const unsigned int face[3],
    std::unordered_map<SplitVertice, unsigned int, Hash> &sv_map,
    unsigned int new_face_vertices[6]) {
  /* Split a triangle face and returns an ordered set of vertices indices along
   * its edges.
   * The split vertices map is updated with uniques vertices.
   * */

  SplitVertice sv{.i = 0, .j = 0};
  sv.i = face[0];
  sv.j = maxuint;
  new_face_vertices[0] = splitvertice_map_insert(sv_map, sv);

  // Vertices indices are sorted to make the SplitVertice ID unique
  sv.i = face[0] > face[1] ? face[1] : face[0];
  sv.j = face[0] > face[1] ? face[0] : face[1];
  new_face_vertices[1] = splitvertice_map_insert(sv_map, sv);

  sv.i = face[1];
  sv.j = maxuint;
  new_face_vertices[2] = splitvertice_map_insert(sv_map, sv);

  sv.i = face[1] > face[2] ? face[2] : face[1];
  sv.j = face[1] > face[2] ? face[1] : face[2];
  new_face_vertices[3] = splitvertice_map_insert(sv_map, sv);

  sv.i = face[2];
  sv.j = maxuint;
  new_face_vertices[4] = splitvertice_map_insert(sv_map, sv);

  sv.i = face[0] > face[2] ? face[2] : face[0];
  sv.j = face[0] > face[2] ? face[0] : face[2];
  new_face_vertices[5] = splitvertice_map_insert(sv_map, sv);
  // faces.push_back(i);
}

void splitvertice_to_vertice(const std::vector<double> &vertices,
                             const SplitVertice &sv,
                             std::vector<double>::iterator out_vertice) {
  /* Convert a SplitVertice vertice identifier to coordinates. */
  if (sv.j == maxuint) {
    out_vertice[0] = vertices.at(sv.i * 3);
    out_vertice[1] = vertices.at(sv.i * 3 + 1);
    out_vertice[2] = vertices.at(sv.i * 3 + 2);
  } else {
    out_vertice[0] = 0.5 * (vertices.at(sv.j * 3) + vertices.at(sv.i * 3));
    out_vertice[1] =
        0.5 * (vertices.at(sv.j * 3 + 1) + vertices.at(sv.i * 3 + 1));
    out_vertice[2] =
        0.5 * (vertices.at(sv.j * 3 + 2) + vertices.at(sv.i * 3 + 2));
  }
}

void Mesh::subdivide() {
  /* Split each edge in half.
   * In result, each triangle is split into 4 triangles.
   * This method try to preseve data locality as much as possible.
   * */

  if (edges.size() < 3) {
    set_edges();
  }

  std::vector<unsigned int> faces_tmp(faces);
  faces.resize(faces.size() * 4);
  std::vector<double> vertices_tmp(vertices);
  vertices.resize(vertices.size() + 3 * edges.size() / 2);

  // Defines vertices by their old index or by the edge they split
  // Uses a hash map to register uniques edges
  std::unordered_map<SplitVertice, unsigned int, Hash> uniques_vertices;
  unsigned int face[] = {0, 0, 0};
  unsigned int split_face_vertices[] = {0, 0, 0, 0, 0, 0};
  for (int i = 0, new_i = 0; i < n_faces * 3; i += 3, new_i += 3) {
    face[0] = faces_tmp.at(i);
    face[1] = faces_tmp.at(i + 1);
    face[2] = faces_tmp.at(i + 2);
    split_triangle(face, uniques_vertices, split_face_vertices);

    faces.at(new_i) = split_face_vertices[0];
    faces.at(new_i + 1) = split_face_vertices[1];
    faces.at(new_i + 2) = split_face_vertices[5];

    new_i += 3;
    faces.at(new_i) = split_face_vertices[1];
    faces.at(new_i + 1) = split_face_vertices[3];
    faces.at(new_i + 2) = split_face_vertices[5];

    new_i += 3;
    faces.at(new_i) = split_face_vertices[1];
    faces.at(new_i + 1) = split_face_vertices[2];
    faces.at(new_i + 2) = split_face_vertices[3];

    new_i += 3;
    faces.at(new_i) = split_face_vertices[3];
    faces.at(new_i + 1) = split_face_vertices[4];
    faces.at(new_i + 2) = split_face_vertices[5];
  }

  for (const auto &[key, value] : uniques_vertices) {
    splitvertice_to_vertice(vertices_tmp, key, vertices.begin() + value * 3);
  }
  n_faces = n_faces * 4;
  n_vertices = (int)vertices.size() / 3;
  set_edges();
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
