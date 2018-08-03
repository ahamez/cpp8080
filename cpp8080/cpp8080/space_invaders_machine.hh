#pragma once

#include <cstdint>

namespace cpp8080 {

/*------------------------------------------------------------------------------------------------*/

class space_invaders_machine
{
public:
  
  enum class key {coin, left, right, fire, start};

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

  void
  key_down(key k)
  noexcept
  {
    switch (k)
    {
      case key::coin  : in_port1 |= 0x01; break;
      case key::left  : in_port1 |= 0x20; break;
      case key::right : in_port1 |= 0x40; break;
      case key::fire  : in_port1 |= 0x10; break;
      case key::start : in_port1 |= 0x04; break;
    }
  }
  
  void
  key_up(key k)
  noexcept
  {
    switch (k)
    {
      case key::coin  : in_port1 &= ~0x01; break;
      case key::left  : in_port1 &= ~0x20; break;
      case key::right : in_port1 &= ~0x40; break;
      case key::fire  : in_port1 &= ~0x10; break;
      case key::start : in_port1 &= ~0x04; break;
    }
  }

  
private:
  
  std::uint8_t shift0_;
  std::uint8_t shift1_;
  std::uint8_t shift_offset_;
  std::uint8_t in_port1;
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080
