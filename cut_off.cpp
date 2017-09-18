

// turn a multiple alignment into an instance of chordal completion using
// the theorem of Bunemann:
// a character-state matrix allows a perfect phylogeny <=> the character-state intersection graph has a properly colored chordal supergraph
// herein, the character state intersection graph is the graph resulting from:
// for each species s add a clique of the vertices (c,i) such that character c has state i in species s

#include <iostream>
#include <vector>
#include "utils/graph.hpp"
#include "io/fasta.hpp"
//#include "io/dimacs.hpp"
#include "io/edgelist.hpp"

int main(int argc, char* argv[])
{
  if((argc < 3) || (std::string(argv[1]) == "-h") || (std::string(argv[1]) == "--help") || (std::string(argv[1]) == "/?")){
    std::cout << "syntax: "<<argv[0]<<" <graph file> <threshold index> [output file]"<<std::endl;
    exit(EXIT_FAILURE);
  } else {
    Graph g;
    DEBUG1(std::cout << "reading graph..."<<std::endl);
    std::ifstream in_file(argv[1]);
    io::read_edgelist(in_file, g);
    const unsigned threshold = std::atoi(argv[2]);
    
    if(threshold > 0){
      DEBUG1(std::cout << "cutting off vertices of index > "<<threshold<<std::endl);
      DEBUG3(std::cout << "currently, "<<g.num_edges()<<" edges"<<std::endl);
      for(unsigned i = g.num_vertices() - 1; i >= threshold; --i)
        g.isolate_vertex(i);

      if(argc > 3){
        std::ofstream os(argv[3]);
        io::write_edgelist(os, g);
      } else io::write_edgelist(std::cout, g);
    }

    exit(EXIT_SUCCESS);
  }
}
