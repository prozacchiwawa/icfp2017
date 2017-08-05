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
#include "ourgraph.h"

// This builds a map from mine -> set of reachable verticies, for a specific player
template < typename ReachMap > class bfs_reachable_visitor:
  public boost::default_bfs_visitor {
  //  typedef typename boost::property_traits < ReachMap >::value_type T;
public:
  
  bfs_reachable_visitor(SiteID mine_id, const DumbMap &ourmap, ReachMap rmap)
    : m_mine(mine_id), m_ourmap(ourmap), m_reachmap(rmap) { }

  template < typename Vertex, typename Graph >
  void discover_vertex(Vertex u, const Graph & g) {
    auto vtx_name_it = m_ourmap.vertices_by_number.find(u);
    if (vtx_name_it == m_ourmap.vertices_by_number.end()) {
      throw std::exception();
    }

    m_reachmap[m_mine].insert(vtx_name_it->second);
  }

  SiteID m_mine; // SiteID is a node ID. A "mine" is a type of node
  const DumbMap &m_ourmap;
  ReachMap m_reachmap;
};

// Note: this will be slow as hell if we don't memoize this
std::map<SiteID,std::set<SiteID> > player_mine_connected_paths(const DumbMap& pg) {
  const Graph& player_graph = pg.world;
  std::vector<std::pair<SiteID, SiteID> > paths;
  std::map<SiteID,std::set<SiteID> > reach_map;
  
  for (auto it = pg.mines.begin(); it != pg.mines.end(); ++it) {
    auto v0_it = pg.vertices_by_name.find(*it);
    if (v0_it == pg.vertices_by_name.end()) {
      throw std::exception();
    }
    auto v0 = *v0_it;
    
    bfs_reachable_visitor <std::map<SiteID,std::set<SiteID> > > vis(*it, pg, reach_map);
    //breadth_first_search(player_graph, vertex(*it, player_graph), visitor(vis));
    //breadth_first_search(player_graph.graph(), player_graph[*it], visitor(vis));
    breadth_first_search(player_graph, v0.second, visitor(vis));
  }
  
  return reach_map;
}


uint64_t score_player_map(PID punter, const DumbMap& d) {
    uint64_t score = 0;

    std::set<SiteID>::iterator it;
    auto &mines = d.player_mines[punter];
    auto &player_vertices = d.player_vertices[punter];
    auto &pg = d.played[punter];

    for (it = mines.begin(); it != mines.end(); ++it) {
  
        auto &g = d.world;
        auto &player_graph = pg;
        auto v0_it = d.vertices_by_name.find(*it);
        if (v0_it == d.vertices_by_name.end()) {
            throw std::exception();
        }
        auto v0 = *v0_it;
      
        std::vector<int> distances;
        distances.assign
            (d.vertices_by_name.size(), std::numeric_limits<int>::max());

        // XXX Never change, we can cache these.
        std::vector<int> world_distances;
        world_distances.assign
            (d.vertices_by_name.size(), std::numeric_limits<int>::max());
        
        // State for Dijkstra
        // Compute shortest paths from each mine to all othervertices, 
        // and store the output in predecessors and distances
        boost::dijkstra_shortest_paths
            (pg, v0.second, boost::distance_map(&distances[0]));

        boost::dijkstra_shortest_paths
            (pg, v0.second, boost::distance_map(&world_distances[0]));
      
        auto v0_name = v0.first;
      
        // Output results
        std::cout << "distances and parents for mine " << v0_name << std::endl;
      
        for (auto &it : player_vertices) {
            auto v1_it = d.vertices_by_name.find(it);
            if (v1_it == d.vertices_by_name.end()) {
                throw std::exception();
            }
          
            auto &v1_name = v1_it->first;
            auto v = v1_it->second;

            if (v1_name != v0_name &&
                distances[v] != std::numeric_limits<int>::max()) {
                score += world_distances[v] * world_distances[v];
            }
        }
    }
    return score;
}
 
