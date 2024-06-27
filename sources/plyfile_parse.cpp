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

template <class T> void print_vect(std::vector<T> v, int m, int n);

template <class IN_TYPE, class OUT_TYPE>
void PlyFile::retrieve_subelement_data(SubElement &subelement,
                                       std::vector<OUT_TYPE> &sub_data) {
  /* Retrieve the data for a subset of element property in
   * separated vector. For example, if only the vertices position are needed.
   * */

  unsigned int stride = get_element_stride(*subelement.parent);

  std::vector<int> parent_elem_data_offset;

  parent_elem_data_offset.reserve(subelement.property_names.size());

  for (auto &prop : subelement.property_names) {
    // hopefully the layout in the file is in the right order.
    parent_elem_data_offset.push_back(
        get_property_offset(prop, *subelement.parent));
  }

  std::vector<char> &parent_data = subelement.parent->data;

  unsigned int i = 0;
  for (char *p_data = parent_data.data();
       p_data < parent_data.data() + parent_data.size(); p_data += stride) {
    for (auto &offset : parent_elem_data_offset) {
      sub_data.at(i) = (OUT_TYPE) * ((IN_TYPE *)(p_data + offset));
      ++i;
    }
  }
};

template <class IN_TYPE>
void PlyFile::retrieve_face_data(Element &face_element,
                                 std::vector<unsigned int> &sub_data) {

  unsigned int offset, size;
  offset = get_property_offset(PropertyName::vertex_indices, face_element);
  // TODO is it the right list ? not always
  offset += type_size_map.at(face_element.lists.at(0).at(0));
  size = type_size_map.at(face_element.lists.at(0).at(1));

  unsigned int stride = get_element_stride(face_element);
  unsigned int tail = stride - offset - 3 * size;

  char *p_data = face_element.data.data();
  unsigned int i = 0;
  while (p_data < face_element.data.data() + face_element.data.size()) {
    p_data += offset;
    for (unsigned int j = 0; j < 3; ++j) {
      sub_data.at(i) = (unsigned int)*((IN_TYPE *)p_data);
      p_data += size;
      ++i;
    }
    p_data += tail;
  }
};

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
  print();

  SubElement vertices_sub, faces_sub;
  vertices_sub.property_names = {PropertyName::x, PropertyName::y,
                                 PropertyName::z};
  for (auto &elem : elements) {
    if (elem.type == ElementType::VERTEX) {
      vertices_sub.parent = &elem;
    }
    if (elem.type == ElementType::FACE) {
      faces_sub.parent = &elem;
    }
  }

  vertices.resize(n_vertices * 3, -9999);
  faces.resize(n_faces * 3, -9999);
  retrieve_subelement_data<float, double>(vertices_sub, vertices);
  retrieve_face_data<int>(*faces_sub.parent, faces);

  return 1;
}

int check_same_type(Element &elem, const std::vector<PropertyName> &names,
                    PropertyType &type) {
  /*
   * Checks if a list of property names share the same type.
   *
   *  */
  type = PropertyType::NONE;
  auto name = names.begin();
  int idx = -1;
  // finding the first known property
  // get_property_index return -1 if unknown
  while (idx == -1 && name != names.end()) {
    idx = get_property_index(*name, elem);
    ++name;
  }
  if (idx == -1) {
    return 1;
  }

  type = elem.property_types.at(idx);
  while (name != names.end()) {
    idx = get_property_index(*name, elem);
    if (idx != -1) {
      if (elem.property_types.at(idx) != type) { // check against previous type
        return 0;
      }
      type = elem.property_types.at(idx);
    }
    ++name;
  }
  return 1;
}

