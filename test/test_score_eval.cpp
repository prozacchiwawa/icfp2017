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

#include "ourgraph.h"
#include "score_eval.h"
 
int main(int, char *[])
{
  // Create game and player graph
  DumbMap world;
  std::istringstream world_iss("0 1 2 3 end 0 1 1 3 0 2 2 3 end 0 end");

  world_iss >> world;

  DumbMap player;
  std::istringstream player_iss("0 1 2 3 end 1 3 0 2 2 3 end 0 end");
  
  player_iss >> player;

  /* At this point the game graph is
        v0
         .
        / \ 
       /   \
      /     . v2
     /       \
    /         \ 
   /           \
  v1----------- v3

  */

  /* And the player graph is:
        v0
         .
          \ 
           \
            . v2
             \
              \ 
               \
  v1----------- v3
    
  */

  uint64_t score = score_player_map(world, player);
  std::cout << "Score: " << score << std::endl;

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
