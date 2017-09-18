

#pragma once

#include <boost/unordered_map.hpp>
#include <string>
#include "utils/vector2d.hpp"

#define BASES "ABCDEFGHIJKLMNOPQRSTUVWXYZ*-"
#define CYCLIC_SEQUENCE_INDICATOR "(c)"
#define REVERSE_SEQUENCE_INDICATOR "(rev)"

const char* COMPLEMENTARY_BASES = "ATAUCGRYMK";


// forward declaration to read fasta files in Sequences constructor
class SequenceMap;
namespace io{
  void read_fasta_file(const std::string& input, SequenceMap& name_to_seq, const std::string& acceptable_bases);
}

  // return the index behind the real name if name is "(real_name)(rev)" and 0 otherwise
  unsigned is_reversed_sequence(const std::string& name)
  {
    unsigned start_of_rev_indicator = name.length() - strlen(REVERSE_SEQUENCE_INDICATOR);
    const unsigned end_of_name = start_of_rev_indicator - 1;
    if(name[0] != '(') return 0;
    if(name[end_of_name] != ')') return 0;
    if(name.substr(start_of_rev_indicator) != REVERSE_SEQUENCE_INDICATOR) return 0;
    // check that the bracket at index 0 really does belong to the one at end_of_name
    unsigned level = 1;
    const char* name_cptr = name.c_str();
    for(unsigned i = 1; i != end_of_name; ++i){
      const char c = name_cptr[i];
      switch(c){
        case '(': ++level; break;
        case ')': --level; break;
      }
      if(level == 0) return 0;
    }
    return start_of_rev_indicator;
  }

  // change name as to indicate that it's referring to a reversed sequence
  void indicate_reversal(std::string& name)
  {
    // if the sequence is already reversed, just remove the reverse indicator, otherwise, add a reverse indicator
    const unsigned start_of_rev_indicator = is_reversed_sequence(name);
    if(start_of_rev_indicator){
      name.erase(start_of_rev_indicator-1);
      name.erase(0, 1);
    } else name = '(' + name + ')' + REVERSE_SEQUENCE_INDICATOR;
  }


  // get the complement of the given base, or 'N' if it does not have a complement
  char get_complement(const char base)
  {
    const char* complement_index = std::strchr(COMPLEMENTARY_BASES, base);
    if(complement_index != NULL){
      const int offset = (complement_index - COMPLEMENTARY_BASES);
      assert(offset >= 0);
      char result = COMPLEMENTARY_BASES[(offset % 2) ? offset - 1 : offset + 1];
      return result;
    } else return 'N';
  }

  // reverse complement a given string in place
  void reverse_complement_inplace(std::string& sequence)
  {
    const int seq_len = sequence.length();
    const int last_index = seq_len - 1;
    for(int i = 0; 2 * i < last_index; ++i){
      const char swap_base = sequence[i];
      sequence[i] = get_complement(sequence[last_index - i]);
      sequence[last_index - i] = get_complement(swap_base);
    }
    // reverse the middle if the sequence length is even
    if(seq_len % 2) sequence[last_index / 2] = get_complement(sequence[last_index / 2]);
  }

  // return the reverse complement of a sequence
  std::string reverse_complement(const std::string& sequence)
  {
    std::string out(sequence);
    reverse_complement_inplace(out);
    return out;
  }


  // return the reverse complement of a sequence, modifying its name to account for it
  std::string named_reverse_complement(const std::string& sequence, std::string& name)
  {
    std::string out(sequence);
    reverse_complement_inplace(out);
    indicate_reversal(name);
    return out;
  }


// a SequenceMap assigns each contig name a sequence of base pairs
class SequenceMap: public boost::unordered_map<std::string, std::string>
{
  using Parent = boost::unordered_map<std::string, std::string>;
  using Parent::unordered_map;

public:

  //! a constructor that reads sequence data from a file
  SequenceMap(const std::string& filename):
    Parent::unordered_map()
  {
    io::read_fasta_file(filename, *this, "");
  }
};

class CharMatrix: public std::vector2d<char>
{
  using Parent = std::vector2d<char>;
  using Parent::columns;

public:

  CharMatrix(const SequenceMap& sequences):
    Parent(sequences.size(), sequences.begin()->second.size())
  {
    const unsigned num_chars = sequences.begin()->second.size();
    unsigned seq_id = 0;
    for(const auto& seq: sequences){
      assert(seq.second.size() == num_chars);
      for(unsigned i = 0; i < num_chars; ++i)
        operator[]({seq_id, i}) = seq.second[i];
      ++seq_id;
    }
  }

  //! remove all characters that have only one state
  //NOTE: removed characters are encoded as characters having state '\0'
  void IsolateSNIPs()
  {
    const auto sz = size();
    for(unsigned ch = 0; ch < sz.second; ++ch){
      const char first_state = operator[]({0, ch});
      bool broke = false;
      if(first_state != '-'){
        for(unsigned species = 1; species < sz.first; ++species)
          if((broke = ((operator[]({species, ch}) != first_state) || (operator[]({species, ch}) == '-')))) break;
      } else broke = false;
      if(!broke){
        for(unsigned species = 0; species < sz.first; ++species)
          operator[]({species, ch}) = 0;
      }
    }
  }
};


typedef std::list<std::pair<std::string, std::string> > NamedSequenceList;




