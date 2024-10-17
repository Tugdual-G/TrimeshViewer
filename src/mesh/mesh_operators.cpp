#include "mesh.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <span>
#include <vector>

static auto norm(const double *w) -> double {
  return pow(w[0] * w[0] + w[1] * w[1] + w[2] * w[2], 0.5);
}

static void cross(const double *u, const double *v, double *w) {
  w[0] = u[1] * v[2] - u[2] * v[1];
  w[1] = u[2] * v[0] - u[0] * v[2];
  w[2] = u[0] * v[1] - u[1] * v[0];
}

static auto dot(const double *u, const double *v) -> double {
  return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

void get_ring_vertices(int ring_nv,
                       std::vector<unsigned int>::iterator one_ring_iter,
                       std::vector<double> &vertices,
                       std::vector<double> &ring_vertices);

auto Mesh::get_scalar_mean_curvature(std::vector<double> &mean_curvature)
    -> std::vector<double> {
  if (mean_curvature.size() != (long unsigned int)n_vertices * 3LU) {
    std::cout << "Error, mean_curvature should be of size 3*n_vertices\n";
    exit(1);
  }

  if (vertex_normals.size() != (long unsigned int)n_vertices * 3LU) {
    set_vertex_normals();
  }
  std::vector<double> k(n_vertices);

  for (unsigned int i = 0; i < (unsigned int)n_vertices; ++i) {
    k[i] =
        dot(mean_curvature.data() + (3 * i), vertex_normals.data() + (3 * i));
  }
  return k;
}

// Takes one_ring as an argument to make explicit that the
// method depends on the one-ring.
auto Mesh::get_mean_curvature(std::vector<unsigned int> &vertices_one_ring)
    -> std::vector<double> {

  if (vertices_one_ring.size() < 2) {
    std::cout << "Error, vertices_one_ring should be initialized \n";
    exit(1);
  }

  std::vector<double> mean_curvature(3 * n_vertices);
  int one_ring_i0{0}; // the first element for each vertex
  int ring_nv{0};     // number of vertex per ring

  std::vector<double> ring_vertices( // ring vertices spacial coords
      n_adja_faces_max * 3, -999999);

  std::vector<double> edges_vect(n_adja_faces_max * 3);

  double area_x2{0};                   // area of the dual mesh
  std::vector<double> cross_tmp(3, 0); // to store temporary cross product
  double norm_tmp{0};
  double *e1{nullptr};
  std::vector<double> o1(3, 0); // opposite edge
  double cot1{0};               // cotangent
  double cot2{0};

  for (int i = 0; i < n_vertices; ++i) {
    ring_nv = (int)vertices_one_ring[one_ring_i0]; // retrieves the number of
                                                   // vertices in the ring
    if (ring_nv > 2) {
      // copy each ring vert coord into ring_vertices.
      get_ring_vertices(ring_nv, vertices_one_ring.begin() + (one_ring_i0 + 1),
                        vertices, ring_vertices);

      // edges connecting the point to its neighbours
      for (int j = 0; j < ring_nv; ++j) {
        for (int k = 0; k < 3; ++k) {
          edges_vect.at(j * 3 + k) =
              ring_vertices.at(j * 3 + k) - vertices.at(i * 3 + k);
        }
      }

      area_x2 = 0;
      for (int j = 0; j < ring_nv; ++j) {
        e1 = &edges_vect[((ring_nv + j - 1) % ring_nv) * 3];

        for (int k = 0; k < 3; ++k) {
          o1[k] = ring_vertices.at(j * 3 + k) -
                  ring_vertices.at(((ring_nv + j - 1) % ring_nv) * 3 + k);
        }

        cross(o1.data(), e1, cross_tmp.data());
        norm_tmp = norm(cross_tmp.data());
        cot1 = dot(e1, o1.data()) / norm_tmp;
        // std::cout << norm_tmp << "\n";
        area_x2 += norm_tmp;

        e1 = &edges_vect[((ring_nv + j + 1) % ring_nv) * 3];

        for (int k = 0; k < 3; ++k) {
          o1[k] = ring_vertices.at(j * 3 + k) -
                  ring_vertices.at(((j + 1) % ring_nv) * 3 + k);
        }

        cross(e1, o1.data(), cross_tmp.data());
        norm_tmp = norm(cross_tmp.data());
        cot2 = dot(e1, o1.data()) / norm_tmp;

        for (int k = 0; k < 3; ++k) {
          mean_curvature.at(i * 3 + k) -=
              edges_vect.at(j * 3 + k) * (cot2 + cot1);
        }
      }

      area_x2 = 3 / (2 * area_x2);
      for (int k = 0; k < 3; ++k) {
        mean_curvature.at(i * 3 + k) *= area_x2;
      }
    }
    // disp_vect(mean_curvature.data() + i * 3, 3);
    one_ring_i0 += ring_nv + 1;
  }
  return mean_curvature;
}

void get_ring_vertices(int ring_nv,
                       std::vector<unsigned int>::iterator one_ring_iter,
                       std::vector<double> &vertices,
                       std::vector<double> &ring_vertices) {
  /* Get the ring vertices coordinates. */
  int vert_idx{0};
  for (int j = 0; j < ring_nv; ++j, ++one_ring_iter) {
    vert_idx = (int)*one_ring_iter;
    std::copy(vertices.begin() + (vert_idx * 3),
              vertices.begin() + ((vert_idx + 1) * 3),
              ring_vertices.begin() + (j * 3));
  }
}

auto Mesh::get_face_areas() -> std::vector<double> {
  std::vector<double> area(n_faces, 0.0);

  double cross_tmp[3]; // to store temporary cross_tmp product
  double e0[] = {0, 0, 0};
  double e1[] = {0, 0, 0};

  for (int i = 0; i < n_faces; ++i) {
    for (int j = 0; j < 3; ++j) {
      e0[j] = vertices.at(3 * faces.at(i * 3 + 1) + j) -
              vertices.at(3 * faces.at(i * 3) + j);

      e1[j] = vertices.at(3 * faces.at(i * 3 + 2) + j) -
              vertices.at(3 * faces.at(i * 3) + j);
    }
    cross(e0, e1, cross_tmp);
    area.at(i) = norm(cross_tmp);
    area.at(i) *= 0.5;
  }

  return area;
}
