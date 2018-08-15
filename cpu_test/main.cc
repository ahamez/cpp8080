#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream> // istreambuf_iterator
#include <vector>

#include "cpp8080/meta/instructions.hh"
#include "cpp8080/specific/instructions.hh"

/*------------------------------------------------------------------------------------------------*/

struct call_adr
  : cpp8080::meta::describe_instruction<
      cpp8080::specific::call_adr::opcode,
      cpp8080::specific::call_adr::cycles,
      cpp8080::specific::call_adr::bytes>
{
  static constexpr auto name = "call_adr";

  template <typename Machine>
  void operator()(cpp8080::specific::state<Machine>& state) const
  {
    if (((state.op2() << 8) | state.op1()) == 5)
    {
      if (state.c == 9)
      {
        const std::uint16_t offset = state.de();
        auto s = std::string{};
        for (auto i = std::uint16_t{0}; ; ++i)
        {
          const auto c = state.read_memory(offset + i);
          if (c == '$')
          {
            break;
          }
          s.push_back(c);
        }

        std::cout << s << '\n';
      }
      else if (state.c == 2)
      {
        std::cout << state.e;
      }
    }
    else
    {
      cpp8080::specific::call_adr{}(state);
    }
  }
};

/*------------------------------------------------------------------------------------------------*/

class cpu_test
{
private:

  using overrides = cpp8080::meta::make_instructions<
    call_adr
  >;

public:

  using instructions = cpp8080::meta::override_instructions<
    cpp8080::specific::instructions_8080,
    overrides
  >;

public:

  template <typename InputIterator>
  cpu_test(InputIterator first, InputIterator last)
    : state_{*this}
    , memory_(65536, 0)
  {
    // Test ROMS start at 0x100.
    std::copy(first, last, memory_.begin() + 0x100);
    state_.pc = 0x100;
  }

  void
  write_memory(std::uint16_t address, std::uint8_t value)
  {
    memory_.at(address) = value;
  }

  [[nodiscard]]
  std::uint8_t
  read_memory(std::uint16_t address)
  const
  {
    return memory_.at(address);
  }

  void
  operator()()
  {
    while (true)
    {
      state_.step();
      if (state_.pc == 0)
      {
        std::cout << '\n';
        break;
      }
    }
  }

private:

  cpp8080::specific::state<cpu_test> state_;
  std::vector<std::uint8_t> memory_;
};

/*------------------------------------------------------------------------------------------------*/

int
main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Usage: " << argv[0] << " /path/to/rom_1 ... /path/to/rom_n\n";
    return 1;
  }

  for (auto i = 1ul; i < argc; ++i)
  {
    std::cout << "\n--------------------------\nTest with ROM " << argv[i] << '\n';

    auto file = std::ifstream{argv[i], std::ios::binary};
    if (not file.is_open())
    {
      std::cerr << "Cannot open ROM file " << argv[i] << '\n';
      return 1;
    }

    auto tester = cpu_test{
      std::istreambuf_iterator<char>{file},
      std::istreambuf_iterator<char>{}
    };

    tester();

    std::cout << "End of test with ROM " << argv[i] << "\n";
  }
}

/*------------------------------------------------------------------------------------------------*/
