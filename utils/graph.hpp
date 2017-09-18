
#pragma once

#include <boost/unordered_map.hpp>
#include "utils/bitset2d.hpp"

typedef std::symmetric_bitset2d AdjMatrix;

class Counter
{
protected:
  unsigned current;
public:
  Counter(const unsigned i): current(i){}
  unsigned operator*() const { return current; }
  Counter& operator++() { ++current; return *this; }
  Counter operator++(int) { return Counter(current++); }
  bool operator==(const Counter& c) const { return c.current == current; }
  bool operator!=(const Counter& c) const { return c.current != current; }
  Counter& operator=(const unsigned i) { current = i; return *this; }
};

class AdjMatrixIter
{
protected:
  const AdjMatrix& m;
  const unsigned end;
  const unsigned vertex;
  unsigned current;

  void fix_current()
  {
    while((current < end) && !m.test({vertex, current})) ++current;
  }
public:
  AdjMatrixIter(const AdjMatrix& _m, const unsigned _vertex, const unsigned _current):
    m(_m), end(_m.cols()), vertex(_vertex), current(_current)
  {
    fix_current();
  }
  unsigned operator*() const { return current; }
  AdjMatrixIter& operator++() { ++current; fix_current(); return *this; }
  AdjMatrixIter operator++(int)
  {
    AdjMatrixIter tmp(m, vertex, current++);
    fix_current();
    return tmp;
  }
  bool operator==(const AdjMatrixIter& c) const { return c.current == current; }
  bool operator!=(const AdjMatrixIter& c) const { return c.current != current; }
  AdjMatrixIter& operator=(const unsigned i) { current = i; fix_current(); return *this; }
};

struct AdjIterRange
{
  AdjMatrixIter first, second;

  AdjIterRange(const AdjMatrix& _m, const unsigned v):
    first(_m, v, 0),
    second(_m, v, _m.cols())
  {}
};
// in the graph, vertices their indices
class Graph
{
public:
  typedef uint32_t Vertex;
  typedef std::pair<Vertex, Vertex> Edge;
  typedef std::pair<uint32_t, char> VertexName;
  typedef Counter VertexIter;
  typedef std::pair<VertexIter, VertexIter> VertexIterRange;
  typedef AdjMatrixIter AdjIter;

protected:
  boost::unordered_map<VertexName, Vertex> name_to_vertex;
  AdjMatrix adj;
public:

  size_t num_vertices() const
  {
    return adj.cols();
  }

  size_t num_edges() const
  {
    return adj.count();
  }

  unsigned get_index(const Vertex& u) const
  {
    return u;
  }

  Vertex add_vertex()
  {
    const Vertex v = adj.cols();
    adj.resize(v + 1, v + 1);
    return v;
  }

  // return the vertex with the specified name or create one if the name does not exist
  const Vertex& emplace_vertex_by_name(const VertexName& vname)
  {
    const auto n2v_iter = name_to_vertex.find(vname);
    if(n2v_iter == name_to_vertex.end())
      return name_to_vertex.emplace_hint(n2v_iter, vname, add_vertex())->second;
    else
      return n2v_iter->second;
  }

  void add_edge(const Vertex u, const Vertex v)
  {
    adj.set({u,v});
  }

  template<typename Container = std::vector<Vertex>>
  void make_clique(const Container& clique)
  {
    // translate vertex names to vertices
    for(auto u_iter = clique.begin(); u_iter != clique.end(); ++u_iter)
      for(auto v_iter = std::next(u_iter); v_iter != clique.end(); ++v_iter)
        add_edge(*u_iter, *v_iter);
  }

  VertexIterRange vertices() const
  {
    return VertexIterRange(0, adj.cols());
  }
  AdjIterRange adjacent_vertices(const Vertex v) const
  {
    return AdjIterRange(adj, v);
  }

  void isolate_vertex(const Vertex& u)
  {
    unsigned v = adj.cols() - 1;
    do adj.reset({u,v}); while(v-- != 0);
  }
};


