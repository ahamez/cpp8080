#include <algorithm>
#include <chrono>
#include <sstream>
#include <thread>

#include "space_invaders.hh"

/*------------------------------------------------------------------------------------------------*/

static constexpr auto fps = 60;
static constexpr auto cycles_per_second = 2'000'000; // 2 MHz
static constexpr auto cycles_per_frame = cycles_per_second / fps;

/*------------------------------------------------------------------------------------------------*/

[[nodiscard]]
std::uint8_t
space_invaders::in(std::uint8_t port)
const noexcept
{
  switch (port)
  {
    case 1:
      return port1_;

    case 2:
      return port2_;

    case 3:
    {
      const std::uint16_t v = (shift1_ << 8) | shift0_;
      return (v >> (8 - shift_offset_)) & 0xff;
    }

    default:
      throw std::runtime_error{"Unknwown IN port"};
  }
}

/*------------------------------------------------------------------------------------------------*/

void
space_invaders::out(std::uint8_t port, std::uint8_t value)
noexcept
{
  switch (port)
  {
    case 2:
      shift_offset_ = value & 0x07;
      break;

    case 3:
      // play sound;
      break;

    case 4:
      shift0_ = shift1_;
      shift1_ = value;
      break;

    case 5:
      // play sound;
      break;

    case 6:
      break;

    default:
      throw std::runtime_error{"Unknwown OUT port"};
  }
}

/*------------------------------------------------------------------------------------------------*/

void
space_invaders::memory_write_byte(std::uint16_t address, std::uint8_t value)
{
  if (address < 0x2000)
  {
    auto ss = std::stringstream{};
    ss << std::hex << "Attempt to write 0x" << +value << " in ROM at 0x" << +address;
    throw std::runtime_error{ss.str()};
  }

  if (address >= 0x4000)
  {
    // Mirror RAM. What's the goal of this?
    return; // address -= 0x2000 also works;
  }

  memory_[address] = value;
}

/*------------------------------------------------------------------------------------------------*/

[[nodiscard]]
std::uint8_t
space_invaders::memory_read_byte(std::uint16_t address)
const
{
  return memory_.at(address);
}

/*------------------------------------------------------------------------------------------------*/

void
space_invaders::operator()()
{
  arcade_->render_screen(memory_);
  auto next_interrupt = std::uint8_t{0x08};

  while (process_events())
  {
    const auto now = std::chrono::high_resolution_clock::now();
    for (auto counter = 0ul, total_cycles = 0ul; total_cycles < cycles_per_frame;)
    {
      const auto cycles = cpu_.step();

      counter += cycles;
      total_cycles += cycles;

      if (counter >= (cycles_per_frame / 2))
      {
        counter -= (cycles_per_frame / 2);
        cpu_.interrupt(next_interrupt);
        next_interrupt = next_interrupt == 0x08 ? 0x10 : 0x08;
      }
    }
    arcade_->render_screen(memory_);

    const auto duration = std::chrono::high_resolution_clock::now() - now;
    // A frame lasts 1/60s.
    std::this_thread::sleep_for(std::chrono::microseconds{16666} - duration);
  }
}

/*------------------------------------------------------------------------------------------------*/

bool
space_invaders::process_events()
{
  for (auto process = true; process;)
  {
    switch (const auto [kind, event] = arcade_->get_next_event(); kind)
    {
      case kind::key_up   : key_up(event);   break;
      case kind::key_down : key_down(event); break;
      case kind::other:
        switch (event)
        {
          case event::quit : return false;
          case event::end  : process = false; break;
          default          : break;
        }
    }
  }

  return true;
}

/*------------------------------------------------------------------------------------------------*/

void
space_invaders::key_down(event k)
noexcept
{
  switch (k)
  {
    case event::coin    : port1_ |= 0x01; break;
    case event::left    : port1_ |= 0x20; break;
    case event::right   : port1_ |= 0x40; break;
    case event::fire    : port1_ |= 0x10; break;
    case event::player1 : port1_ |= 0x04; break;
    default: break;
  }
}

/*------------------------------------------------------------------------------------------------*/

void
space_invaders::key_up(event k)
noexcept
{
  switch (k)
  {
    case event::coin    : port1_ &= ~0x01; break;
    case event::left    : port1_ &= ~0x20; break;
    case event::right   : port1_ &= ~0x40; break;
    case event::fire    : port1_ &= ~0x10; break;
    case event::player1 : port1_ &= ~0x04; break;
    default: break;
  }
}

/*------------------------------------------------------------------------------------------------*/
