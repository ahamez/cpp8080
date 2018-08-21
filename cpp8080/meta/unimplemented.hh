#include <cstdint>
#include <sstream>

namespace cpp8080::meta {

/*------------------------------------------------------------------------------------------------*/

template <std::uint8_t Opcode>
struct unimplemented : meta::describe_instruction<Opcode, 255, 1>
{
  static constexpr auto name = "unimplemented";
  
  template <typename Cpu>
  void operator()(const Cpu&) const
  {
    const auto ss = std::ostringstream{} << "Unimplemented instruction " << Opcode;
    throw std::runtime_error{ss.str()};
  }
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::meta
