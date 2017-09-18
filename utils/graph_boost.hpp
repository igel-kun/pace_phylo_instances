
#pragma once

#include <boost/graph/adjacency_list.hpp>

struct VertexProp{
  unsigned index;
  VertexProp(const unsigned _idx = 0): index(_idx) {};
};
typedef boost::adjacency_list<
      boost::vecS, // OutEdgeList
      boost::vecS,     // VertexList
      boost::undirectedS, // (bi)directed ?
      VertexProp
    > RawGraph;

// in the graph, vertices are pairs of (int,char): the index of the character and the state
class Graph: public RawGraph
{
  using RawGraph::RawGraph;

public:
  typedef typename boost::graph_traits<RawGraph>::vertex_descriptor Vertex;
  typedef typename boost::graph_traits<RawGraph>::edge_descriptor   Edge;
  typedef std::pair<unsigned, char> VertexName;
  typedef typename boost::graph_traits<Graph>::adjacency_iterator   AdjIter;
  typedef typename std::pair<AdjIter, AdjIter>                      AdjIterRange;
  typedef typename boost::graph_traits<Graph>::vertex_iterator      VertexIter;
  typedef typename std::pair<VertexIter, VertexIter>                VertexIterRange;

protected:
  boost::unordered_map<VertexName, Vertex> name_to_vertex;

public:

  size_t num_vertices() const
  {
    return boost::num_vertices(*this);
  }

  AdjIterRange adjacent_vertices(const Vertex& u) const
  {
    return boost::adjacent_vertices(u, *this);
  }

  VertexIterRange vertices() const
  {
    return boost::vertices(*this);
  }

  size_t num_edges() const
  {
    return boost::num_edges(*this);
  }

  unsigned get_index(const Vertex& u) const
  {
    return (*this)[u].index;
  }

  Vertex add_vertex(const unsigned _index)
  {
    return boost::add_vertex(VertexProp(_index), *this);
  }

  // return the vertex with the specified name or create one if the name does not exist
  const Vertex& emplace_vertex_by_name(const VertexName& vname)
  {
    const auto n2v_iter = name_to_vertex.find(vname);
    if(n2v_iter == name_to_vertex.end())
      return name_to_vertex.emplace_hint(n2v_iter, vname, add_vertex(boost::num_vertices(*this)))->second;
    else
      return n2v_iter->second;
  }

  template<typename Container = std::vector<Vertex>>
  void make_clique(const Container& clique)
  {
    // translate vertex names to vertices
    for(auto u_iter = clique.begin(); u_iter != clique.end(); ++u_iter)
      for(auto v_iter = std::next(u_iter); v_iter != clique.end(); ++v_iter)
        boost::add_edge(*u_iter, *v_iter, *this);
  }
};


