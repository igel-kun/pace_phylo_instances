/* ----------------------------------------------------------------- */

#pragma once

#include <vector>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>


#include "utils/exceptions.hpp"

namespace io {


  // read a graph from the instream "in", return the number of vertices & edges read
  template<typename Graph, typename Vertex = typename Graph::Vertex, typename Edge = typename Graph::Edge>
  bool read_edgelist(std::istream& in, Graph& g){
    DEBUG1(std::cout << "reading edgelist"<<std::endl);
    boost::unordered_map<unsigned, Vertex> idx_to_vertex;
    unsigned line_no = 0;
    std::string in_line;

    while(std::getline(in, in_line)){
      ++line_no;
      if(in_line[0] != '#'){// skip comments
        unsigned u_idx, v_idx;
        if(std::sscanf(in_line.c_str(), "%u %u", &u_idx, &v_idx) == 2){
          if(u_idx != v_idx){ // no self-loops please
            auto u_iter = idx_to_vertex.find(u_idx);
            if(u_iter == idx_to_vertex.end()) u_iter = idx_to_vertex.emplace_hint(u_iter, u_idx, g.add_vertex());
            auto v_iter = idx_to_vertex.find(v_idx);
            if(v_iter == idx_to_vertex.end()) v_iter = idx_to_vertex.emplace_hint(v_iter, v_idx, g.add_vertex());
            g.add_edge(u_iter->second, v_iter->second);
          }
        } else {
          std::cout << "unexpected input on line "<<line_no<<" - missing vertex?"<<std::endl;
          return false;
        }
      }
    }
    return true;
  }

  template<typename Graph, typename Vertex = typename Graph::Vertex, typename Edge = typename Graph::Edge>
  Graph* read_edgelist(std::istream& in){
    Graph* g = new Graph();
    if(!read_edgelist(in, g)){
      delete g;
      return NULL;
    } else return g;
  } // function


  // write the graph g to the outstream "out"
  template<typename Graph, typename Vertex = typename Graph::Vertex, typename Edge = typename Graph::Edge>
  void write_edgelist(std::ostream& out, const Graph& g){
    for(auto ur = g.vertices(); ur.first != ur.second; ++ur.first){
      const Vertex& u = *ur.first;
      const size_t u_idx = g.get_index(u);
      for(auto vr = g.adjacent_vertices(u); vr.first != vr.second; ++vr.first){
        // only output edges from smaller to larger index
        const Vertex& v = *vr.first;
        const size_t v_idx = g.get_index(v);
        if(v_idx < u_idx) out <<v_idx<<" "<<u_idx<<std::endl;
      } // for
    }
  } // function

} // namespace
