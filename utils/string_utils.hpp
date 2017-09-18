

/** \file string_utils.hpp
 * collection of utilities working with strings
 */

#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>
#include <boost/unordered_map.hpp>

#include "utils/exceptions.hpp"
#include "utils/vector2d.hpp"

#define WHITESPACES " \t"

typedef boost::unordered_map<std::string, unsigned> StringCount;

//! remove all characters not in 'skip_to' from the beginning of s
inline void skip_to(std::string& s, const std::string& skip_to)
{
  const size_t pos = s.find_first_of(skip_to);
  if(pos == std::string::npos) s.clear(); else s.erase(0,pos);
}

//! remove all characters in 'to_skip' from the beginning of s
inline void skip_all(std::string& s, const std::string& to_skip)
{
  const size_t pos = s.find_first_not_of(to_skip);
  if(pos == std::string::npos) s.clear(); else s.erase(0,pos);
}

//! consume an integer from the beginning of s and return it
long read_single_number(std::string& s)
{
  long result = std::stoi(s.c_str());
  // remove result from the number
  skip_all(s, "+-");
  skip_all(s, "0123456789");
  return result;
}

//! remove leading & trailing chars (whitespaces by default) from str
std::string trim(const std::string& str, const std::string& to_remove = WHITESPACES)
{
    const size_t first = str.find_first_not_of(to_remove);
    if(first == std::string::npos) return "";
    const size_t last = str.find_last_not_of(to_remove);
    return str.substr(first, last - first + 1);
}

//! returns the Hamming distance between the maximal prefixes of equal length
unsigned hamming_distance(const std::string& s1, const std::string& s2)
{
  unsigned len = std::min(s1.length(), s2.length()) + 1;
  unsigned result = 0;
  while(len--) result += (s1[len] != s2[len]);
  return result;
}
//! returns the Hamming distance between the maximal prefixes of equal length (char* version)
/** Note: no checks are performed, use at own risk
 */
unsigned hamming_distance(const char* s1, const char* s2, unsigned length)
{
  unsigned result = 0;
  while(length--) result += (s1[length] != s2[length]);
  return result;
}

std::vector<unsigned> get_hamming_distances(const std::string& s1, const std::string& s2, const size_t lower_index, const size_t upper_index)
{
  std::vector<unsigned> result(upper_index - lower_index);
  for(unsigned index = lower_index; index < upper_index; ++index){
    const unsigned length = std::min(s1.length() - index, s2.length());
    result[index - lower_index] = hamming_distance(s1.c_str() + index, s2.c_str(), length);
  }
  return result;
}


enum Op : unsigned char {NOOP, DEL_LEFT, DEL_RIGHT, CHANGE};
typedef std::vector2d<Op> Levenstein_Op_Table;

//! modified Levenstein distance of an alignment of two sequences
/** this Levenstein distance is modified to absorb any suffix of s1 or s2 at no cost, as it will be used to
 * find alignments of sub-sequences and we don't want to punish the fact that one of the sequences may be shorter.
 * Note that the seeming advantage of short sequences is countered by using the AVERAGE distance for scoring.
 *
 * NOTE: this is a modification of https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C.2B.2B
 */
