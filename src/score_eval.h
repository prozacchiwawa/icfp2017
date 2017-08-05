#ifndef SCORE_EVAL_H
#define SCORE_EVAL_H

#include "ourgraph.h"

typedef boost::property_map < Graph, boost::vertex_index_t >::type IndexMap;
typedef boost::property_map < Graph, boost::vertex_name_t >::type NameMap;
 
typedef boost::iterator_property_map < Vertex*, IndexMap, Vertex, Vertex& > PredecessorMap;
typedef boost::iterator_property_map < Weight*, IndexMap, Weight, Weight& > DistanceMap;
typedef boost::iterator_property_map < Weight*, IndexMap, Weight, Weight& > WeightMap;
 
uint64_t score_player_map(const DumbMap &world, const DumbMap &player);

#endif //SCORE_EVAL_H
