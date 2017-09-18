
/** \file utils.hpp
 * collection of general utilities
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <bitset>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

#define default_buckets boost::unordered::detail::default_bucket_count


#ifndef M_PI
#define M_PI 3.14159265359f
#endif
#define byte unsigned char
#define SIZE_T_BITS (unsigned)(8*sizeof(size_t))
// avoid the clutter for map-emplaces where both constructors take just one argument
#define DEEP_EMPLACE(x,y) emplace(std::piecewise_construct, std::make_tuple(x), std::make_tuple(y))
#define DEEP_EMPLACE_TWO(x,y,z) emplace(std::piecewise_construct, std::make_tuple(x), std::make_tuple(y, z))
// avoid the clutter for map-emplaces where the target is default constructed
#define DEFAULT_EMPLACE(x) emplace(std::piecewise_construct, std::make_tuple(x), std::make_tuple())
// merge two lists
#define SPLICE_LISTS(x,y) x.splice(x.end(), y)

#ifdef LONG_WIDTH
  #define MAX_TW (byte)LONG_WIDTH
#else
  #define MAX_TW (byte)64
#endif

// on debuglevel 3 all DEBUG1, DEBUG2, and DEBUG3 statements are evaluated
#ifndef NDEBUG
  #ifndef debuglevel
    #define debuglevel 5
  #endif
#else
  #define debuglevel 0
#endif

#if debuglevel > 0
#define DEBUG1(x) x
#else
#define DEBUG1(x)
#endif

#if debuglevel > 1
#define DEBUG2(x) x
#else
#define DEBUG2(x)
#endif

#if debuglevel > 2
#define DEBUG3(x) x
#else
#define DEBUG3(x)
#endif

#if debuglevel > 3
#define DEBUG4(x) x
#else
#define DEBUG4(x)
#endif

#if debuglevel > 4
#define DEBUG5(x) x
#else
#define DEBUG5(x)
#endif

#if debuglevel > 5
#define DEBUG6(x) x
#else
#define DEBUG6(x)
#endif

#ifdef STATISTICS
#define STAT(x) x
#else
#define STAT(x) 
#endif


// inverse error function (approximative)
// thx to
// https://stackoverflow.com/questions/27229371/inverse-error-function-in-c
// and
// A handy approximation for the error function and its inverse" by Sergei Winitzki.
float erf_inv_apx(const float& p)
{
  const float sgn = (p < 0) ? -1.0f : 1.0f;

  // lnx = log(1 - x*x)
  const float lnx = logf((1 - p)*(1 + p));

  const float tt1 = 2/(M_PI*0.147) + 0.5f * lnx;
  const float tt2 = 1/(0.147) * lnx;

  return sgn*sqrtf(-tt1 + sqrtf(tt1*tt1 - tt2));
}


//! return whether a pair is pareto-smaller than another pair
template <typename ElementA, typename ElementB>
bool pareto_le(const std::pair<ElementA,ElementB>& p1, const std::pair<ElementA,ElementB>& p2)
{
  return (p1.first <= p2.first) && (p1.second <= p2.second);
}

//! output list of things
template<typename Element>
std::ostream& operator<<(std::ostream& os, const std::list<Element>& lst)
{
  for(auto i : lst) os << i << "  ";
  return os;
}
//! output map of things
template<typename Element1, typename Element2>
std::ostream& operator<<(std::ostream& os, const boost::unordered_map<Element1, Element2>& map)
{
  for(auto i : map) os << i << "  ";
  return os;
}

//! a more readable containment check
/** \param s any container object that implements find() and cend()
 * \param el some element to check containment of in s
 */
template <class Set, typename Element>
inline bool contains(const Set& s, const Element& el)
{
  return s.find(el) != s.cend();
}

//! a hash computation for an unordered set, XORing its members
template<typename T>
size_t hash_value(const boost::unordered_set<T>& S)
{
  size_t result = 0;
  for(const auto& i : S)
    result = (result << 1) ^ hash_value(i);
  return result;
}

//! set intersection for unordered sets
/** removes all elements from S1 that are not in S2 */
template<typename T>
void operator&=(boost::unordered_set<T>& S1, const boost::unordered_set<T>& S2)
{
  for(typename boost::unordered_set<T>::iterator i = S1.begin(); i != S1.end();)
    if(!contains(S2, *i)) i = S1.erase(i); else ++i;
}

//! setminus for unordered sets
/** removes all elements from S1 that are in S2 */
template<typename T>
void operator-=(boost::unordered_set<T>& S1, const boost::unordered_set<T>& S2)
{
  for(typename boost::unordered_set<T>::iterator i = S1.begin(); i != S1.end();)
    if(contains(S2, *i)) i = S1.erase(i); else ++i;
}

//! symmetric set difference for unordered sets
/** removes all elements of S1 that are not in S2 and emplaces copies of all elements of S2 that are not in S1 */
template<typename T>
void operator^=(boost::unordered_set<T>& S1, const boost::unordered_set<T>& S2)
{
  for(typename boost::unordered_set<T>::iterator i = S1.begin(); i != S1.end();)
    if(contains(S2, *i)) i = S1.erase(i); else ++i;
  for(typename boost::unordered_set<T>::iterator i = S2.begin(); i != S2.end(); ++i) S1.emplace(*i);
}

//! testing whether a file exists by trying to open it
inline bool file_exists(const std::string& filename) 
{
  return std::ifstream(filename.c_str()).good();
}

//! reverse a pair of things, that is, turn (x,y) into (y,x)
template<typename A, typename B>
inline std::pair<B,A> reverse(const std::pair<A,B>& p)
{
  return {p.second, p.first};
}

//! add two pairs of things
template <typename A, typename B>
std::pair<A,B> operator+(const std::pair<A,B>& l, const std::pair<A,B>& r)
{
	return {l.first + r.first, l.second + r.second};
}

template <typename A, typename B>
std::ostream& operator<<(std::ostream& os, const std::pair<A,B>& p)
{
	return os << "("<<p.first<<", "<<p.second<<")";
}




#endif
