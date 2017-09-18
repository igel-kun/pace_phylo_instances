
//! file bitset2d.hpp
/** This is a 2 dimentional bitset (aka bit-matrix) that is slightly faster than
 * vector<bitset> and slightly more convenient than keeping track of the
 * index offsets using bitset
 **/

#pragma once

#include <cassert>
#include "vector2d.hpp"
#include <boost/dynamic_bitset.hpp>

namespace std{
  template<typename Symmetry = Asymmetric>
  class bitset2d : public Something2d<bool, boost::dynamic_bitset<unsigned>, Symmetry>
  {
  protected:
    using Parent = Something2d<bool, boost::dynamic_bitset<unsigned>, Symmetry>;
    using GrandPa = typename Parent::Parent;
    using Coords = typename Parent::Coords;

  public:
    bitset2d& set(const Coords& coords, bool val = true)
    {
      GrandPa::set(Parent::linearize(coords), val); return *this;
    }
    bitset2d& reset(const Coords& coords, bool val = false)
    {
      return set(coords, val);
    }
    bitset2d& flip(const Coords& coords)
    {
      GrandPa::flip(Parent::linearize(coords)); return *this;
    }
    bool test(const Coords& coords) const
    {
      return GrandPa::test(Parent::linearize(coords));
    }
    bool test_set(const Coords& coords, bool val = true)
    {
      return GrandPa::test_set(Parent::linearize(coords), val);
    }
    size_t count() const
    {
      return GrandPa::count();
    }
  };
  typedef bitset2d<Symmetric> symmetric_bitset2d;

/*
  class _bitset2d : public boost::dynamic_bitset<unsigned>
  {
  protected:
    using Parent = boost::dynamic_bitset<unsigned>;
    using Coords = pair<size_t, size_t>;
    //! translate 2d coordinates into 1d coordinates
    virtual size_t linearize(const Coords& coords) const = 0;

  public:
    virtual ~_bitset2d(){}
    _bitset2d(size_t num_bits = 0): boost::dynamic_bitset<unsigned>(num_bits) {}
    //! NOTE: coordinates are always (col, row) as in (x, y)
    Parent::reference operator[](const Coords& coords)
    {
      return Parent::operator[](linearize(coords));
    }
    bool operator[](const Coords& coords) const
    {
      return Parent::operator[](linearize(coords));
    }
    _bitset2d& set(const Coords& coords, bool val = true)
    {
      Parent::set(linearize(coords), val); return *this;
    }
    _bitset2d& reset(const Coords& coords, bool val = false)
    {
      return set(coords, val);
    }
    _bitset2d& flip(const Coords& coords)
    {
      Parent::flip(linearize(coords)); return *this;
    }
    bool test(const Coords& coords) const
    {
      return Parent::test(linearize(coords));
    }
    bool test_set(const Coords& coords, bool val = true)
    {
      return Parent::test_set(linearize(coords), val);
    }
    void resize(const size_t cols, const size_t rows)
    {
      Parent::resize(linearize({cols - 1, rows - 1}) + 1);
    }
    virtual Coords size() const = 0;
  };// class bitset2d



  template<typename T>
  class bitset2d : public _bitset2d
  {
  protected:
    using Parent = _bitset2d;
    using Parent::Coords;

    size_t columns;

    size_t linearize(const Coords& coords) const
    {
      return coords.second * columns + coords.first;
    }
  public:
    bitset2d(): _bitset2d() {}

    // reserve size such that we can access the last index (cols-1,rows-1)
    bitset2d(const size_t cols, const size_t rows):
      columns(cols),
      _bitset2d(linearize(cols - 1, rows - 1) + 1)
    {}

    void resize(const size_t cols, const size_t rows)
    {
      Parent::resize(cols, rows);
      columns = cols;
    }
    Coords size() const
    {
      assert(columns > 0);
      return Coords(columns, Parent::Parent::size() / columns);
    }
  };


  //! a symmetric version of the 2d bitset that only uses half the space
  class symmetric_bitset2d : public _bitset2d
  {
  protected:
    using Parent = _bitset2d;
    using Parent::Coords;

    //! translate 2d coordinates into 1d coordinates
    size_t linearize(const Coords& coords) const
    {
      if(coords.first <= coords.second)
        return (coords.second * (coords.second + 1)) / 2  + coords.first;
      else
        return (coords.first * (coords.first + 1)) / 2  + coords.second;
    }
    
  public:
    symmetric_bitset2d(): _bitset2d() {}

    symmetric_bitset2d(const size_t rows_and_cols):
      _bitset2d(linearize({rows_and_cols, rows_and_cols}))
    {}

    size_t rows() const
    {
      return sqrt(Parent::Parent::size() * 2 + 0.25) - .5;
    }
    Coords size() const
    {
      const size_t root = rows();
      return Coords(root, root);
    }
  };// class bitset2d

*/

}// namespace


