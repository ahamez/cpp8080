#include <algorithm>
#include <cstdint>
#include <fstream>
#include <future>
#include <iostream>
#include <istream> // istreambuf_iterator
#include <sstream>
#include <vector>

#include "cpp8080/meta/instructions.hh"
#include "cpp8080/specific/cpu.hh"
#include "cpp8080/specific/instructions.hh"
#include "cpp8080/util/concat.hh"

/*------------------------------------------------------------------------------------------------*/

class cpu_test
{
private:

  struct call
    : cpp8080::meta::describe_instruction<
        cpp8080::specific::call::opcode,
        cpp8080::specific::call::cycles,
        cpp8080::specific::call::bytes>
  {
    static constexpr auto name = "call";

    void operator()(cpp8080::specific::cpu<cpu_test>& cpu) const
    {
      if (const auto operands = cpu.operands_word(); operands == 5)
      {
        if (cpu.c == 9)
        {
          const std::uint16_t offset = cpu.de();
          auto s = std::string{};
          for (auto i = std::uint16_t{0}; ; ++i)
          {
            const auto c = cpu.memory_read_byte(offset + i);
            if (c == '$')
            {
              break;
            }
            s.push_back(c);
          }

          cpu.machine().ostream() << s << '\n';
        }
        else if (cpu.c == 2)
        {
          cpu.machine().ostream() << cpu.e;
        }
      }
      else
      {
        cpu.call(operands);
      }
    }
  };

public:

  using overrides = cpp8080::meta::make_instructions<
    call
  >;

public:

  template <typename InputIterator>
  cpu_test(InputIterator first, InputIterator last, std::ostream& os)
    : cpu_{*this}
    , memory_(65536, 0)
    , os_{os}
  {
    // Test ROMS start at 0x100.
    std::copy(first, last, memory_.begin() + 0x100);
    cpu_.jump(0x100);
  }

  void
  memory_write_byte(std::uint16_t address, std::uint8_t value)
  {
    memory_.at(address) = value;
  }

  [[nodiscard]]
  std::uint8_t
  memory_read_byte(std::uint16_t address)
  const
  {
    return memory_.at(address);
  }

  void
  operator()()
  {
    while (true)
    {
      cpu_.step();
      if (cpu_.pc() == 0x0000)
      {
        break;
      }
    }
  }

  std::ostream&
  ostream()
  noexcept
  {
    return os_;
  }

private:

  cpp8080::specific::cpu<cpu_test> cpu_;
  std::vector<std::uint8_t> memory_;
  std::ostream& os_;
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

  auto tasks = std::vector<std::packaged_task<std::string()>>{};
  auto futures = std::vector<std::future<std::string>>{};

  for (auto i = 1ul; i < argc; ++i)
  {
    const auto filename = std::string{argv[i]};

    auto& task = tasks.emplace_back([filename]() mutable
    {
      auto file = std::ifstream{filename, std::ios::binary};
      if (not file.is_open())
      {
        throw std::runtime_error{cpp8080::util::concat("Cannot open ROM file ", filename)};
      }

      auto oss = std::ostringstream{};
      auto tester = cpu_test{
        std::istreambuf_iterator<char>{file},
        std::istreambuf_iterator<char>{},
        oss
      };

      tester();
      return oss.str();
    });

    futures.emplace_back(task.get_future());
    task();
  }

  std::for_each(begin(futures), end(futures), [](auto& future)
                {
                  std::cout << future.get() << '\n';
                });
}

/*------------------------------------------------------------------------------------------------*/
