#ifndef PLYFILE_H_
#define PLYFILE_H_

// #include "mesh.hpp"
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

enum class Entries {
  NONE,
  PLY,
  FORMAT,
  ELEMENT,
  PROPERTY,
  COMMENT,
  END,
};

enum class PropertyType {
  NONE,    // bytes
  CHAR,    // 1
  UCHAR,   // 1
  SHORT,   // 2
  USHORT,  // 2
  INT,     // 4
  UINT,    // 4
  FLOAT,   // 4
  FLOAT32, //
  FLOAT64, //
  DOUBLE,  // 8
  LIST,
};

enum class PropertyName {
  NONE,
  x,
  y,
  z,
  nx,
  ny,
  nz,
  red,
  green,
  blue,
  alpha,
  vertex_indices,
};

enum class ElementType {
  NONE,
  VERTEX,
  FACE,
};

typedef struct Element {
  ElementType type{ElementType::NONE};
  unsigned int n_elem{0}, stride{0};
  std::vector<PropertyName> property_names;
  std::vector<PropertyType> property_types;
  std::vector<std::vector<PropertyType>> lists{0};
  std::vector<char> data;
  int file_begin_pos{-1};
} Element;

// typedef struct SubElement {
//   std::vector<PropertyName> property_names;
//   Element *parent;
// } SubElement;

class PlyFile {
  int file_data_offset{0};
  // Element &vertex_element;
  // Element &face_element;
  // std::vector<int> vertices_elements_sizes;

  std::unordered_map<std::string, Entries> const entries_map{
      {"", Entries::NONE},
      {"ply", Entries::PLY},
      {"format", Entries::FORMAT},
      {"element", Entries::ELEMENT},
      {"property", Entries::PROPERTY},
      {"comment", Entries::COMMENT},
      {"end_header", Entries::END}};
  std::unordered_map<Entries, std::string> entries_rmap;

  std::unordered_map<std::string, PropertyType> const prop_type_map{
      {"", PropertyType::NONE},          //
      {"char", PropertyType::CHAR},      //
      {"uchar", PropertyType::UCHAR},    //
      {"short", PropertyType::SHORT},    //
      {"ushort", PropertyType::USHORT},  //
      {"int", PropertyType::INT},        //
      {"uint", PropertyType::UINT},      //
      {"float", PropertyType::FLOAT},    //
      {"float32", PropertyType::FLOAT},  //
      {"float64", PropertyType::DOUBLE}, //
      {"double", PropertyType::DOUBLE},  //
      {"list", PropertyType::LIST}};
  std::unordered_map<PropertyType, std::string> prop_type_rmap;

  std::unordered_map<std::string, PropertyName> const prop_name_map{
      {"", PropertyName::NONE},
      {"x", PropertyName::x},
      {"y", PropertyName::y},
      {"z", PropertyName::z},
      {"nx", PropertyName::nx},
      {"ny", PropertyName::ny},
      {"nz", PropertyName::nz},
      {"red", PropertyName::red},
      {"green", PropertyName::green},
      {"blue", PropertyName::blue},
      {"alpha", PropertyName::alpha},
      {"vertex_indices", PropertyName::vertex_indices},
      {"vertex_index", PropertyName::vertex_indices}};
  std::unordered_map<PropertyName, std::string> prop_name_rmap;

  std::unordered_map<PropertyType, unsigned int> const type_size_map{
      // type             bytes
      {PropertyType::CHAR, 1},   //
      {PropertyType::UCHAR, 1},  //
      {PropertyType::SHORT, 2},  //
      {PropertyType::USHORT, 2}, //
      {PropertyType::INT, 4},    //
      {PropertyType::UINT, 4},   //
      {PropertyType::FLOAT, 4},  //
      {PropertyType::DOUBLE, 8}};

  std::unordered_map<std::string, ElementType> const elem_type_map{
      {"", ElementType::NONE},           //
      {"vertice", ElementType::VERTEX},  //
      {"vertices", ElementType::VERTEX}, //
      {"vertex", ElementType::VERTEX},   //
      {"faces", ElementType::FACE},      //
      {"face", ElementType::FACE}};      //
  std::unordered_map<ElementType, std::string> elem_type_rmap;

  int parse_header(std::ifstream *file);
  int load_data(std::ifstream *file);
  int from_file(const char *fname);
  int parse_vertices_properties(std::string &line, std::ifstream *file,
                                unsigned int n_elem);
  int parse_faces_properties(std::string &line, std::ifstream *file,
                             unsigned int n_elem);

  void build_inverse_maps();
  unsigned int get_element_stride(Element &elem);
  int get_property_offset(PropertyName name, Element &elem);

  void set_elements_file_begin_position();

public:
  std::vector<Element> elements{0};
  int n_dim{3};
  int vertices_per_face{3};
  int n_vertices{0};
  int n_faces{0};
  std::string data_layout;
  std::vector<double> vertices;
  std::vector<unsigned int> faces;
  std::vector<double> vertex_normals;

  template <class OUT_TYPE>
  void get_subelement_data(std::string const element_type,
                           std::vector<PropertyName> &property_names,
                           std::vector<OUT_TYPE> &out_data);

  template <class IN_TYPE>
  void get_face_data(std::vector<unsigned int> &out_data);

  // Mesh mesh;
  PlyFile() {}
  PlyFile(const char *fname) {
    from_file(fname);
    // mesh.init(vertices, faces);
  }

  void print();
  // void build_rmap();
  //  PlyFile(Mesh &mesh) {}
};

#endif // PLYFILE_H_
