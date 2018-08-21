#pragma once

#include <sstream>
#include <string>

namespace cpp8080::util {

/*------------------------------------------------------------------------------------------------*/

template <typename... Args>
std::string
concat(Args&&... args)
{
  return (std::ostringstream{} << ... << args).str();
}

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::util
