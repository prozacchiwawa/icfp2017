#ifndef SCORE_EVAL_H
#define SCORE_EVAL_H

#include "ourgraph.h"

typedef boost::property_map < Graph, boost::vertex_index_t >::type IndexMap;
typedef boost::property_map < Graph, boost::vertex_name_t >::type NameMap;
 
typedef boost::iterator_property_map < Vertex*, IndexMap, Vertex, Vertex& > PredecessorMap;
typedef boost::iterator_property_map < Weight*, IndexMap, Weight, Weight& > DistanceMap;
typedef boost::iterator_property_map < Weight*, IndexMap, Weight, Weight& > WeightMap;
 
uint64_t score_one_mine(const SiteID &mine, const std::set<SiteID> &player_vertices, const std::map<std::string, std::vector<uint32_t> > &weights, const Graph &player, const DumbMap &world);
uint64_t score_player_map(PID punter, const std::map<std::string, std::vector<uint32_t> > &weights, const std::set<SiteID> &player_mines, const Graph &player_graph, const DumbMap& d);
uint64_t score_player_map(PID player, const std::map<std::string, std::vector<uint32_t> > &weights, const DumbMap &world);

#endif //SCORE_EVAL_H
