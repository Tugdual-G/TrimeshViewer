#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np

n_colors = 100
cmaps = {}
cmaps["PLASMA"] = plt.cm.plasma(np.linspace(0,1,100))[:,0:3]
cmaps["MAGMA"] = plt.cm.magma(np.linspace(0,1,100))[:,0:3]
cmaps["INFERNO"] = plt.cm.inferno(np.linspace(0,1,100))[:,0:3]
cmaps["VIRIDIS"] = plt.cm.viridis(np.linspace(0,1,100))[:,0:3]

head = """
#ifndef CMAP_H_
#define CMAP_H_
#include <algorithm>
#include <vector>

"""

func = """
void interpolated_color(double *val, double &vmin, double &inv_delta,
                        double *cmap, double *color) {
  double normalized_val = (NCOLORS - 2) * (*val - vmin) * inv_delta;
  unsigned int i0 = normalized_val;
  unsigned int i1 = (double)1 + normalized_val;
  double coef = (normalized_val - (double)i0);
  color[0] = cmap[i0 * 3] * coef + cmap[i1 * 3] * (1 - coef);
  color[1] = cmap[i0 * 3 + 1] * coef + cmap[i1 * 3 + 1] * (1 - coef);
  color[2] = cmap[i0 * 3 + 2] * coef + cmap[i1 * 3 + 2] * (1 - coef);
}

void nearest_color(double *val, double &vmin, double &inv_delta, double *cmap,
                   double *color) {
  unsigned int i = (NCOLORS - 1) * (*val - vmin) * inv_delta + 0.5;
  color[0] = cmap[i * 3];
  color[1] = cmap[i * 3 + 1];
  color[2] = cmap[i * 3 + 2];
}

void get_interpolated_colors(std::vector<double> &values,
                             std::vector<double> &colors, double *cmap) {
  auto [min, max] = std::minmax_element(values.begin(), values.end());
  double minv = *min, maxv = *max;
  double inv_delta = 1 / (maxv - minv);
  colors.resize(values.size() * 3);
  for (unsigned int i = 0; i < values.size(); ++i) {
    interpolated_color(&values[i], minv, inv_delta, cmap, &colors[i * 3]);
  }
}

void get_interpolated_colors(std::vector<double> &values,
                             std::vector<double> &colors, double *cmap,
                             double &minv, double &maxv) {
  colors.resize(values.size() * 3);
  double inv_delta = 1 / (maxv - minv);
  for (unsigned int i = 0; i < values.size(); ++i) {
    interpolated_color(&values[i], minv, inv_delta, cmap, &colors[i * 3]);
  }
}

void get_nearest_colors(std::vector<double> &values,
                        std::vector<double> &colors, double *cmap) {
  auto [min, max] = std::minmax_element(values.begin(), values.end());
  double minv = *min, maxv = *max;
  double inv_delta = 1 / (maxv - minv);
  colors.resize(values.size() * 3);
  for (unsigned int i = 0; i < values.size(); ++i) {
    nearest_color(&values[i], minv, inv_delta, cmap, &colors[i * 3]);
  }
}

void get_nearest_colors(std::vector<double> &values,
                        std::vector<double> &colors, double *cmap, double &minv,
                        double &maxv) {
  double inv_delta = 1 / (maxv - minv);
  colors.resize(values.size() * 3);
  for (unsigned int i = 0; i < values.size(); ++i) {
    nearest_color(&values[i], minv, inv_delta, cmap, &colors[i * 3]);
  }
}

#endif // CMAP_H_
"""

with open('colormap.h', mode="w") as f:
    f.write(head)
    f.write(f"#define NCOLORS {n_colors}\n")
    for key, value in cmaps.items():
        f.write(f"\ndouble {key}[] = ""{")
        for i in range(value.shape[0]-1):
            for j in range(value.shape[1]):
                f.write(f"{value[i,j]}, ")
            f.write("  //\n                   ")

        f.write(f"{value[-1,0]}, {value[-1,1]}, {value[-1,2]}""};")
        f.write("  //\n\n")
    f.write(func)
