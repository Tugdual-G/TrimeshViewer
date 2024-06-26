// #include "mesh.hpp"
#include "plyfile.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <vector>

static unsigned int get_property_index(PropertyName name, Element &elem);

int PlyFile::from_file(const char *fname) {
  // vertices_elements_sizes.resize(10, 0);
  std::ifstream file;
  file.open(fname, std::ios::binary | std::ios::in);
  parse_header(&file);

  if (!load_data(&file)) {
    std::cout << "Error parsing data in : " << fname << "\n";
    exit(1);
  }

  file.close();

  return 1;
}

int check_same_type(Element &elem, const std::vector<PropertyName> &names) {
  int idx;
  idx = get_property_index(names.at(0), elem);
  if (idx == -1) {
    return 1;
  }
  PropertyType previous_type = elem.property_types.at(idx);
  for (auto &name : names) {
    idx = get_property_index(name, elem);
    if (idx == -1) {
      return 1;
    }
    if (elem.property_types.at(idx) != previous_type) {
      return 0;
    }
    previous_type = elem.property_types.at(idx);
  }
  return 1;
}

int PlyFile::load_data(std::ifstream *file) {

  //     std::vector<float> vertices_tmp(2 * n_vertices * n_dim, -99999);
  //     file->read((char *)vertices_tmp.data(),
  //                2 * n_vertices * n_dim * sizeof(float));

  //     double *v = vertices.data();
  //     double *n = vertex_normals.data();
  //     for (float *v_tmp = vertices_tmp.data();
  //          v_tmp < vertices_tmp.data() + 2 * n_vertices * n_dim;
  //          v_tmp += 6, v += 3, n += 3) {
  //       std::copy(v_tmp, v_tmp + 3, v);
  //       std::copy(v_tmp + 3, v_tmp + 6, n);
  //     }
  // vertices.resize(n_vertices * n_dim, -1);
  unsigned int stride;
  int i;

  const std::vector<PropertyName> vertex_pos = {
      PropertyName::x, PropertyName::y, PropertyName::z};

  const std::vector<PropertyName> normal_pos = {
      PropertyName::nx, PropertyName::ny, PropertyName::nz};

  for (auto &elem : elements) {
    stride = get_element_stride(elem);
    switch (elem.type) {
    case ElementType::VERTEX: {
      if (!check_same_type(elem, vertex_pos)) {
        std::cout << "Error, vertex position must use the same type of float\n";
        exit(1);
      }
      if (!check_same_type(elem, normal_pos)) {
        std::cout
            << "Error, normal coordinates must use the same type of float\n";
        exit(1);
      }
      break;
    }
    case ElementType::FACE: {
      break;
    }
    default: {
      std::cout << "Warning, element type not handled \n";
      break;
    }
    }
  }

  return 1;
}

