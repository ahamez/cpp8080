#pragma once

#include <cstdint>
#include <vector>

#include "cpp8080/meta/instructions.hh"
#include "cpp8080/specific/cpu.hh"

#include "events.hh"
#include "sdl.hh"

/*------------------------------------------------------------------------------------------------*/

class space_invaders
{
private:

  struct in_override : cpp8080::meta::describe_instruction<0xdb, 3, 2>
  {
    static constexpr auto name = "in";

    void operator()(cpp8080::specific::cpu<space_invaders>& cpu) const
    {
      const auto port = cpu.op1();
      cpu.a = cpu.machine().in(port);
    }
  };

  struct out_override : cpp8080::meta::describe_instruction<0xd3, 3, 2>
  {
    static constexpr auto name = "out";

    void operator()(cpp8080::specific::cpu<space_invaders>& cpu) const
    {
      const auto port = cpu.op1();
      cpu.machine().out(port, cpu.a);
    }
  };

public:

  using overrides = cpp8080::meta::make_instructions<
    in_override,
    out_override
  >;

public:

  template <typename InputIterator>
  space_invaders(InputIterator first, InputIterator last)
    : cpu_{*this}
    , memory_(16384, 0)
    , shift0_{0}
    , shift1_{0}
    , shift_offset_{0}
    , port1_{1 << 3}
    , port2_{0}
  {
    std::copy(first, last, memory_.begin());
  }

  [[nodiscard]]
  std::uint8_t
  in(std::uint8_t port)
  const noexcept;

  void
  out(std::uint8_t port, std::uint8_t value)
  noexcept;

  void
  memory_write_byte(std::uint16_t address, std::uint8_t value);

  [[nodiscard]]
  std::uint8_t
  memory_read_byte(std::uint16_t address)
  const;

  //  TODO space_invaders doesn't need to know sdl type.
  void
  operator()(sdl&);

private:

  bool
  process_events(sdl&);

  void
  key_down(event)
  noexcept;

  void
  key_up(event)
  noexcept;

private:

  cpp8080::specific::cpu<space_invaders> cpu_;
  std::vector<std::uint8_t> memory_;
  std::uint8_t shift0_;
  std::uint8_t shift1_;
  std::uint8_t shift_offset_;
  std::uint8_t port1_;
  std::uint8_t port2_;
};

/*------------------------------------------------------------------------------------------------*/
