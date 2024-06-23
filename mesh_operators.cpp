#include "mesh.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>

// static void disp_vect(double *v, int n) {
//   for (int i = 0; i < n; ++i) {
//     std::cout << v[i] << " ";
//   }
//   std::cout << std::endl;
// }

// static void normalize(double *w) {
//   double norm = pow(pow(w[0], 2.0) + pow(w[1], 2.0) + pow(w[2], 2.0), 0.5);
//   w[0] /= norm;
//   w[1] /= norm;
//   w[2] /= norm;
// }

static double norm(double *w) {
  return pow(pow(w[0], 2.0) + pow(w[1], 2.0) + pow(w[2], 2.0), 0.5);
}

static void vector_prod(double *u, double *v, double *w) {
  w[0] = u[1] * v[2] - u[2] * v[1];
  w[1] = u[2] * v[0] - u[0] * v[2];
  w[2] = u[0] * v[1] - u[1] * v[0];
}

static double dot(double *u, double *v) {
  return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

void Mesh::scalar_mean_curvature(std::vector<double> &k) {
  if (mean_curvature.size() < (long unsigned int)n_vertices) {
    set_mean_curvature();
  }
  if (vertex_normals.size() < (long unsigned int)n_vertices) {
    set_vertex_normals();
  }
  k.resize(n_vertices);
  for (unsigned int i = 0; i < (unsigned int)n_vertices; ++i) {
    k[i] =
        dot(mean_curvature.data() + (3 * i), vertex_normals.data() + (3 * i));
  }
}

void Mesh::set_mean_curvature() {
  if (one_ring.size() < (unsigned int)n_vertices) {
    set_one_ring();
  }

  mean_curvature.resize(n_vertices * 3, 0);
  unsigned int one_ring_i0 = 0; // the first element for each vertex
  unsigned int ring_nv = 0;     // number of vertex per ring

  std::vector<double> ring_vert( // ring vertices spacial coords
      n_adja_faces_max * 3, -99999999);

  std::vector<double> edges_vect(n_adja_faces_max * 3);

  unsigned int r_vert_idx; // points to the current vertice idx

  double A_x2 = 0; // area
  double cross[3]; // to store temporary cross product
  double *e1, o1[3];
  double cos1, cos2, sin1, sin2, cot1, cot2;
  for (unsigned int i = 0; i < (unsigned int)n_vertices; ++i) {
    ring_nv =
        one_ring[one_ring_i0]; // retrieves the number of vertices in the ring
    if (ring_nv > 1) {
      // copy each ring vert coord into ring_vert.
      for (unsigned int r_i = 0; r_i < ring_nv; ++r_i) {
        r_vert_idx = one_ring.at(one_ring_i0 + r_i + 1);

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
}
