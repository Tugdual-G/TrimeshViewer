#include "plymesh.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>

void PlyMesh::set_mean_curvature() {

  mean_curvature.resize(n_vertices * 3, 0);
  unsigned int or_i0 = 0;   // the first element for each vertex
  unsigned int ring_nv = 0; // number of vertex per ring

  std::vector<double> ring_vert( // ring vertices spacial coords
      n_adja_faces_max * 3);

  std::vector<double> edges_vect(n_adja_faces_max * 3);

  unsigned int r_vert_idx; // points to the current vertice idx

  // for i in range(n_vertices):
  for (unsigned int i = 0; i < n_vertices; ++i) {
    ring_nv = one_ring[or_i0]; // retrieves the number of vertices in the ring
    if (ring_nv > 1) {
      // copy each ring vert coord into ring_vert.
      for (unsigned int r_i = 0; r_i < ring_nv; ++i) {
        r_vert_idx = one_ring.at(or_i0 + r_i + 1);
        std::copy(vertices.begin() + r_vert_idx * 3,
                  vertices.begin() + (r_vert_idx + 1) * 3,
                  ring_vert.begin() + r_i * 3);
      }
      // edges connecting the point to itś neighbours

      for (unsigned int k = 0; k < 3 : ++k) {
        edges_vect.at(k) = ring_vert - vertices [i, :]
      }

      // area of the ring
          A = 0
          for j in range(ring_nv - 1):{
            A += norm(np.cross(edges_vect[j], edges_vect[(j + 1) % ring_nv]))
          }
          for
            j in range(ring_nv)
                : {e1 = edges_vect[(j - 1) % ring_nv].copy() o1 =
                       ring_vert[j] - ring_vert[(j - 1) % ring_nv] cos1 =
                           np.dot(e1, o1)
                               sin1 = np.linalg.norm(np.cross(o1, e1)) cot1 =
                               cos1 / sin1

                                          e2 =
                                   edges_vect[(j + 1) % ring_nv].copy() o2 =
                                       ring_vert[j] -
                                       ring_vert[(j + 1) % ring_nv] cos2 =
                                           np.dot(e2, o2) sin2 = np.linalg.norm(
                                               np.cross(e2, o2)) cot2 =
                                               cos2 / sin2

                                                          mean_curvature[i] -=
                   edges_vect[j] * (cot2 + cot1)} mean_curvature[i] /= 2 * A
    }
    or_i0 += ring_nv + 1;
  }
  return kN
}
