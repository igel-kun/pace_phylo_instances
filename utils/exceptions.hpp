
/** \file exceptions.hpp
 * exceptions used in the program
 */


#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <string>
#include <exception>
#include "utils/utils.hpp"

namespace except {

  //! an exception for the case that a graph property is read that is not up to date
  struct info_not_up_to_date: public std::exception 
  {
    const std::string which_info;
    info_not_up_to_date(const std::string _which_info = "unknown info"):
      which_info(_which_info + " not up to date") {};

    const char* what() const throw() {
      return which_info.c_str();
    }
  };

  //! an exception for the case that some assumption that is made is not true
  /** for example when running the procedure treating deg-2 graphs for a graph that is not deg-2 */
  struct invalid_assumption: public std::exception 
  {
    const std::string which;
    invalid_assumption(const std::string _which): which(_which) {};

    const char* what() const throw() {
      return which.c_str();
    }
  };

  //! an exception for the case that the given program options are not valid
  struct invalid_options: public invalid_assumption{
    using invalid_assumption::invalid_assumption;
  };

  //! an exception for the case that a given file could not be read on a low level
  struct read_error: public std::exception {
    const unsigned line_no;   //!< store the line number on which the exception ocurred
    const std::string errmsg; //!< store the error string

    read_error(const unsigned _line_no, const std::string& _errmsg):
      line_no(_line_no), errmsg(_errmsg) {}
    const char* what() const noexcept { return errmsg.c_str(); }
  };

  //! an exception for the case that a given file does not conform to expected syntax
  struct bad_syntax: public read_error {
    using read_error::read_error;
  };

}// namespace

#endif
