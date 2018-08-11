#include <cstdint>
#include <sstream>

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/

template <std::uint8_t Opcode>
struct unimplemented : meta::describe_instruction<Opcode, 255, 1>
{
  static constexpr auto name = "unimplemented";
  
  template <typename State>
  void operator()(const State&) const
  {
    auto ss = std::stringstream{};
    ss << "Unimplemented instruction " << Opcode;
    throw std::runtime_error{ss.str()};
  }
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::meta
