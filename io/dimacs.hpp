/*
  Reads coloring problem in DIMACS format.
  This works, but could use some polishing.
*/

/* ----------------------------------------------------------------- */

#pragma once

#include <vector>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>


#include "utils/exceptions.hpp"

namespace io {

  // read the graph g from the instream "in"
  template<typename Graph, typename Vertex = typename Graph::Vertex, typename Edge = typename Graph::Edge>
  bool read_dimacs_graph(std::istream& in, Graph& g)
  {
    const uint P_FIELDS = 2; // number of fields expected in a "p" line
    const uint E_FIELDS = 2; // number of fields expected in an "e" line
    unsigned no_lines = 0; // number of lines read
    unsigned no_elines = 0; // number of lines defining edges
    unsigned num_vertices, num_edges; // number of vertices & edges in the graph
    std::string in_line; // buffer for the next line
    std::vector<Vertex> verts;

    try{
      while(std::getline(in, in_line)) {
        ++no_lines;
        switch (in_line[0]) {
          case 'c':                  /* skip lines with comments */
          case '\n':                 /* skip empty lines   */
          case '\0':                 /* skip empty lines at the end of file */
            break;
          case 'p':   /* contains "edge NUMBER_NODES NUMBER_EDGES */
            if(std::sscanf(in_line.c_str(), "%*c %*s %u %u", &num_vertices, &num_edges) != P_FIELDS) 
              throw except::bad_syntax(no_lines, "unexpected p-line format");
            verts.reserve(num_vertices);
            for(unsigned vi = 0; vi < num_vertices; ++vi) verts.push_back(g.add_vertex(vi));
            break;
          case 'e':                    /* edge description */
            unsigned u,v; // store the indices of the incident vertices
            if(no_elines >= num_edges) throw except::bad_syntax(no_lines,"too many edges specified or no p line found");
            if(std::sscanf(in_line.c_str(),"%*c %u %u", &u, &v) != E_FIELDS) 
              throw except::bad_syntax(no_lines, "unexpected e-line format");
            --u; // index from 0, not 1
            --v;
            if((u < num_vertices) && (v < num_vertices)){
              Edge e;
              bool success;
              boost::tie(e, success) = g.add_edge(verts[u], verts[v]);
              assert(success);
            } else throw except::bad_syntax(no_lines, "vertex index out of bounds");
            ++no_elines;
            break;
          default:  /* unknown type of line */
            throw except::bad_syntax(no_lines, "unrecognized line type");
        } // switch
      } // while
      if ( in.eof() == 0 ) throw except::bad_syntax(no_lines, "read error"); 
      if ( no_lines == 0 ) throw except::read_error(no_lines, "input stream is empty");
      if ( no_elines < num_edges ) throw except::bad_syntax(no_lines, "did not read enough edges");
    } catch(except::bad_syntax ex){
      std::cout << "Syntax error in line "<<ex.line_no<<": "<<ex.what()<<std::endl;
      return false;
    } catch(except::read_error ex){
      std::cout << "Error reading after "<<ex.line_no<<" lines: "<<ex.what()<<std::endl;
      return false;
    }
    return true;
  } // function


  // write the graph g to the outstream "out"
  template<typename Graph, typename Vertex = typename Graph::Vertex, typename Edge = typename Graph::Edge>
  void write_dimacs_graph(std::ostream& out, const Graph& g){
    // write p line
    out << "p edge "<<g.num_vertices()<<" "<<g.num_edges()<<std::endl;

    // write e lines. To this end, iterate over all vertices & print their adjacent vertices one by one
    for(auto ur = g.vertices(); ur.first != ur.second; ++ur.first){
      const Vertex& u = *ur.first;
      const size_t u_idx = g.get_index(u);
      for(auto vr = g.adjacent_vertices(u); vr.first != vr.second; ++vr.first){
        // only output edges from smaller to larger index
        const Vertex& v = *vr.first;
        const size_t v_idx = g.get_index(v);
        if(v_idx < u_idx){
          // remember to shift the vertex index by one, since boost starts with 0 and DIMACS starts with 1
          out << "e "<<v_idx + 1<<" "<<u_idx+1<<std::endl;
        }
      } // for
    }
  } // function
} // namespace
