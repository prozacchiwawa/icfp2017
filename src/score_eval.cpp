//#include <boost/config.hpp>
 
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/properties.hpp>
 
#include <boost/property_map/property_map.hpp>
 
#include <iostream>
#include <utility>
#include <vector>
 
#include "score_eval.h"


/*
template <class EdgeIter, class Graph>
b(EdgeIter mine_edge, const Graph& G) {
  
}
*/

uint64_t
score_player_map(const Graph& g, std::set<Vertex> mines) {

  /*
  std::set<Vertex>::iterator it;
  for (it = mines.begin(); it != mines.end(); ++it)
    {
  */
  Vertex v0 = *(mines.begin());
  // Create things for Dijkstra
  std::vector<Vertex> predecessors(boost::num_vertices(g)); // To store parents
  std::vector<Weight> distances(boost::num_vertices(g)); // To store distances
 
  IndexMap indexMap = boost::get(boost::vertex_index, g);
  PredecessorMap predecessorMap(&predecessors[0], indexMap);
  DistanceMap distanceMap(&distances[0], indexMap);
 
  // Compute shortest paths from v0 to all vertices, and store the output in predecessors and distances
  boost::dijkstra_shortest_paths(g, v0, boost::distance_map(distanceMap).predecessor_map(predecessorMap));
 
  // Output results
  std::cout << "distances and parents:" << std::endl;
  auto nameMap = boost::get(boost::vertex_name, g);
 
  BGL_FORALL_VERTICES(v, g, Graph)
  {
    std::cout << "distance(" << nameMap[v0] << ", " << nameMap[v] << ") = " << distanceMap[v] << ", ";
    std::cout << "predecessor(" << nameMap[v] << ") = " << nameMap[predecessorMap[v]] << std::endl;
  }
 

}
 
