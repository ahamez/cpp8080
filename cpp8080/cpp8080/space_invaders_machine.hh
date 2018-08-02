#pragma once

#include <cstdint>

namespace cpp8080 {

/*------------------------------------------------------------------------------------------------*/

class spache_invaders_machine
{
public:

  [[nodiscard]]
  std::uint8_t
  in(std::uint8_t port)
  const noexcept
  {
    switch (port)
    {
      case 0:
        return 1;

      case 1:
        return 0;

      case 3:
      {
        const std::uint16_t v = (shift1_ << 8) | shift0_;
        return (v >> (8 - shift_offset_)) & 0xff;
      }
    }
    return 0;
  }

  void
  out(std::uint8_t port, std::uint8_t value)
  noexcept
  {
    switch (port)
    {
      case 2:
        shift_offset_ = value & 0x7;
        break;

      case 4:
        shift0_ = shift1_;
        shift1_ = value;
        break;
    }
  }

private:
  
  std::uint8_t shift0_;
  std::uint8_t shift1_;
  std::uint8_t shift_offset_;
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080
