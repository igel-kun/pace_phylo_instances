
#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "utils/exceptions.hpp"
#include "utils/sequences.hpp"
#include "utils/string_utils.hpp" // for trim

#define FASTA_MAX_LINELENGTH 79

namespace io {

  // read a fasta file into an associative name_to_seq mapping a sequence name to its sequence
  void read_fasta_file(std::istream& input, SequenceMap& name_to_seq, const std::string& acceptable_bases = "")
  {
    SequenceMap::iterator current_entry;
    bool success;
    unsigned line_no = 0;
    std::string line; // input buffer
    
    name_to_seq.clear();
    while(std::getline(input, line).good()){
      ++line_no;
      if(!line.empty()){
        if(line[0] == '>'){
          // if the line starts with '>' it's a sequence name
          // step 1: remove leading and trailing whitespaces of the sequence name
          std::string trimmed_line(trim(line.erase(0, 1)));
          if(trimmed_line.empty()) throw except::bad_syntax(line_no, "empty sequence name");
          // step 2: insert into the unordered_map
          boost::tie(current_entry, success) = name_to_seq.DEEP_EMPLACE(trim(line), "");
          if(!success) throw except::bad_syntax(line_no, (std::string)"repeated sequence name: " + line);
        } else {
          // if the line is not empty and does not start with '>', then it's part of the sequence
          if(current_entry == name_to_seq.end()) throw except::bad_syntax(line_no, "missing sequence name");
          // sanity check
          if(!acceptable_bases.empty())
            if(line.find_first_not_of(acceptable_bases) != std::string::npos)
              throw except::bad_syntax(line_no, (std::string)"contains a base that's not in " + acceptable_bases);
          // append the line to the current sequence
          current_entry->second += line;
        }// if
      }// if line not empty
    }// while file contains data
  }// function

  void read_fasta_file(const std::string& input, SequenceMap& name_to_seq, const std::string& acceptable_bases = "")
  {
    std::ifstream in_stream(input);
    read_fasta_file(in_stream, name_to_seq, acceptable_bases);
  }

  // write the sequences into a fasta file 
  void write_sequence_map(std::ostream& out, SequenceMap& name_to_seq)
  {
    for(const auto& name_seq: name_to_seq){
      out << ">" << name_seq.first << std::endl;
      unsigned pos = 0;
      try{
        while(true){
          const std::string buffer = name_seq.second.substr(pos, FASTA_MAX_LINELENGTH);
          out << buffer << std::endl;
          pos += FASTA_MAX_LINELENGTH;
        }
      } catch(std::out_of_range e) {};
    }
  }// function


}// namespace

