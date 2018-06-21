#ifndef _TRANSIT_TRANSIT_H_
#define _TRANSIT_TRANSIT_H_

#include <cmath>

#include "transit/utils.h"

namespace transit {

  template <typename T>
  TRANSIT_CUDA_CALLABLE
  inline T index_to_coord (int size, int index)
  {
    T val = 1.0 - index / (size - 1);
    return 1.0 - val * val;
  }

  template <typename T>
  TRANSIT_CUDA_CALLABLE
  inline T coord_to_index (int size, T coord)
  {
    return (size - 1) * (1.0 + sqrt(1.0 - coord));
  }

  template <typename T>
  TRANSIT_CUDA_CALLABLE
  inline T compute_area (T x, T z, T r)
  {
    T rmz = r - z,
      rpz = r + z,
      x2 = x*x,
      r2 = r*r;

    if (fabs(x - r) < z < x + r) {
      T z2 = z*z;
      T u = 0.5 * (z2 + x2 - r2) / ((z) * (x));
      T v = 0.5 * (z2 + r2 - x2) / ((z) * (r));
      T w = (x + rmz) * (x - rmz) * (rpz - x) * (rpz + x);
      if (w < 0.0) w = 0.0;
      return x2 * acos(u) + r2 * acos(v) - 0.5 * sqrt(w);
    } else if (x >= rpz) {
      return M_PI * r2;
    } else if (x <= rmz) {
      return M_PI * x2;
    }
    return 0.0;
  }

  template <typename T>
  TRANSIT_CUDA_CALLABLE
  inline T compute_delta (
      int                    grid_size,
      T*  __restrict__ const grid,
      T                      z,
      T                      r)
  {
    int indmin = max(0,           int(floor(coord_to_index(grid_size, z - r))));
    int indmax = min(grid_size-1, int( ceil(coord_to_index(grid_size, z + r))));
    T delta = 0.0;
    T A1 = 0.0;
    T I1 = grid[indmin];
    T x2, A2, I2;
    for (int ind = indmin+1; ind <= indmax; ++ind) {
      x2 = index_to_coord<T>(grid_size, ind);
      A2 = compute_area<T>(x2, z, r);
      I2 = grid[ind];
      delta += 0.5 * (I1 + I2) * (A2 - A1);
      A1 = A2;
      I1 = I2;
    }
    return delta;
  }

};

#endif
