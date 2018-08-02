#pragma once

namespace cpp8080 {

/*------------------------------------------------------------------------------------------------*/

[[nodiscard]]
inline
bool
even_parity(std::uint8_t x)
noexcept
{
  x ^= x >> 4;
  x ^= x >> 2;
  x ^= x >> 1;
  return (~x) & 1;
}

/*------------------------------------------------------------------------------------------------*/
  
} // namespace cpp8080
