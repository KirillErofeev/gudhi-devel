/*    This file is part of the Gudhi Library. The Gudhi library
 *    (Geometric Understanding in Higher Dimensions) is a generic C++
 *    library for computational topology.
 *
 *    Author(s):       Francois Godi
 *
 *    Copyright (C) 2015  INRIA Sophia-Antipolis (France)
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

#ifndef SRC_BOTTLENECK_INCLUDE_GUDHI_PLANAR_NEIGHBORS_FINDER_H_
#define SRC_BOTTLENECK_INCLUDE_GUDHI_PLANAR_NEIGHBORS_FINDER_H_

#include <list>
#include <map>

#include "Persistence_diagrams_graph.h"
#include "Grid_cell.h"

namespace Gudhi {

namespace bipartite_graph_matching {

/** \internal \brief Structure used to find any point in V near (according to the planar distance) to a query point from U.
 *
 * V points have to be added manually using their index and before the first remove/pull. A neighbor pulled is automatically removed. but we can also
 * remove points manually using their index.
 *
 * \ingroup bottleneck_distance
 */
class Abstract_planar_neighbors_finder {
public:
    /** \internal \brief Constructor TODO. */
    Abstract_planar_neighbors_finder(double r);
    virtual ~Abstract_planar_neighbors_finder() = 0;
    /** \internal \brief A point added will be possibly pulled. */
    virtual void add(int v_point_index) = 0;
    /** \internal \brief A point manually removed will no longer be possibly pulled. */
    virtual void remove(int v_point_index) = 0;
    /** \internal \brief Can the point given as parameter be returned ? */
    virtual bool contains(int v_point_index) const = 0;
    /** \internal \brief Provide and remove a V point near to the U point given as parameter, null_point_index() if there isn't such a point. */
    virtual int pull_near(int u_point_index) = 0;
    /** \internal \brief Provide and remove all the V points near to the U point given as parameter. */
    virtual std::unique_ptr< std::list<int> > pull_all_near(int u_point_index);

protected:
    const double r;
};

/** \internal \brief Naive_pnf is an naïve Abstract_planar_neighbors_finder implementation
 *
 * \ingroup bottleneck_distance
 */
class Naive_pnf : public Abstract_planar_neighbors_finder {
public:
    /** \internal \brief Constructor taking the near distance definition as parameter. */
    Naive_pnf(double r);
    /** \internal \brief A point added will be possibly pulled. */
    void add(int v_point_index);
    /** \internal \brief A point manually removed will no longer be possibly pulled. */
    void remove(int v_point_index);
    /** \internal \brief Can the point given as parameter be returned ? */
    bool contains(int v_point_index) const;
    /** \internal \brief Provide and remove a V point near to the U point given as parameter, null_point_index() if there isn't such a point. */
    int pull_near(int u_point_index);

private:
    std::pair<int,int> get_v_key(int v_point_index) const;
    std::multimap<std::pair<int,int>,int> grid;
};

/** \internal \typedef \brief Planar_neighbors_finder is the used Abstract_planar_neighbors_finder's implementation. */
typedef Naive_pnf Planar_neighbors_finder;


inline Abstract_planar_neighbors_finder::Abstract_planar_neighbors_finder(double r) :
    r(r) { }

inline Abstract_planar_neighbors_finder::~Abstract_planar_neighbors_finder() {}

inline std::unique_ptr< std::list<int> > Abstract_planar_neighbors_finder::pull_all_near(int u_point_index) {
    std::unique_ptr< std::list<int> > all_pull(new std::list<int>);
    int last_pull = pull_near(u_point_index);
    while (last_pull != null_point_index()) {
        all_pull->emplace_back(last_pull);
        last_pull = pull_near(u_point_index);
    }
    return all_pull;
}

inline Naive_pnf::Naive_pnf(double r) :
    Abstract_planar_neighbors_finder(r), grid() { }


inline std::pair<int,int> Naive_pnf::get_v_key(int v_point_index) const{
    G::Internal_point v_point = G::get_v_point(v_point_index);
    return std::make_pair(static_cast<int>(v_point.first/r), static_cast<int>(v_point.second/r));
}

inline void Naive_pnf::add(int v_point_index) {
    grid.emplace(get_v_key(v_point_index),v_point_index);
}

inline void Naive_pnf::remove(int v_point_index) {
    for(auto it = grid.find(get_v_key(v_point_index)); it!=grid.end(); it++)
        if(it->second==v_point_index){
            grid.erase(it);
            return;
        }
}

inline bool Naive_pnf::contains(int v_point_index) const {
    if(v_point_index == null_point_index())
        return false;
    for(auto it = grid.find(get_v_key(v_point_index)); it!=grid.end(); it++)
        if(it->second==v_point_index)
            return true;
    return false;
}

inline int Naive_pnf::pull_near(int u_point_index) {
    G::Internal_point u_point = G::get_u_point(u_point_index);
    int i0 = static_cast<int>(u_point.first/r);
    int j0 = static_cast<int>(u_point.second/r);
    for(int i = i0 -1; i<= i0 + 1; i++)
        for(int j = j0 -1; j<= j0 + 1; j++)
            for(auto it = grid.find(std::make_pair(i,j)); it!=grid.end(); it++)
                if (G::distance(u_point_index, it->second) <= r) {
                    int tmp = it->second;
                    grid.erase(it);
                    return tmp;
                }
    return null_point_index();
}

}  // namespace bipartite_graph_matching

}  // namespace Gudhi

#endif  // SRC_BOTTLENECK_INCLUDE_GUDHI_PLANAR_NEIGHBORS_FINDER_H_
