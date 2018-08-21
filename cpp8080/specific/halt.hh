#pragma once

#include <exception>
#include <sstream>
#include <string>

namespace cpp8080::specific {

/*------------------------------------------------------------------------------------------------*/

class halt
  : std::exception
{
public:

  halt(const std::string& reason)
    : reason_{}
  {
    const auto ss = std::ostringstream{} << "CPU halted: " << reason;
    reason_ = ss.str();
  }

  const char* what() const noexcept override
  {
    return reason_.c_str();
  }

private:

  std::string reason_;
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::specific
