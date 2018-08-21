#pragma once

#include <exception>
#include <string>

#include "cpp8080/util/concat.hh"

namespace cpp8080::specific {

/*------------------------------------------------------------------------------------------------*/

class halt
  : std::exception
{
public:

  halt(const std::string& reason)
    : reason_{util::concat("CPU halted: ", reason)}
  {}

  const char* what() const noexcept override
  {
    return reason_.c_str();
  }

private:

  std::string reason_;
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::specific
