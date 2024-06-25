#include "mesh.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <variant>
#include <vector>

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

int PlyFile::from_file(const char *fname) {
  // vertices_elements_sizes.resize(10, 0);
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
  if (vertex_type != normal_type && (normal_type != NONE)) {
    std::cout << "Error, normals and vertices are of different float types \n";
    exit(1);
  }

  if (!load_data(&file)) {
    std::cout << "Error parsing data in : " << fname << "\n";
    exit(1);
  }

  file.close();

  return 1;
}

int PlyFile::load_data(std::ifstream *file) {
  if (n_dim != 3) {
    std::cout << "Warning, n_dim != 3 \n";
    return 0;
  }
  vertices.resize(n_vertices * n_dim, -1);

  if (data_layout == "vvv") {
    switch (vertex_type) {
    case FLOAT: {
      std::vector<float> vertices_tmp(n_vertices * n_dim);
      file->read((char *)vertices_tmp.data(),
                 n_vertices * n_dim * sizeof(float));
      std::copy(vertices_tmp.begin(), vertices_tmp.end(), vertices.begin());
      break;
    }
    case DOUBLE:
      file->read((char *)vertices.data(), n_vertices * n_dim * sizeof(double));
      break;
    default:
      std::cout << "Warning, data has NONE type. \n";
      break;
    }
  } else if (data_layout == "vvvnnn") {
    vertex_normals.resize(n_vertices * n_dim, -1);
    switch (vertex_type) {
    case FLOAT: {
      std::vector<float> vertices_tmp(2 * n_vertices * n_dim, -99999);
      file->read((char *)vertices_tmp.data(),
                 2 * n_vertices * n_dim * sizeof(float));

      double *v = vertices.data();
      double *n = vertex_normals.data();
      for (float *v_tmp = vertices_tmp.data();
           v_tmp < vertices_tmp.data() + 2 * n_vertices * n_dim;
           v_tmp += 6, v += 3, n += 3) {
        std::copy(v_tmp, v_tmp + 3, v);
        std::copy(v_tmp + 3, v_tmp + 6, n);
      }
      break;
    }
    case DOUBLE: {
      for (double *v = vertices.data(), *n = vertex_normals.data();
           v < vertices.data() + n_vertices * n_dim; v += 3, n += 3) {
        file->read((char *)v, n_dim * sizeof(float));
        file->read((char *)n, n_dim * sizeof(float));
      }
      break;
    }
    default:
      std::cout << "Warning, data has NONE type. \n";
      return 0;
    }
  } else {
    std::cout << "Warning, data layout not implemented :" << data_layout
              << std::endl;
    return 0;
  }

  if (n_faces < 1) {
    std::cout << "Warning, wrong number of faces \n";
    return 0;
  }
  faces.resize(n_faces * 3, -1);
  // store the number of vertices per face given at the begining of each line.
  char nv = 'q';
  for (auto i = 0; i < n_faces; ++i) {
    file->read(&nv, sizeof(char));
    if (nv != 3) {
      std::cout
          << "Warning, this program cannot handle non-triangular meshes.\n";
      std::cout << "last caracter read : \n" << nv << std::endl;
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

int PlyFile::parse_header(std::ifstream *file) {
  PlyFile::data_layout.reserve(7);
  std::string line, word;
  n_dim = 0;
  int i = 0;
  if (std::getline(*file, line)) {
    std::stringstream iss(line, std::istringstream::in);
    iss >> word;
    if (str2entries(word) != PLY) {
      std::cout
          << "Warning, this file doesn't look like a ply file. line read :\n"
          << line << std::endl;
      return 0;
    }
  }
  while (std::getline(*file, line) && i < 20) {
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
            std::cout << "warning, number of vertices = 0 , last line read :\n"
                      << line << std::endl;
            return 0;
          }
        } else if (str2entries(word) == FACE) {
          iss >> n_faces;
          if (!n_faces) {
            std::cout << "warning, number of faces = 0, last line read : \n"
                      << line << std::endl;
            return 0;
          }
        } else {
          return 0;
        }
        break;
      case PROPERTY:
        iss >> word;
        if (word == "float") {
          iss >> word;
          if (word == "x" || word == "y" || word == "z") {
            if (vertex_type == DOUBLE) {
              std::cout
                  << "Warning, different float types are used to describe "
                     "the vertices. \n";
              return 0;
            }
            data_layout.push_back('v');
            vertex_type = FLOAT;
            ++n_dim;
          } else if (word == "nx" || word == "ny" || word == "nz") {
            if (vertex_type == DOUBLE) {
              std::cout
                  << "Warning, different float types are used to describe "
                     "the normals. \n";
              return 0;
            }
            data_layout.push_back('n');
            normal_type = FLOAT;
          }
          break;
        } else if (word == "double") {
          iss >> word;
          if (word == "x" || word == "y" || word == "z") {
            if (vertex_type == FLOAT) {
              std::cout
                  << "Warning, different float types are used to describe "
                     "the vertices. \n";
              return 0;
            }
            data_layout.push_back('v');
            vertex_type = DOUBLE;
            ++n_dim;
          } else if (word == "nx" || word == "ny" || word == "nz") {
            if (vertex_type == FLOAT) {
              std::cout
                  << "Warning, different float types are used to describe "
                     "the normals. \n";
              return 0;
            }

            data_layout.push_back('n');
            normal_type = DOUBLE;
          }
          break;
        } else if (word == "uchar") {
          iss >> word;
          if (word == "red" || word == "green" || word == "blue" ||
              word == "alpha") {
            std::cout << "Warning, colors not implemented: " << line
                      << std::endl;
            data_layout.push_back('c');
          } else {
            std::cout << "Warning, property not implemented: " << line
                      << std::endl;
          }
        } else if (word == "list") {
        } else {
          std::cout << "Warning, unrecognized property : " << line << std::endl;
        }
        iss >> word;
        break;
      case END:
        // std::getline(*file, line);
        file_data_offset = file->tellg();
        if (file_data_offset < 10) {
          std::cout << "Data offset error : last line read :\n"
                    << line << std::endl;
          return 0;
        }
        return 1;
      default:
        break;
      }
    } else {
      std::cout << "Warning, cannot read : " << line << std::endl;
    }
  }
  std::cout << "Warning, end of header not found, last line read :\n"
            << line << std::endl;
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

