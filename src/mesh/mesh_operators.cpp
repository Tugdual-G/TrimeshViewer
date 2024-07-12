#include "mesh.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

static auto norm(double *w) -> double {
  return pow(pow(w[0], 2.0) + pow(w[1], 2.0) + pow(w[2], 2.0), 0.5);
}

static void vector_prod(const double *u, const double *v, double *w) {
  w[0] = u[1] * v[2] - u[2] * v[1];
  w[1] = u[2] * v[0] - u[0] * v[2];
  w[2] = u[0] * v[1] - u[1] * v[0];
}

static auto dot(const double *u, const double *v) -> double {
  return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

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
  unsigned int one_ring_i0{0}; // the first element for each vertex
  unsigned int ring_nv{0};     // number of vertex per ring

  std::vector<double> ring_vert( // ring vertices spacial coords
      n_adja_faces_max * 3, -999999);

  std::vector<double> edges_vect(n_adja_faces_max * 3);

  unsigned int r_vert_idx = 0; // points to the current vertice idx

  double A_x2 = 0; // area
  double cross[3]; // to store temporary cross product
  double *e1;
  double o1[3];
  double cos1;
  double cos2;
  double sin1;
  double sin2;
  double cot1;
  double cot2;
  for (unsigned int i = 0; i < (unsigned int)n_vertices; ++i) {
    ring_nv = vertices_one_ring[one_ring_i0]; // retrieves the number of
                                              // vertices in the ring
    if (ring_nv > 1) {
      // copy each ring vert coord into ring_vert.
      for (unsigned int r_i = 0; r_i < ring_nv; ++r_i) {
        r_vert_idx = vertices_one_ring.at(one_ring_i0 + r_i + 1);

        std::copy(vertices.begin() + r_vert_idx * 3,
                  vertices.begin() + (r_vert_idx + 1) * 3,
                  ring_vert.begin() + r_i * 3);
      }

      // edges connecting the point to itś neighbours
      for (unsigned int r_i = 0; r_i < ring_nv; ++r_i) {
        for (unsigned int k = 0; k < 3; ++k) {
          edges_vect.at(r_i * 3 + k) =
              ring_vert.at(r_i * 3 + k) - vertices.at(i * 3 + k);
        }
      }

      A_x2 = 0;
      for (unsigned k = 0; k < ring_nv - 1; ++k) {
        vector_prod(edges_vect.data() + k * 3, edges_vect.data() + (k + 1) * 3,
                    cross);
        A_x2 += norm(cross);
      }
      A_x2 = 1 / (2 * A_x2);

      for (unsigned int j = 0; j < ring_nv; ++j) {
        e1 = edges_vect.data() + ((ring_nv + j - 1) % ring_nv) * 3;
        // this is horrendous
        for (unsigned int k = 0; k < 3; ++k) {
          o1[k] = ring_vert.at(j * 3 + k) -
                  ring_vert.at(((ring_nv + j - 1) % ring_nv) * 3 + k);
        }
        cos1 = dot(e1, o1);
        vector_prod(o1, e1, cross);
        sin1 = norm(cross);
        cot1 = cos1 / sin1;

        e1 = edges_vect.data() + ((ring_nv + j + 1) % ring_nv) * 3;
        // this is horrendous
        for (unsigned int k = 0; k < 3; ++k) {
          o1[k] = ring_vert.at(j * 3 + k) -
                  ring_vert.at(((j + 1) % ring_nv) * 3 + k);
        }
        // help !
        cos2 = dot(e1, o1);
        vector_prod(e1, o1, cross);

        sin2 = norm(cross);
        cot2 = cos2 / sin2;
        // std::cout << "sin1, sin2 : " << sin1 << " , " << sin2 << std::endl;

        for (unsigned int k = 0; k < 3; ++k) {
          mean_curvature.at(i * 3 + k) -=
              edges_vect.at(j * 3 + k) * (cot2 + cot1);
        }
      }

      for (unsigned int k = 0; k < 3; ++k) {
        mean_curvature.at(i * 3 + k) *= A_x2;
      }
    }
    // disp_vect(mean_curvature.data() + i * 3, 3);
    one_ring_i0 += ring_nv + 1;
  }
  return mean_curvature;
}