int PlyFile::load_data(std::ifstream *file) {

  set_elements_file_begin_position(); // where each_element data starts in the
                                      // file
  unsigned int stride;
  PropertyType type;

  const std::vector<PropertyName> vertex_pos = {
      PropertyName::x, PropertyName::y, PropertyName::z};

  const std::vector<PropertyName> normal_pos = {
      PropertyName::nx, PropertyName::ny, PropertyName::nz};

  const std::vector<PropertyName> colors = {
      PropertyName::red, PropertyName::green, PropertyName::blue,
      PropertyName::alpha};

  for (auto &elem : elements) {
    stride = get_element_stride(elem);
    if (stride > 100) {
      std::cout
          << "Error, element stride to large, might consume to much memory \n";
      file->close();
      exit(1);
    }

    switch (elem.type) {
    case ElementType::VERTEX: {
      if (!check_same_type(elem, vertex_pos, type)) {
        std::cout << "Error, vertex position must use the same type of float\n";
        file->close();
        exit(1);
      }
      if (!check_same_type(elem, normal_pos, type)) {
        std::cout
            << "Error, normal coordinates must use the same type of float\n";
        file->close();
        exit(1);
      }

      if (!check_same_type(elem, colors, type)) {
        std::cout
            << "Error, colors coordinates must use the same data type. \n";
        file->close();
        exit(1);
      }
      elem.data.resize(stride * elem.n_elem, -127);
      file->read(elem.data.data(), stride * elem.n_elem);
      break;
    }
    case ElementType::FACE: {
      // TODO take the file offset !!!!
      if (!check_same_type(elem, colors, type)) {
        std::cout
            << "Error, colors coordinates must use the same data type. \n";
        file->close();
        exit(1);
      }
      elem.data.resize(stride * elem.n_elem, 127);
      file->read(elem.data.data(), stride * elem.n_elem);
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
      file->close();
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
            file->close();
            exit(1);
          }
          parse_vertices_properties(line, file, n_vertices);
          break;
        case ElementType::FACE:
          iss >> n_faces;
          if (!n_faces) {
            std::cout << "Error, number of faces = 0, last line read : \n"
                      << line << std::endl;
            file->close();
            exit(1);
          }
          parse_faces_properties(line, file, n_faces);
          break;
        default:
          std::cout << "Error, " << word << " element type not implemented \n";
          file->close();
          exit(1);
          break;
        }
        break;
      case Entries::END:
        file_data_offset = file->tellg();
        return 1;
      default:
        break;
      }
    } else {
      std::cout << "Warning, cannot read : " << line << std::endl;
      file->close();
      exit(1);
    }
  }
  std::cout << "Warning, end of header not found, last line read :\n"
            << line << std::endl;
  file->close();
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
        file->close();
        exit(1);
      }
      elements.push_back(vertex_element);
      file->seekg(last_offset);
      return 1;
    }
    iss >> word;
    if (prop_type_map.find(word) == prop_type_map.end()) {
      std::cout << "Warning, uknown type \" " << word << " \" \n";
      exit(1);
    }
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

        file->close();
        exit(1);
      }
      // End of faces propr
      elements.push_back(face_element);
      file->seekg(last_offset);
      return 1;
    }

    iss >> word;
    if (prop_type_map.find(word) == prop_type_map.end()) {
      std::cout << "Warning, uknown type \" " << word << " \" \n";
      exit(1);
    }
    switch (prop_type_map.at(word)) {
    case PropertyType::LIST: {
      std::vector<PropertyType> types;
      types.reserve(2);
      while (iss >> word) {
        if (prop_type_map.find(word) != prop_type_map.end()) {
          types.push_back(prop_type_map.at(word));
        } else if (prop_name_map.find(word) == prop_name_map.end()) {
          std::cout << "Error: face list of " << word << " not recognized \n";
          file->close();
          exit(1);
        } else {
          break;
        }
      }

      if (prop_name_map.find(word) == prop_name_map.end()) {
        std::cout << " Error, this should not have hapenend =( \n";
        std::cout << word << std::endl;
        file->close();
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

int PlyFile::get_property_offset(PropertyName name, Element &elem) {
  /* Return the property (x, ny, blue) position in the element data. */
  unsigned int offset{0};  // offset in bytes
  unsigned int n_list = 0; // number of list read
  PropertyType list_type;
  std::vector<PropertyName>::iterator n_current_name =
      elem.property_names.begin();
  for (auto &type : elem.property_types) {
    if (*n_current_name == name) {
      return offset;
    }
    if (type == PropertyType::LIST) {
      list_type = elem.lists.at(n_list).at(0);
      offset += type_size_map.at(list_type);
      // TODO will only work for triangular meshes
      list_type = elem.lists.at(n_list).at(1);
      offset += type_size_map.at(list_type) * 3;
      ++n_list;
    } else {
      offset += type_size_map.at(type);
    }
    ++n_current_name;
  }
  return -1;
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

unsigned int PlyFile::get_element_stride(Element &elem) {
  unsigned int stride{0};  // stride in bytes
  unsigned int n_list = 0; // number of list read
  PropertyType list_type;
  for (auto &type : elem.property_types) {
    if (type == PropertyType::LIST) {
      list_type = elem.lists.at(n_list).at(0);
      stride += type_size_map.at(list_type);
      // TODO will only work for triangular meshes
      list_type = elem.lists.at(n_list).at(1);
      stride += type_size_map.at(list_type) * 3;
      ++n_list;
    } else {
      stride += type_size_map.at(type);
    }
  }
  return stride;
}

void PlyFile::set_elements_file_begin_position() {
  if (!file_data_offset) {
    std::cout << "Set file_data_offset before file_begin_pos \n";
    exit(1);
  }
  int global_offset = file_data_offset;
  for (auto &elem : elements) {
    elem.file_begin_pos = global_offset;
    global_offset += elem.n_elem * get_element_stride(elem);
  }
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

template <class T> void print_vect(std::vector<T> v, int m, int n) {
  std::cout << "\n";
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < n; ++j) {
      std::cout << v.at(i * n + j) << " ";
    }
    std::cout << "\n";
  }
}