std::vector<unsigned> modified_levenstein_distance(const std::string& s1,
                                                   const std::string& s2,
                                                   const size_t lower_index,
                                                   const size_t upper_index,
                                                   Levenstein_Op_Table* op_table = NULL)
{
  assert(upper_index >= lower_index);
  assert(upper_index <= s1.length());
  const ssize_t len1 = s1.length() - lower_index;
  const ssize_t len2 = s2.length();
  const ssize_t window = upper_index - lower_index;
  DEBUG5(std::cout << "computing shifted Levenstein distances for strings of length "<<len1<<" and "<<len2<<" - op table @"<<op_table<<std::endl);
  // d[x,y] = the cost for the substrings s1[x:len1] and s2[y:len2]
  std::vector2d<unsigned> d(len1 + 1, len2 + 1);
  if(op_table) op_table->resize(len1 + 1, len2 + 1);

  // step1: init; allow eating any suffix of s1 OR s2 for free
  for(unsigned i = 0; i <= len1; ++i) d[{i, len2}] = 0;
  for(unsigned j = 1; j <= len2; ++j) d[{len1, j}] = 0;

  // step2: compute inner cells
  for(ssize_t i = len1-1; i >= 0; --i){
    for(ssize_t j = len2-1; j >= 0; --j){
      const unsigned del1_cost = d[{i+1, j}] + 1;
      const unsigned del2_cost = d[{i, j+1}] + 1;
      const unsigned change_cost = d[{i+1, j+1}] + (s1[i + lower_index] == s2[j] ? 0 : 1);
      if(del1_cost < del2_cost){
        if(change_cost < del1_cost){
          d[{i, j}] = change_cost;
          if(op_table) (*op_table)[{i, j}] = CHANGE;
        } else {
          d[{i, j}] = del1_cost;
          if(op_table) (*op_table)[{i, j}] = DEL_LEFT;
        }
      } else {
        if(change_cost < del2_cost){
          d[{i, j}] = change_cost;
          if(op_table) (*op_table)[{i, j}] = CHANGE;
        } else {
          d[{i, j}] = del2_cost;
          if(op_table) (*op_table)[{i, j}] = DEL_RIGHT;
        }
      }
    }
  }

  std::vector<unsigned> result(window);
  for(unsigned i = 0; i < window; ++i) result[i] = d[{i, 0}];

  return result;
}

//! a simple char consensus; This can be used if some transitions are more likely than others
char char_consensus(const char& x, const char& y)
{
  return x;
}

//! retrace an operations table for a Levenstein-distance computation, producting the implied consensus
std::string levenstein_consensus(const std::string& s1, const std::string& s2, const unsigned s2_offset, const Levenstein_Op_Table& op_table)
{
  std::string result;
  const size_t len1 = op_table.size().first - 1;
  const size_t len2 = op_table.size().second - 1;
  size_t i = s2_offset, j = 0;
  Op last_op;

  do{
    last_op = op_table[{i, j}];
    switch(last_op){
      case DEL_LEFT:
        ++i;
        break;
      case DEL_RIGHT:
        ++j;
        break;
      case CHANGE:
        result += char_consensus(s1[i], s2[j + s2_offset]);
        ++i;
        ++j;
        break;
      default:
        break;
    }
  } while(last_op != NOOP);

  if(i == len1) result += s2.substr(j);
  if(j == len2) result += s1.substr(i);
  return result;
}







//! merge two sequences seq1 and seq2 with overlap "overlap" according to their order
/** This functions segfaults for reasons that are beyond me...
 * For now, use the non-segfaulting "merge_strings()" below
 */
std::string merge_strings_segfault(const std::string& seq1,
                            const std::string& seq2,
                            const unsigned overlap)
{
  assert(overlap <= seq1.length());
  const unsigned seq1_len = seq1.length() - overlap;
  const unsigned seq2_len = seq2.length();
  const unsigned new_len = seq1_len + seq2_len;
  char* const new_seq = (char*)malloc(new_len + 1);
  char* const seq1_start = new_seq;
  char* const seq2_start = new_seq + seq1_len;
  std::cout << "got "<<new_len+1<<" bytes @"<<(long)new_seq<<std::endl;

  std::cout << "writing "<<(long)(seq2_start - seq1_start)<<" chars of "<<seq1<<" to "<<(long)seq1_start<<std::endl;
  memcpy(seq1_start, seq1.c_str(), seq1_len);
  
  std::cout << "writing "<<seq2_len<<" chars of "<<seq2<<" to "<<(long)seq2_start<<std::endl;
  memcpy(seq2_start, seq2.c_str(), seq2_len);
  new_seq[new_len+1] = 0;

  std::string result(new_seq);
  std::cout << "result string is "<< result <<" with c_str @"<<(long)(result.c_str())<<", freeing pointer @"<<(long)new_seq<<std::endl;
  free(new_seq);
  std::cout << "done!"<<std::endl;
  return result;
}

//! merge two sequences seq1 and seq2 with overlap "overlap" according to their order
/** For example: merge_strings("abcde", "1234", 3) = "ab1234"
 */
std::string merge_strings(const std::string& seq1,
                            const std::string& seq2,
                            const unsigned overlap)
{

  std::string result = seq1.substr(0, seq1.length() - overlap) + seq2;
  return result;
}



#endif
