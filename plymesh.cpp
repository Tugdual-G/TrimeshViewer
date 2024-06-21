#include "plymesh.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <variant>
#include <vector>

void normalize(double *w);
void vector_prod(double *u, double *v, double *w);

void PlyMesh::set_vertex_adjacent_faces() {
  // TODO do not harcode the max number of adjacent faces.
  vertex_adjacent_faces.resize(vertices.size() * 10, -1);
  int n_adja = 1;
  long unsigned int total_size = 0;
  for (unsigned int i = 0; i < (unsigned int)n_vertices; ++i) {
    n_adja = 0;
    ++total_size; // since we store the number of adj at the begining.
                  // TODO maybe vector can handle this with reserve
    if (total_size >= vertex_adjacent_faces.size()) {
      vertex_adjacent_faces.resize(total_size + vertices.size());
    }
    for (unsigned int j = 0; j < (unsigned int)n_faces; ++j) {
      for (unsigned int k = 0; k < 3; ++k) {
        if (faces[j * 3 + k] == i) {
          if (total_size >= vertex_adjacent_faces.size()) {
            vertex_adjacent_faces.resize(total_size + vertices.size());
          }
          vertex_adjacent_faces.at(total_size) = j;
          ++n_adja;
          ++total_size;
        }
      }
    }
    vertex_adjacent_faces[total_size - n_adja - 1] = n_adja;
    if (!n_adja) {
      std::cout << "Error, vertex without an adjacent face.\n";
    }
  }
  vertex_adjacent_faces.resize(total_size);
}