int PlyFile::parse_header(std::ifstream *file) {
  PlyFile::data_layout.reserve(7);
  std::string line, word;
  int i = 0;
  if (std::getline(*file, line)) {
    // std::stringstream iss(line, std::istringstream::in);
    // iss >> word;
    if (line != "ply") {
      std::cout
          << "Error, this file doesn't look like a ply file. line read :\n"
          << line << std::endl;
      exit(1);
    }
  }
  while (std::getline(*file, line) && i < 10) {
    // std::cout << line << std::endl;
    ++i;
    std::stringstream iss(line, std::istringstream::in);
    if (iss >> word) {
      switch (entries_map.at(word)) {
      case Entries::FORMAT:
        // TODO handle binary format
        break;
      case Entries::ELEMENT:
        iss >> word;
        switch (elem_type_map.at(word)) {
        case ElementType::VERTEX:
          iss >> n_vertices;
          if (!n_vertices) {
            std::cout << "Error, number of vertices = 0 , last line read :\n"
                      << line << std::endl;
            exit(1);
          }
          parse_vertices_properties(line, file, n_vertices);
          break;
        case ElementType::FACE:
          iss >> n_faces;
          if (!n_faces) {
            std::cout << "Error, number of faces = 0, last line read : \n"
                      << line << std::endl;
            exit(1);
          }
          parse_faces_properties(line, file, n_faces);
          break;
        default:
          std::cout << "Error, " << word << " element type not implemented \n";
          exit(1);
          break;
        }
        break;
      case Entries::END:
        std::cout << "header read" << std::endl;
        file_data_offset = file->tellg();
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
  exit(1);
}

int PlyFile::parse_vertices_properties(std::string &line, std::ifstream *file,
                                       unsigned int n_elem) {
  Element vertex_element;
  vertex_element.n_elem = n_elem;
  vertex_element.type = ElementType::VERTEX;
  vertex_element.property_names.reserve(9);
  vertex_element.property_types.reserve(9);
  int i = 0;
  std::string word;
  int last_offset = file->tellg();
  while (std::getline(*file, line) && i < 9) {
    std::stringstream iss(line, std::istringstream::in);
    iss >> word;
    if (entries_map.at(word) != Entries::PROPERTY) {
      if (i == 0) {
        std::cout << "Error, expected element definition line to be folowed by "
                     "property statement \n";
        exit(1);
      }
      elements.push_back(vertex_element);
      file->seekg(last_offset);
      return 1;
    }
    iss >> word;
    vertex_element.property_types.push_back(prop_type_map.at(word));

    iss >> word;
    vertex_element.property_names.push_back(prop_name_map.at(word));

    last_offset = file->tellg();
    ++i;
  }
  std::cout << "Warning, max number of vertex properties reached : " << i
            << std::endl;

  elements.push_back(vertex_element);
  file->seekg(last_offset);
  return 0;
};

int PlyFile::parse_faces_properties(std::string &line, std::ifstream *file,
                                    unsigned int n_elem) {
  Element face_element;
  face_element.n_elem = n_elem;
  face_element.type = ElementType::FACE;
  face_element.property_names.reserve(9);
  face_element.property_types.reserve(9);
  face_element.lists.reserve(1);
  int i = 0;
  std::string word;
  int last_offset = file->tellg();

  while (std::getline(*file, line) && i < 9) {
    std::stringstream iss(line, std::istringstream::in);
    iss >> word;
    if (entries_map.at(word) != Entries::PROPERTY) {
      if (i == 0) {
        std::cout << "Error, expected element definition line to be folowed by "
                     "property statement \n";
        exit(1);
      }
      elements.push_back(face_element);
      file->seekg(last_offset);
      return 1;
    }

    iss >> word;
    switch (prop_type_map.at(word)) {
    case PropertyType::LIST: {
      std::vector<PropertyType> types;
      types.reserve(2);
      while (iss >> word) {
        if (prop_type_map.find(word) != prop_type_map.end()) {
          types.push_back(prop_type_map.at(word));
        } else if ((word != "vertex_indices") && (word != "vetex_indices")) {
          std::cout << "Error: face list of " << word << " not recognized \n";
          exit(1);
        } else {
          break;
        }
      }

      if (prop_name_map.find(word) == prop_name_map.end()) {
        std::cout << " Error, this should not have hapenend =( \n";
        std::cout << word << std::endl;
        exit(1);
      }
      face_element.property_types.push_back(PropertyType::LIST);
      face_element.property_names.push_back(prop_name_map.at(word));
      face_element.lists.push_back(types);
      break;
    }
    default:
      face_element.property_types.push_back(prop_type_map.at(word));
      iss >> word;
      face_element.property_names.push_back(prop_name_map.at(word));
      break;
    }

    ++i;
  }

  elements.push_back(face_element);
  file->seekg(last_offset);
  std::cout << "Warning, max number of face properties reached : " << i
            << std::endl;
  std::cout << line << std::endl;

  return 0;
};

unsigned int get_property_index(PropertyName name, Element &elem) {
  unsigned int i = 0;
  std::vector<PropertyName>::iterator n = elem.property_names.begin();
  while (n != elem.property_names.end()) {
    if (*n != name) {
      return i;
    }
    ++n;
    ++i;
  }
  return -1; // Returns -1 if not found
};

unsigned int PlyFile::get_element_stride(Element elem) {
  unsigned int stride; // stride in bytes
  for (auto &type : elem.property_types) {
    stride += type_size_map.at(type);
  }
  return stride;
}

template <class T, class U>
void inverse_map(std::unordered_map<T, U> const &map,
                 std::unordered_map<U, T> &rmap) {
  rmap.clear();
  for (const auto &[key, value] : map) {
    rmap[value] = key;
  }
}
void PlyFile::build_inverse_maps() {
  inverse_map<std::string, Entries>(entries_map, entries_rmap);
  inverse_map<std::string, PropertyType>(prop_type_map, prop_type_rmap);
  inverse_map<std::string, PropertyName>(prop_name_map, prop_name_rmap);
  inverse_map<std::string, ElementType>(elem_type_map, elem_type_rmap);
};

void PlyFile::print() {
  build_inverse_maps();
  std::cout << "ply\n";
  std::cout << "format --------\n";
  std::cout << "comment --------\n";
  unsigned int list_idx = 0;
  for (auto &elem : elements) {
    std::cout << "element " << elem_type_rmap.at(elem.type);
    std::cout << " " << elem.n_elem << "\n";
    for (unsigned int i = 0; i < elem.property_types.size(); ++i) {
      std::cout << "property " << prop_type_rmap.at(elem.property_types.at(i));

      if (elem.property_types.at(i) == PropertyType::LIST) {
        for (auto &lp : elem.lists.at(list_idx)) {
          std::cout << " " << prop_type_rmap.at(lp);
        }
        ++list_idx;
      }
      std::cout << " " << prop_name_rmap.at(elem.property_names.at(i)) << "\n";
    }
  }
  std::cout << "end_header\n";
  std::cout << "data offset : " << file_data_offset << "\n";
}
