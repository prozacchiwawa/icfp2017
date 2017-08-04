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
 
int main(int, char *[])
{
#if 0 
  // Create a graph
  Graph g;
 
  // Add named vertices
  Vertex v0 = boost::add_vertex(std::string("v0"), g);
  Vertex v1 = boost::add_vertex(std::string("v1"), g);
  Vertex v2 = boost::add_vertex(std::string("v2"), g);
  Vertex v3 = boost::add_vertex(std::string("v3"), g);
 
  // Add weighted edges
  Weight weight0 = 5;
  Weight weight1 = 3;
  Weight weight2 = 2;
  Weight weight3 = 4;
 
  boost::add_edge(v0, v1, weight0, g);
  boost::add_edge(v1, v3, weight1, g);
  boost::add_edge(v0, v2, weight2, g);
  boost::add_edge(v2, v3, weight3, g);
 
  // At this point the graph is
  /*    v0
         .
        / \ 2
       /   \
      /     . v2
    5/       \
    /         \ 4
   /           \
  v1----------- v3
      3
  */

  uint64_t score = score_player_map(g, std::set(v0));
  std::cout << "Score: " << score << std::endl;
#endif   
  return EXIT_SUCCESS;
}
 
/*
 * Output:
distances and parents:
distance(v0, v0) = 0, predecessor(v0) = v0
distance(v0, v1) = 5, predecessor(v1) = v0
distance(v0, v2) = 2, predecessor(v2) = v0
distance(v0, v3) = 6, predecessor(v3) = v2
 
Score: 
 
*/