void Mesh::print() {
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

void Mesh::print_vertices() {
  for (int i = 0; i < n_vertices; ++i) {
    for (int j = 0; j < 3; ++j) {
      std::cout << vertices.at(i * 3 + j) << " ";
    }
    std::cout << "\n";
  }
}

void Mesh::print_faces() {
  for (int i = 0; i < n_faces; ++i) {
    std::cout << "face " << i << " : ";
    for (int j = 0; j < 3; ++j) {
      std::cout << faces.at(i * 3 + j) << " ";
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
  int face;
  for (int i = 0; i < n_vertices; ++i) {
    n_adja = vertex_adjacent_faces.at(adja_array_idx);
    std::cout << "vert  " << i << " : ";
    for (int j = 0; j < n_adja; ++j) {
      ++adja_array_idx;
      face = vertex_adjacent_faces.at(adja_array_idx);
      std::cout << face << " ";
    }
    std::cout << std::endl;
    ++adja_array_idx;
  }
}

void Mesh::print_one_ring() {
  int n_adja = 0;
  int one_ring_array_idx = 0;
  int vert;
  for (int i = 0; i < n_vertices; ++i) {
    n_adja = one_ring.at(one_ring_array_idx);
    std::cout << "vert  " << i << " : ";
    for (int j = 0; j < n_adja; ++j) {
      ++one_ring_array_idx;
      vert = one_ring.at(one_ring_array_idx);
      std::cout << vert << " ";
    }
    std::cout << std::endl;
    ++one_ring_array_idx;
  }
}

void Mesh::print_vertex_normals() {
  for (int i = 0; i < n_vertices; ++i) {
    for (int j = 0; j < 3; ++j) {
      std::cout << vertex_normals.at(i * 3 + j) << " ";
    }
    std::cout << std::endl;
  }
}
