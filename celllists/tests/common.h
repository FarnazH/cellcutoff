// CellList is a 3D domain decomposition library.
// Copyright (C) 2011-2015 The CellList Development Team
//
// This file is part of CellList.
//
// CellList is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// CellList is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// aint with this program; if not, see <http://www.gnu.org/licenses/>
//
//--


#ifndef CELLLISTS_TESTS_COMMON_H_
#define CELLLISTS_TESTS_COMMON_H_

#include <memory>

#include "celllists/cell.h"


namespace cl = celllists;


#define NREP 100
#define NPOINT 1000


/* Some notes:

    - Usually, the output argument is last. The exception in this module is the size
      of the output argument, which comes after the actual output argument.

*/


//! Fills an array of doubles with random numbers in range ]-0.5*scale, 0.5*scale]
unsigned int fill_random_double(const unsigned int seed, double* array, const int size,
    const double low = -0.5, const double high = 0.5);

//! Fills an array of int with random numbers in range [-range, range]
unsigned int fill_random_int(const unsigned int seed, int* array, const int size,
    const int begin, const int end);

//! Fills and array of int with a random permutation
unsigned int fill_random_permutation(const unsigned int seed, int* array, const int size);

//! Random cell with a volume larger than (0.1*scale)**nvec
std::unique_ptr<cl::Cell> create_random_cell_nvec(const unsigned int seed, const int nvec,
    const double scale = 1.0, const bool cuboid = false);

//! Compute a random point in a cubic box centered around center. Also computes distance.
unsigned int random_point(const unsigned int seed,  const double* center,
    const double rcut, double* point, double* norm);


#endif  // CELLLISTS_TESTS_COMMON_H_

// vim: textwidth=90 et ts=2 sw=2
