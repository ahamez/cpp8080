#include <cstdint>
#include "cpp8080/util/concat.hh"

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/

template <std::uint8_t Opcode>
struct unimplemented : meta::describe_instruction<Opcode, 255, 1>
{
  static constexpr auto name = "unimplemented";
  
  template <typename Cpu>
  void operator()(const Cpu&) const
  {
    throw std::runtime_error{util::concat("Unimplemented instruction ", Opcode)};
  }
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::meta