void PlyMesh::set_vertex_normals() {
  if (vertex_adjacent_faces.size() == 0) {
    set_vertex_adjacent_faces();
  }
  if (face_normals.size() == 0) {
    set_face_normals();
  }

  vertex_normals.resize(n_vertices * 3, 0);
  int n_adja = 0;
  int adja_array_idx = 0;
  int face;
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

void PlyMesh::set_face_normals() {
  face_normals.resize(faces.size(), 0);
  unsigned int i, j, k;
  double e0[3], e1[3];
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

void vector_prod(double *u, double *v, double *w) {
  w[0] = u[1] * v[2] - u[2] * v[1];
  w[1] = u[2] * v[0] - u[0] * v[2];
  w[2] = u[0] * v[1] - u[1] * v[0];
}

enum Entries {
  VOID = 0,
  PLY,
  FORMAT,
  ELEMENT,
  PROPERTY,
  FACE,
  VERTEX,
  COMMENT,
  END,
};

Entries str2entries(std::string_view word);

int PlyMesh::from_file(const char *fname) {
  vertices_elements_sizes.resize(10, 0);
  std::ifstream file;
  file.open(fname, std::ios::binary | std::ios::in);
  if (!parse_header(&file)) {
    std::cout << "Error parsing file : " << fname << " header \n";
    exit(1);
  }
  if (vertex_type == NONE) {
    std::cout << "Error : vertex float type = NONE \n";
    exit(1);
  }

  if (!load_data(&file)) {
    std::cout << "Error parsing data in : " << fname << "\n";
    exit(1);
  }
  file.close();
  return 1;
}

int PlyMesh::load_data(std::ifstream *file) {
  if (n_dim != 3) {
    std::cout << "Warning, n_dim != 3 \n";
    return 0;
  }
  vertices.resize(n_vertices * n_dim, -1);

  if (vertex_type == FLOAT && (normal_type == FLOAT || normal_type == NONE)) {
    std::vector<float> vertices_tmp(n_vertices * n_dim);
    file->read((char *)vertices_tmp.data(), n_vertices * n_dim * sizeof(float));
    std::copy(vertices_tmp.begin(), vertices_tmp.end(), vertices.begin());
  } else if (vertex_type == DOUBLE &&
             (normal_type == DOUBLE || normal_type == NONE)) {
    file->read((char *)vertices.data(), n_vertices * n_dim * sizeof(double));
  } else {
    std::cout
        << "Warning, normals and vertices are of different float types \n";
    return 0;
  }

  if (n_faces < 1) {
    std::cout << "Warning, wrong number of faces \n";
    return 0;
  }
  faces.resize(n_faces * 3, -1);
  char nv = 'q';
  for (auto i = 0; i < n_faces; ++i) {
    file->read(&nv, sizeof(char));
    if (nv != 3) {
      std::cout
          << "Warning, this program cannot handle non-triangular meshes.\n";
      return 0;
    }
    file->read((char *)&faces.data()[3 * i], 3 * sizeof(int));
  }
  vertices_per_face = (int)nv;
  const auto [min, max] = std::minmax_element(vertices.begin(), vertices.end());
  if (*min == *max) {
    std::cout << "Error, all coordinates ares equal.\n"; // paranoid check
    return 0;
  }

  return 1;
}

int PlyMesh::parse_header(std::ifstream *file) {
  std::string line, word;
  n_vertice_elements = 0;
  n_dim = 0;
  int i = 0;
  if (std::getline(*file, line)) {
    std::stringstream iss(line, std::istringstream::in);
    iss >> word;
    if (str2entries(word) != PLY) {
      return 0;
    }
  }
  while (std::getline(*file, line) && i < 10) {
    // std::cout << line << std::endl;
    ++i;
    std::stringstream iss(line, std::istringstream::in);
    if (iss >> word) {
      switch (str2entries(word)) {
      case FORMAT:
        break;
      case ELEMENT:
        iss >> word;
        if (str2entries(word) == VERTEX) {
          iss >> n_vertices;
          if (!n_vertices) {
            return 0;
          }
        } else if (str2entries(word) == FACE) {
          iss >> n_faces;
          if (!n_faces) {
            return 0;
          }
        } else {
          return 0;
        }
        break;
      case PROPERTY:
        iss >> word;
        if (n_vertice_elements == 9) {
          std::cout << "max elements reached \n";
          return 0;
        }
        if (word == "float") {
          vertices_elements_sizes[n_vertice_elements] = sizeof(float);
          ++n_vertice_elements;
          iss >> word;
          if (word == "x" || word == "y" || word == "z") {
            vertex_type = FLOAT;
            ++n_dim;
          } else if (word == "nx" || word == "ny" || word == "nz") {
            normal_type = FLOAT;
          }
          break;
        } else if (word == "double") {
          vertices_elements_sizes[n_vertice_elements] = sizeof(double);
          ++n_vertice_elements;
          iss >> word;
          if (word == "x" || word == "y" || word == "z") {
            vertex_type = DOUBLE;
            ++n_dim;
          } else if (word == "nx" || word == "ny" || word == "nz") {
            normal_type = DOUBLE;
          }
          break;
        } else if (word == "list") {
        } else {
          return 0;
        }
        iss >> word;
        break;
      case END:
        // std::getline(*file, line);
        file_data_offset = file->tellg();
        if (file_data_offset == -1) {
          std::cout << "Data offset error \n";
          return 0;
        }
        return 1;
      default:
        break;
      }
    }
  }
  return 0;
}

Entries str2entries(std::string_view word) {
  if (word == "ply") {
    return PLY;
  } else if (word == "format") {
    return FORMAT;
  } else if (word == "element") {
    return ELEMENT;
  } else if (word == "property") {
    return PROPERTY;
  } else if (word == "face") {
    return FACE;
  } else if (word == "vertex" || word == "vertice" || word == "vertices") {
    return VERTEX;
  } else if (word == "end_header") {
    return END;
  } else if (word == "comment") {
    return COMMENT;
  } else {
    return VOID;
  }
}

void PlyMesh::print() {
  std::cout << "ply\n";
  std::cout << "format --------\n";
  std::cout << "comment --------\n";
  std::cout << "element vertex " << n_vertices << "\n";
  std::cout << "property float x--- \n";
  std::cout << "property float y--- \n";
  std::cout << "property float z--- \n";
  std::cout << "element face " << n_faces << "\n";
  std::cout << "property --------\n";
  std::cout << "end_header\n";
  std::cout << "data offset : " << file_data_offset << "\n";
}

void PlyMesh::print_vertices() {
  for (int i = 0; i < n_vertices; ++i) {
    for (int j = 0; j < 3; ++j) {
      std::cout << vertices.at(i * 3 + j) << " ";
    }
    std::cout << "\n";
  }
}

void PlyMesh::print_faces() {
  for (int i = 0; i < n_faces; ++i) {
    for (int j = 0; j < 3; ++j) {
      std::cout << faces.at(i * 3 + j) << " ";
    }
    std::cout << "\n";
  }
}

void PlyMesh::print_face_normals() {
  for (int i = 0; i < n_faces; ++i) {
    for (int j = 0; j < 3; ++j) {
      std::cout << face_normals.at(i * 3 + j) << " ";
    }
    std::cout << "\n";
  }
}

void PlyMesh::print_vertex_adjacent_face() {
  int n_adja = 0;
  int adja_array_idx = 0;
  int face;
  for (int i = 0; i < n_vertices; ++i) {
    n_adja = vertex_adjacent_faces.at(adja_array_idx);
    std::cout << "n  " << n_adja << " : ";
    for (int j = 0; j < n_adja; ++j) {
      ++adja_array_idx;
      face = vertex_adjacent_faces.at(adja_array_idx);
      std::cout << face << " ";
    }
    std::cout << std::endl;
    ++adja_array_idx;
  }
}

void PlyMesh::print_vertex_normals() {
  for (int i = 0; i < n_vertices; ++i) {
    for (int j = 0; j < 3; ++j) {
      std::cout << vertex_normals.at(i * 3 + j) << " ";
    }
    std::cout << std::endl;
  }
}
