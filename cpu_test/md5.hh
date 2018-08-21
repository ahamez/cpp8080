#pragma once

#include <array>
#include <cstdint>
#include <string>

/*------------------------------------------------------------------------------------------------*/

class md5
{
public:

  using checksum = std::array<std::uint8_t, 16>;

public:

  md5();

  md5& update(const std::uint8_t* data, std::size_t size) noexcept;
  checksum operator()() noexcept;

  static std::string digest(const checksum&);

private:

  const std::uint8_t* body(const std::uint8_t* ptr, std::size_t size) noexcept;

private:

	std::uint_fast32_t a_;
  std::uint_fast32_t b_;
  std::uint_fast32_t c_;
  std::uint_fast32_t d_;

  std::uint_fast32_t lo_;
  std::uint_fast32_t hi_;

  std::array<std::uint8_t, 64> buffer_;
};

/*------------------------------------------------------------------------------------------------*/
