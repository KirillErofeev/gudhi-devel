/*    This file is part of the Gudhi Library. The Gudhi library
 *    (Geometric Understanding in Higher Dimensions) is a generic C++
 *    library for computational topology.
 *
 *    Author(s):       Pawel Dlotko
 *
 *    Copyright (C) 2016 Inria
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef READ_PERSISTENCE_FROM_FILE_H_
#define READ_PERSISTENCE_FROM_FILE_H_

#include <gudhi/reader_utils.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <utility>
#include <limits>  // for std::numeric_limits<>

namespace Gudhi {
namespace Persistence_representations {

/**
 * Universal procedure to read files with persistence. It ignores the lines starting from # (treat them as comments).
 * It reads the fist line which is not a comment and assume that there are some numerical entries over there. The
 *program assume
 * that each other line in the file, which is not a comment, have the same number of numerical entries (2, 3 or 4).
 * If there are two numerical entries per line, then the function assume that they are birth/death coordinates.
 * If there are three numerical entries per line, then the function assume that they are: dimension and birth/death
 *coordinates.
 * If there are four numerical entries per line, then the function assume that they are: the characteristic of a filed
 *over which
 * persistence was computed, dimension and birth/death coordinates.
 * The 'inf' string can appear only as a last element of a line.
 * The procedure returns vector of persistence pairs.
**/
std::vector<std::pair<double, double> > read_persistence_intervals_in_one_dimension_from_file(
    std::string const& filename, int dimension = -1, double what_to_substitute_for_infinite_bar = -1) {
  bool dbg = false;

  std::string line;
  std::vector<std::pair<double, double> > barcode_initial =
      read_persistence_intervals_in_dimension(filename, static_cast<int>(dimension));
  std::vector<std::pair<double, double> > final_barcode;
  final_barcode.reserve(barcode_initial.size());

  if (dbg) {
    std::cerr << "Here are the intervals that we read from the file : \n";
    for (size_t i = 0; i != barcode_initial.size(); ++i) {
      std::cout << barcode_initial[i].first << " " << barcode_initial[i].second << std::endl;
    }
    getchar();
  }

  for (size_t i = 0; i != barcode_initial.size(); ++i) {
    if (dbg) {
      std::cout << "COnsidering interval : " << barcode_initial[i].first << " " << barcode_initial[i].second
                << std::endl;
    }

    if (barcode_initial[i].first > barcode_initial[i].second) {
      // note that in this case barcode_initial[i].second != std::numeric_limits<double>::infinity()
      if (dbg) std::cout << "Swap and enter \n";
      // swap them to make sure that birth < death
      final_barcode.push_back(std::pair<double, double>(barcode_initial[i].second, barcode_initial[i].first));
      continue;
    } else {
      if (barcode_initial[i].second != std::numeric_limits<double>::infinity()) {
        if (dbg) std::cout << "Simply enters\n";
        // in this case, due to the previous conditions we know that barcode_initial[i].first <
        // barcode_initial[i].second, so we put them as they are
        final_barcode.push_back(std::pair<double, double>(barcode_initial[i].first, barcode_initial[i].second));
      }
    }

    if ((barcode_initial[i].second == std::numeric_limits<double>::infinity()) &&
        (what_to_substitute_for_infinite_bar != -1)) {
      if (barcode_initial[i].first < what_to_substitute_for_infinite_bar) {
        // if only birth < death.
        final_barcode.push_back(
            std::pair<double, double>(barcode_initial[i].first, what_to_substitute_for_infinite_bar));
      }
    } else {
      // if the variable what_to_substitute_for_infinite_bar is not set, then we ignore all the infinite bars.
    }
  }

  if (dbg) {
    std::cerr << "Here are the final bars that we are sending further : \n";
    for (size_t i = 0; i != final_barcode.size(); ++i) {
      std::cout << final_barcode[i].first << " " << final_barcode[i].second << std::endl;
    }
    std::cerr << "final_barcode.size() : " << final_barcode.size() << std::endl;
    getchar();
  }

  return final_barcode;
}  // read_persistence_intervals_in_one_dimension_from_file

}  // namespace Persistence_representations
}  // namespace Gudhi

#endif  // READ_PERSISTENCE_FROM_FILE_H_
