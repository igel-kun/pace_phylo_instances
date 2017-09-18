

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
  if((argc < 2) || (std::string(argv[1]) == "-h") || (std::string(argv[1]) == "--help") || (std::string(argv[1]) == "/?")){
    std::cout << "syntax: "<<argv[0]<<" <file in fasta format> [output file]"<<std::endl;
    return 1;
  } else {
    Graph g;
    DEBUG1(std::cout << "reading sequences..."<<std::endl);
    SequenceMap *seq_map = new SequenceMap(argv[1]);
    CharMatrix sequences(*seq_map);
    delete seq_map;
    sequences.IsolateSNIPs();

    // for each character create vertices for each state
    const auto size = sequences.size();
    DEBUG3(std::cout << "read "<<size.first<<" species with "<<size.second<<" characters each"<<std::endl);
    for(unsigned species = 0; species < size.first; ++species){
      std::vector<Graph::Vertex> clique;
      clique.reserve(size.second);
      for(unsigned ch = 0; ch < size.second; ++ch)
        if(sequences[{species, ch}])
          clique.push_back(g.emplace_vertex_by_name(Graph::VertexName(ch, sequences[{species, ch}])));
      DEBUG3(std::cout << "adding clique "<<species<<"/"<<size.first<<" containing "<<clique.size()<<" vertices"<<std::endl);
      g.make_clique(clique);
    }

    if(argc > 2){
      std::ofstream os(argv[2]);
      io::write_edgelist(os, g);
    } else io::write_edgelist(std::cout, g);

    return 0;
  }
}
