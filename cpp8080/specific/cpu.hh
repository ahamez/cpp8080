#pragma once

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <tuple>

#include "cpp8080/meta/make_instructions.hh"
#include "cpp8080/meta/unimplemented.hh"
#include "cpp8080/specific/cpu_fwd.hh"
#include "cpp8080/specific/halt.hh"
#include "cpp8080/util/hooks.hh"
#include "cpp8080/util/parity.hh"

namespace cpp8080::specific {

/*------------------------------------------------------------------------------------------------*/

template <typename Machine>
class cpu final
{
public:

  struct flag_bits
  {
    bool cy;
    bool p;
    bool ac;
    bool z;
    bool s;
  };

public:

  std::uint8_t a;
  std::uint8_t b;
  std::uint8_t c;
  std::uint8_t d;
  std::uint8_t e;
  std::uint8_t h;
  std::uint8_t l;
  std::uint16_t sp;
  flag_bits flags;

private:

  template <std::uint8_t Opcode, auto cpu::* reg>
  struct idcr : meta::describe_instruction<Opcode, 5, 1>
  {
    void operator()(cpu& cpu) const noexcept
    {
      cpu.*reg = cpu.dcr(cpu.*reg);
    }
  };

  template <std::uint8_t Opcode, auto cpu::* reg1, auto cpu::* reg2>
  struct idcx : meta::describe_instruction<Opcode, 5, 1>
  {
    void operator()(cpu& cpu) const noexcept
    {
      cpu.*reg1 -= 1;
      if (cpu.*reg1 == 0x00ff)
      {
        cpu.*reg2 -= 1;
      }
    }
  };

  template <std::uint8_t Opcode, auto cpu::* reg>
  struct iinr : meta::describe_instruction<Opcode, 5, 1>
  {
    void operator()(cpu& cpu) const noexcept
    {
      cpu.*reg = cpu.inr(cpu.*reg);
    }
  };

  template <std::uint8_t Opcode, auto cpu::* reg1, auto cpu::* reg2>
  struct inx : meta::describe_instruction<Opcode, 5, 1>
  {
    void operator()(cpu& cpu) const noexcept
    {
      cpu.*reg1 += 1;
      if (cpu.*reg1 == 0)
      {
        cpu.*reg2 += 1;
      }
    }
  };

  template <std::uint8_t Opcode, auto cpu::* reg1, auto cpu::* reg2>
  struct lxi : meta::describe_instruction<Opcode, 10, 3>
  {
    void operator()(cpu& cpu) const
    {
      std::tie(cpu.*reg1, cpu.*reg2) = cpu.operands();
    }
  };

  template <std::uint8_t Opcode, auto cpu::* reg1, auto cpu::* reg2>
  struct mov : meta::describe_instruction<Opcode, 5, 1>
  {
    void operator()(cpu& cpu) const noexcept
    {
      cpu.*reg1 = cpu.*reg2;
    }
  };

private:

  struct nop : meta::describe_instruction<0x00, 4, 1>
  {
    static constexpr auto name = "nop";

    void operator()(cpu&) const noexcept
    {}
  };

  struct lxi_b : lxi<0x01, &cpu::c, &cpu::b>
  {
    static constexpr auto name = "lxi_b";
  };

  struct stax_b : meta::describe_instruction<0x02, 7, 1>
  {
    static constexpr auto name = "stax_b";

    void operator()(cpu& cpu) const
    {
      cpu.memory_write_byte(cpu.bc(), cpu.a);
    }
  };

  struct inx_b : inx<0x03, &cpu::c, &cpu::b>
  {
    static constexpr auto name = "inx_b";
  };

  struct inr_b : iinr<0x04, &cpu::b>
  {
    static constexpr auto name = "inr_b";
  };

  struct dcr_b : idcr<0x05, &cpu::b>
  {
    static constexpr auto name = "dcr_b";
  };

  struct mvi_b : meta::describe_instruction<0x06, 7, 2>
  {
    static constexpr auto name = "mvi_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.b = cpu.op1();
    }
  };

  struct rlc : meta::describe_instruction<0x07, 4, 1>
  {
    static constexpr auto name = "rlc";

    void operator()(cpu& cpu) const noexcept
    {
      const auto x = cpu.a;
      cpu.a = ((x & 0x80) >> 7) | (x << 1);
      cpu.flags.cy = (0x80 == (x & 0x80));
    }
  };

  struct dad_b : meta::describe_instruction<0x09, 10, 1>
  {
    static constexpr auto name = "dad_b";

    void operator()(cpu& cpu) const noexcept
    {
      const std::uint32_t res = cpu.hl() + cpu.bc();
      cpu.h = (res & 0xff00) >> 8;
      cpu.l = res & 0x00ff;
      cpu.flags.cy = ((res & 0xffff0000) != 0);
    }
  };

  struct ldax_b : meta::describe_instruction<0x0a, 7, 1>
  {
    static constexpr auto name = "ldax_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.a = cpu.memory_read_byte(cpu.bc());
    }
  };

  struct dcx_b : idcx<0x0b, &cpu::c, &cpu::b>
  {
    static constexpr auto name = "dcx_b";
  };

  struct inr_c : iinr<0x0c, &cpu::c>
  {
    static constexpr auto name = "inr_c";
  };

  struct dcr_c : idcr<0x0d, &cpu::c>
  {
    static constexpr auto name = "dcr_b";
  };

  struct mvi_c : meta::describe_instruction<0x0e, 7, 2>
  {
    static constexpr auto name = "mvi_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.c = cpu.op1();
    }
  };

  struct rrc : meta::describe_instruction<0x0f, 4, 1>
  {
    static constexpr auto name = "rrc";

    void operator()(cpu& cpu) const noexcept
    {
      const auto x = cpu.a;
      cpu.a = ((x & 1) << 7) | (x >> 1);
      cpu.flags.cy = (1 == (x & 1));
    }
  };

  struct lxi_d : lxi<0x11, &cpu::e, &cpu::d>
  {
    static constexpr auto name = "lxi_d";
  };

  struct stax_d : meta::describe_instruction<0x12, 7, 1>
  {
    static constexpr auto name = "stax_d";

    void operator()(cpu& cpu) const
    {
      cpu.memory_write_byte(cpu.de(), cpu.a);
    }
  };

  struct inx_d : inx<0x13, &cpu::e, &cpu::d>
  {
    static constexpr auto name = "inx_b";
  };

  struct inr_d : iinr<0x14, &cpu::d>
  {
    static constexpr auto name = "inr_d";
  };

  struct dcr_d : idcr<0x15, &cpu::d>
  {
    static constexpr auto name = "dcr_b";
  };

  struct mvi_d : meta::describe_instruction<0x16, 7, 2>
  {
    static constexpr auto name = "mvi_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.d = cpu.op1();
    }
  };

  struct ral : meta::describe_instruction<0x17, 4, 1>
  {
    static constexpr auto name = "ral";

    void operator()(cpu& cpu) const noexcept
    {
      const auto a = cpu.a;
      cpu.a = cpu.flags.cy | (a << 1);
      cpu.flags.cy = (0x80 == (a & 0x80));
    }
  };

  struct dad_d : meta::describe_instruction<0x19, 10, 1>
  {
    static constexpr auto name = "dad_d";

    void operator()(cpu& cpu) const noexcept
    {
      const std::uint32_t res = cpu.hl() + cpu.de();
      cpu.h = (res & 0xff00) >> 8;
      cpu.l = res & 0x00ff;
      cpu.flags.cy = ((res & 0xffff0000) != 0);
    }
  };

  struct ldax_d : meta::describe_instruction<0x1a, 7, 1>
  {
    static constexpr auto name = "ldax_d";

    void operator()(cpu& cpu) const
    {
      cpu.a = cpu.memory_read_byte(cpu.de());
    }
  };

  struct dcx_d : idcx<0x1b, &cpu::e, &cpu::d>
  {
    static constexpr auto name = "dcx_d";
  };

  struct inr_e : iinr<0x1c, &cpu::e>
  {
    static constexpr auto name = "inr_e";
  };

  struct dcr_e : idcr<0x1d, &cpu::e>
  {
    static constexpr auto name = "dcr_b";
  };

  struct mvi_e : meta::describe_instruction<0x1e, 7, 2>
  {
    static constexpr auto name = "mvi_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.e = cpu.op1();
    }
  };

  struct rar : meta::describe_instruction<0x1f, 4, 1>
  {
    static constexpr auto name = "rar";

    void operator()(cpu& cpu) const noexcept
    {
      const auto a = cpu.a;
      cpu.a = (cpu.flags.cy << 7) | (a >> 1);
      cpu.flags.cy = (1 == (a & 1));
    }
  };

  struct lxi_h : lxi<0x21, &cpu::l, &cpu::h>
  {
    static constexpr auto name = "lxi_b";
  };

  struct shld : meta::describe_instruction<0x22, 16, 3>
  {
    static constexpr auto name = "shld";

    void operator()(cpu& cpu) const
    {
      const std::uint16_t offset = cpu.operands_word();
      cpu.memory_write_byte(offset, cpu.l);
      cpu.memory_write_byte(offset + 1, cpu.h);
    }
  };

  struct inx_h : inx<0x23, &cpu::l, &cpu::h>
  {
    static constexpr auto name = "inx_h";
  };

  struct inr_h : iinr<0x24, &cpu::h>
  {
    static constexpr auto name = "inr_h";
  };

  struct dcr_h : idcr<0x25, &cpu::h>
  {
    static constexpr auto name = "dcr_h";
  };

  struct mvi_h : meta::describe_instruction<0x26, 7, 2>
  {
    static constexpr auto name = "mvi_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.h = cpu.op1();
    }
  };

  struct daa : meta::describe_instruction<0x27, 4, 1>
  {
    static constexpr auto name = "daa";

    void operator()(cpu& cpu) const noexcept
    {
      auto cy = cpu.flags.cy;
      auto value_to_add = std::uint8_t{0};

      const std::uint8_t lsb = cpu.a & 0x0f;
      const std::uint8_t msb = cpu.a >> 4;

      if (cpu.flags.ac or lsb > 9)
      {
        value_to_add += 0x06;
      }
      if (cpu.flags.cy or msb > 9 or (msb >= 9 and lsb > 9))
      {
        value_to_add += 0x60;
        cy = true;
      }
      cpu.adda(value_to_add, 0);
      cpu.flags.p = util::parity(cpu.a);
      cpu.flags.cy = cy;

    }
  };

  struct dad_h : meta::describe_instruction<0x29, 10, 1>
  {
    static constexpr auto name = "dad_h";

    void operator()(cpu& cpu) const noexcept
    {
      const std::uint32_t res = 2 * cpu.hl();
      cpu.h = (res & 0xff00) >> 8;
      cpu.l = res & 0x00ff;
      cpu.flags.cy = ((res & 0xffff0000) != 0);
    }
  };

  struct lhld : meta::describe_instruction<0x2a, 16, 3>
  {
    static constexpr auto name = "lhld";

    void operator()(cpu& cpu) const
    {
      const std::uint16_t offset = cpu.operands_word();
      cpu.l = cpu.memory_read_byte(offset);
      cpu.h = cpu.memory_read_byte(offset + 1);
    }
  };

  struct dcx_h : idcx<0x2b, &cpu::l, &cpu::h>
  {
    static constexpr auto name = "dcx_b";
  };

  struct inr_l : iinr<0x2c, &cpu::l>
  {
    static constexpr auto name = "inr_l";
  };

  struct dcr_l : idcr<0x2d, &cpu::l>
  {
    static constexpr auto name = "dcr_l";
  };

  struct mvi_l_d8 : meta::describe_instruction<0x2e, 7, 2>
  {
    static constexpr auto name = "mvi_l";

    void operator()(cpu& cpu) const
    {
      cpu.l = cpu.op1();
    }
  };

  struct cma : meta::describe_instruction<0x2f, 4, 1>
  {
    static constexpr auto name = "cma";

    void operator()(cpu& cpu) const
    {
      cpu.a ^= 0xff;
    }
  };

  struct lxi_sp : meta::describe_instruction<0x31, 10, 3>
  {
    static constexpr auto name = "lxi_sp";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.sp = cpu.operands_word();
    }
  };

  struct sta : meta::describe_instruction<0x32, 13, 3>
  {
    static constexpr auto name = "sta";

    void operator()(cpu& cpu) const
    {
      cpu.memory_write_byte(cpu.operands_word(), cpu.a);
    }
  };

  struct inx_sp : meta::describe_instruction<0x33, 5, 1>
  {
    static constexpr auto name = "inx_sp";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.sp += 1;
    }
  };

  struct inr_m : meta::describe_instruction<0x34, 10, 1>
  {
    static constexpr auto name = "inr_m";

    void operator()(cpu& cpu) const
    {
      const auto res = cpu.inr(cpu.read_hl());
      cpu.write_hl(res);
    }
  };

  struct dcr_m : meta::describe_instruction<0x35, 10, 1>
  {
    static constexpr auto name = "dcr_m";

    void operator()(cpu& cpu) const
    {
      const auto res = cpu.dcr(cpu.read_hl());
      cpu.write_hl(res);
    }
  };

  struct mvi_m : meta::describe_instruction<0x36, 10, 2>
  {
    static constexpr auto name = "mvi_m";

    void operator()(cpu& cpu) const
    {
      cpu.write_hl(cpu.op1());
    }
  };

  struct stc : meta::describe_instruction<0x37, 4, 1>
  {
    static constexpr auto name = "stc";

    void operator()(cpu& cpu) const
    {
      cpu.flags.cy = true;
    }
  };

  struct dad_sp : meta::describe_instruction<0x39, 10, 1>
  {
    static constexpr auto name = "dad_sp";

    void operator()(cpu& cpu) const
    {
      const std::uint32_t res = cpu.hl() + cpu.sp;
      cpu.h = (res & 0xff00) >> 8;
      cpu.l = res & 0x00ff;
      cpu.flags.cy = ((res & 0xffff0000) > 0);
    }
  };

  struct lda : meta::describe_instruction<0x3a, 13, 3>
  {
    static constexpr auto name = "lda";

    void operator()(cpu& cpu) const
    {
      const std::uint16_t offset = cpu.operands_word();
      cpu.a = cpu.memory_read_byte(offset);
    }
  };

  struct dcx_sp : meta::describe_instruction<0x3b, 5, 1>
  {
    static constexpr auto name = "dcx_sp";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.sp -= 1;
    }
  };

  struct inr_a : meta::describe_instruction<0x3c, 5, 1>
  {
    static constexpr auto name = "inr_a";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.a = cpu.inr(cpu.a);
    }
  };

  struct dcr_a : idcr<0x3d, &cpu::a>
  {
    static constexpr auto name = "dcr_a";
  };

  struct mvi_a : meta::describe_instruction<0x3e, 7, 2>
  {
    static constexpr auto name = "mvi_a";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.a = cpu.op1();
    }
  };

  struct cmc : meta::describe_instruction<0x3f, 4, 1>
  {
    static constexpr auto name = "cmc";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.flags.cy = not(cpu.flags.cy);
    }
  };

  struct mov_b_b : mov<0x40, &cpu::b, &cpu::b>
  {
    static constexpr auto name = "mov_b_b";
  };

  struct mov_b_c : mov<0x41, &cpu::b, &cpu::c>
  {
    static constexpr auto name = "mov_b_c";
  };

  struct mov_b_d : mov<0x42, &cpu::b, &cpu::d>
  {
    static constexpr auto name = "mov_b_d";
  };

  struct mov_b_e : mov<0x43, &cpu::b, &cpu::e>
  {
    static constexpr auto name = "mov_b_e";
  };

  struct mov_b_h : mov<0x44, &cpu::b, &cpu::h>
  {
    static constexpr auto name = "mov_b_h";
  };

  struct mov_b_l : mov<0x45, &cpu::b, &cpu::l>
  {
    static constexpr auto name = "mov_b_l";
  };

  struct mov_b_m : meta::describe_instruction<0x46, 7, 1>
  {
    static constexpr auto name = "mov_b_m";

    void operator()(cpu& cpu) const
    {
      cpu.b = cpu.read_hl();
    }
  };

  struct mov_b_a : mov<0x47, &cpu::b, &cpu::a>
  {
    static constexpr auto name = "mov_b_a";
  };

  struct mov_c_b : mov<0x48, &cpu::c, &cpu::b>
  {
    static constexpr auto name = "mov_c_b";
  };

  struct mov_c_c : mov<0x49, &cpu::c, &cpu::c>
  {
    static constexpr auto name = "mov_c_c";
  };

  struct mov_c_d : mov<0x4a, &cpu::c, &cpu::d>
  {
    static constexpr auto name = "mov_c_d";
  };

  struct mov_c_e : mov<0x4b, &cpu::c, &cpu::e>
  {
    static constexpr auto name = "mov_c_e";
  };

  struct mov_c_h : mov<0x4c, &cpu::c, &cpu::h>
  {
    static constexpr auto name = "mov_c_h";
  };

  struct mov_c_l : mov<0x4d, &cpu::c, &cpu::l>
  {
    static constexpr auto name = "mov_c_l";
  };

  struct mov_c_m : meta::describe_instruction<0x4e, 7, 1>
  {
    static constexpr auto name = "mov_c_m";

    void operator()(cpu& cpu) const
    {
      cpu.c = cpu.read_hl();
    }
  };

  struct mov_c_a : mov<0x4f, &cpu::c, &cpu::a>
  {
    static constexpr auto name = "mov_c_a";
  };

  struct mov_d_b : mov<0x50, &cpu::d, &cpu::b>
  {
    static constexpr auto name = "mov_d_b";
  };

  struct mov_d_c : mov<0x51, &cpu::d, &cpu::c>
  {
    static constexpr auto name = "mov_d_c";
  };

  struct mov_d_d : mov<0x52, &cpu::b, &cpu::b>
  {
    static constexpr auto name = "mov_d_d";
  };

  struct mov_d_e : mov<0x53, &cpu::d, &cpu::e>
  {
    static constexpr auto name = "mov_d_e";
  };

  struct mov_d_h : mov<0x54, &cpu::d, &cpu::h>
  {
    static constexpr auto name = "mov_d_h";
  };

  struct mov_d_l : mov<0x55, &cpu::d, &cpu::l>
  {
    static constexpr auto name = "mov_d_l";
  };

  struct mov_d_m : meta::describe_instruction<0x56, 7, 1>
  {
    static constexpr auto name = "mov_d_m";

    void operator()(cpu& cpu) const
    {
      cpu.d = cpu.read_hl();
    }
  };

  struct mov_d_a : mov<0x57, &cpu::d, &cpu::a>
  {
    static constexpr auto name = "mov_d_a";
  };

  struct mov_e_b : mov<0x58, &cpu::e, &cpu::b>
  {
    static constexpr auto name = "mov_e_b";
  };

  struct mov_e_c : mov<0x59, &cpu::e, &cpu::c>
  {
    static constexpr auto name = "mov_e_c";
  };

  struct mov_e_d : mov<0x5a, &cpu::e, &cpu::d>
  {
    static constexpr auto name = "mov_e_d";
  };

  struct mov_e_e : mov<0x5b, &cpu::e, &cpu::e>
  {
    static constexpr auto name = "mov_e_e";
  };

  struct mov_e_h : mov<0x5c, &cpu::e, &cpu::h>
  {
    static constexpr auto name = "mov_e_h";
  };

  struct mov_e_l : mov<0x5d, &cpu::e, &cpu::l>
  {
    static constexpr auto name = "mov_e_l";
  };

  struct mov_e_m : meta::describe_instruction<0x5e, 7, 1>
  {
    static constexpr auto name = "mov_e_m";

    void operator()(cpu& cpu) const
    {
      cpu.e = cpu.read_hl();
    }
  };

  struct mov_e_a : mov<0x5f, &cpu::e, &cpu::a>
  {
    static constexpr auto name = "mov_e_a";
  };

  struct mov_h_b : mov<0x60, &cpu::h, &cpu::b>
  {
    static constexpr auto name = "mov_h_b";
  };

  struct mov_h_c : mov<0x61, &cpu::h, &cpu::c>
  {
    static constexpr auto name = "mov_h_c";
  };

  struct mov_h_d : mov<0x62, &cpu::h, &cpu::d>
  {
    static constexpr auto name = "mov_h_d";
  };

  struct mov_h_e : mov<0x63, &cpu::h, &cpu::e>
  {
    static constexpr auto name = "mov_h_e";
  };

  struct mov_h_h : mov<0x64, &cpu::h, &cpu::h>
  {
    static constexpr auto name = "mov_h_h";
  };

  struct mov_h_l : mov<0x65, &cpu::h, &cpu::l>
  {
    static constexpr auto name = "mov_h_l";
  };

  struct mov_h_m : meta::describe_instruction<0x66, 7, 1>
  {
    static constexpr auto name = "mov_h_m";

    void operator()(cpu& cpu) const
    {
      cpu.h = cpu.read_hl();
    }
  };

  struct mov_h_a : mov<0x67, &cpu::h, &cpu::a>
  {
    static constexpr auto name = "mov_h_a";
  };

  struct mov_l_b : mov<0x68, &cpu::l, &cpu::b>
  {
    static constexpr auto name = "mov_l_b";
  };

  struct mov_l_c : mov<0x69, &cpu::l, &cpu::c>
  {
    static constexpr auto name = "mov_l_c";
  };

  struct mov_l_d : mov<0x6a, &cpu::l, &cpu::d>
  {
    static constexpr auto name = "mov_l_d";
  };

  struct mov_l_e : mov<0x6b, &cpu::l, &cpu::e>
  {
    static constexpr auto name = "mov_l_e";
  };

  struct mov_l_h : mov<0x6c, &cpu::l, &cpu::h>
  {
    static constexpr auto name = "mov_l_h";
  };

  struct mov_l_l : mov<0x40, &cpu::l, &cpu::l>
  {
    static constexpr auto name = "mov_l_l";
  };

  struct mov_l_m : meta::describe_instruction<0x6e, 7, 1>
  {
    static constexpr auto name = "mov_l_m";

    void operator()(cpu& cpu) const
    {
      cpu.l = cpu.read_hl();
    }
  };

  struct mov_l_a : mov<0x6f, &cpu::l, &cpu::a>
  {
    static constexpr auto name = "mov_l_a";
  };

  struct mov_m_b : meta::describe_instruction<0x70, 7, 1>
  {
    static constexpr auto name = "mov_m_b";

    void operator()(cpu& cpu) const
    {
      cpu.write_hl(cpu.b);
    }
  };

  struct mov_m_c : meta::describe_instruction<0x71, 7, 1>
  {
    static constexpr auto name = "mov_m_c";

    void operator()(cpu& cpu) const
    {
      cpu.write_hl(cpu.c);
    }
  };

  struct mov_m_d : meta::describe_instruction<0x72, 7, 1>
  {
    static constexpr auto name = "mov_m_d";

    void operator()(cpu& cpu) const
    {
      cpu.write_hl(cpu.d);
    }
  };

  struct mov_m_e : meta::describe_instruction<0x73, 7, 1>
  {
    static constexpr auto name = "mov_m_e";

    void operator()(cpu& cpu) const
    {
      cpu.write_hl(cpu.e);
    }
  };

  struct mov_m_h : meta::describe_instruction<0x74, 7, 1>
  {
    static constexpr auto name = "mov_m_h";

    void operator()(cpu& cpu) const
    {
      cpu.write_hl(cpu.h);
    }
  };

  struct mov_m_l : meta::describe_instruction<0x75, 7, 1>
  {
    static constexpr auto name = "mov_m_l";

    void operator()(cpu& cpu) const
    {
      cpu.write_hl(cpu.l);
    }
  };

  struct hlt : meta::describe_instruction<0x76, 7, 1>
  {
    static constexpr auto name = "hlt";

    void operator()(cpu&) const
    {
      throw halt{"HLT"};
    }
  };

  struct mov_m_a : meta::describe_instruction<0x77, 7, 1>
  {
    static constexpr auto name = "mov_m_a";

    void operator()(cpu& cpu) const
    {
      cpu.write_hl(cpu.a);
    }
  };

  struct mov_a_b : mov<0x78, &cpu::a, &cpu::b>
  {
    static constexpr auto name = "mov_a_b";
  };

  struct mov_a_c : mov<0x79, &cpu::a, &cpu::c>
  {
    static constexpr auto name = "mov_a_c";
  };

  struct mov_a_d : mov<0x7a, &cpu::a, &cpu::d>
  {
    static constexpr auto name = "mov_a_d";
  };

  struct mov_a_e : mov<0x7b, &cpu::a, &cpu::e>
  {
    static constexpr auto name = "mov_a_e";
  };

  struct mov_a_h : mov<0x7c, &cpu::a, &cpu::h>
  {
    static constexpr auto name = "mov_a_h";
  };

  struct mov_a_l : mov<0x7d, &cpu::a, &cpu::l>
  {
    static constexpr auto name = "mov_a_l";
  };

  struct mov_a_m : meta::describe_instruction<0x7e, 7, 1>
  {
    static constexpr auto name = "mov_a_m";

    void operator()(cpu& cpu) const
    {
      cpu.a = cpu.read_hl();
    }
  };

  struct mov_a_a : mov<0x7f, &cpu::a, &cpu::a>
  {
    static constexpr auto name = "mov_a_a";
  };

  struct add_b : meta::describe_instruction<0x80, 4, 1>
  {
    static constexpr auto name = "add_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.b, 0);
    }
  };

  struct add_c : meta::describe_instruction<0x81, 4, 1>
  {
    static constexpr auto name = "add_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.c, 0);
    }
  };

  struct add_d : meta::describe_instruction<0x82, 4, 1>
  {
    static constexpr auto name = "add_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.d, 0);
    }
  };

  struct add_e : meta::describe_instruction<0x83, 4, 1>
  {
    static constexpr auto name = "add_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.e, 0);
    }
  };

  struct add_h : meta::describe_instruction<0x84, 4, 1>
  {
    static constexpr auto name = "add_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.h, 0);
    }
  };

  struct add_l : meta::describe_instruction<0x85, 4, 1>
  {
    static constexpr auto name = "add_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.l, 0);
    }
  };

  struct add_m : meta::describe_instruction<0x86, 7, 1>
  {
    static constexpr auto name = "add_m";

    void operator()(cpu& cpu) const
    {
      cpu.adda(cpu.read_hl(), 0);
    }
  };

  struct add_a : meta::describe_instruction<0x87, 4, 1>
  {
    static constexpr auto name = "add_a";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.a, 0);
    }
  };

  struct adc_b : meta::describe_instruction<0x88, 4, 1>
  {
    static constexpr auto name = "adc_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.b, cpu.flags.cy);
    }
  };

  struct adc_c : meta::describe_instruction<0x89, 4, 1>
  {
    static constexpr auto name = "adc_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.c, cpu.flags.cy);
    }
  };

  struct adc_d : meta::describe_instruction<0x8a, 4, 1>
  {
    static constexpr auto name = "adc_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.d, cpu.flags.cy);
    }
  };

  struct adc_e : meta::describe_instruction<0x8b, 4, 1>
  {
    static constexpr auto name = "adc_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.e, cpu.flags.cy);
    }
  };

  struct adc_h : meta::describe_instruction<0x8c, 4, 1>
  {
    static constexpr auto name = "adc_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.h, cpu.flags.cy);
    }
  };

  struct adc_l : meta::describe_instruction<0x8d, 4, 1>
  {
    static constexpr auto name = "adc_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.l, cpu.flags.cy);
    }
  };

  struct adc_m : meta::describe_instruction<0x8e, 7, 1>
  {
    static constexpr auto name = "adc_m";

    void operator()(cpu& cpu) const
    {
      cpu.adda(cpu.read_hl(), cpu.flags.cy);
    }
  };

  struct adc_a : meta::describe_instruction<0x8f, 4, 1>
  {
    static constexpr auto name = "adc_a";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.a, cpu.flags.cy);
    }
  };

  struct sub_b : meta::describe_instruction<0x90, 4, 1>
  {
    static constexpr auto name = "sub_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.b, 0);
    }
  };

  struct sub_c : meta::describe_instruction<0x91, 4, 1>
  {
    static constexpr auto name = "sub_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.c, 0);
    }
  };

  struct sub_d : meta::describe_instruction<0x92, 4, 1>
  {
    static constexpr auto name = "sub_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.d, 0);
    }
  };

  struct sub_e : meta::describe_instruction<0x93, 4, 1>
  {
    static constexpr auto name = "sub_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.e, 0);
    }
  };

  struct sub_h : meta::describe_instruction<0x94, 4, 1>
  {
    static constexpr auto name = "sub_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.h, 0);
    }
  };

  struct sub_l : meta::describe_instruction<0x95, 4, 1>
  {
    static constexpr auto name = "sub_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.l, 0);
    }
  };

  struct sub_m : meta::describe_instruction<0x96, 7, 1>
  {
    static constexpr auto name = "sub_m";

    void operator()(cpu& cpu) const
    {
      cpu.suba(cpu.read_hl(), 0);
    }
  };

  struct sub_a : meta::describe_instruction<0x97, 4, 1>
  {
    static constexpr auto name = "sub_a";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.a, 0);
    }
  };

  struct sbb_b : meta::describe_instruction<0x98, 4, 1>
  {
    static constexpr auto name = "sbb_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.b, cpu.flags.cy);
    }
  };

  struct sbb_c : meta::describe_instruction<0x99, 4, 1>
  {
    static constexpr auto name = "sbb_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.c, cpu.flags.cy);
    }
  };

  struct sbb_d : meta::describe_instruction<0x9a, 4, 1>
  {
    static constexpr auto name = "sbb_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.d, cpu.flags.cy);
    }
  };

  struct sbb_e : meta::describe_instruction<0x9b, 4, 1>
  {
    static constexpr auto name = "sbb_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.e, cpu.flags.cy);
    }
  };

  struct sbb_h : meta::describe_instruction<0x9c, 4, 1>
  {
    static constexpr auto name = "sbb_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.h, cpu.flags.cy);
    }
  };

  struct sbb_l : meta::describe_instruction<0x9d, 4, 1>
  {
    static constexpr auto name = "sbb_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.l, cpu.flags.cy);
    }
  };

  struct sbb_m : meta::describe_instruction<0x9e, 7, 1>
  {
    static constexpr auto name = "sbb_m";

    void operator()(cpu& cpu) const
    {
      cpu.suba(cpu.read_hl(), cpu.flags.cy);
    }
  };

  struct sbb_a : meta::describe_instruction<0x9f, 4, 1>
  {
    static constexpr auto name = "sbb_a";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.a, cpu.flags.cy);
    }
  };

  struct ana_b : meta::describe_instruction<0xa0, 4, 1>
  {
    static constexpr auto name = "ana_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ana(cpu.b);
    }
  };

  struct ana_c : meta::describe_instruction<0xa1, 4, 1>
  {
    static constexpr auto name = "ana_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ana(cpu.c);
    }
  };

  struct ana_d : meta::describe_instruction<0xa2, 4, 1>
  {
    static constexpr auto name = "ana_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ana(cpu.d);
    }
  };

  struct ana_e : meta::describe_instruction<0xa3, 4, 1>
  {
    static constexpr auto name = "ana_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ana(cpu.e);
    }
  };

  struct ana_h : meta::describe_instruction<0xa4, 4, 1>
  {
    static constexpr auto name = "ana_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ana(cpu.h);
    }
  };

  struct ana_l : meta::describe_instruction<0xa5, 4, 1>
  {
    static constexpr auto name = "ana_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ana(cpu.l);
    }
  };

  struct ana_m : meta::describe_instruction<0xa6, 7, 1>
  {
    static constexpr auto name = "ana_m";

    void operator()(cpu& cpu) const
    {
      cpu.ana(cpu.read_hl());
    }
  };

  struct ana_a : meta::describe_instruction<0xa7, 4, 1>
  {
    static constexpr auto name = "ana_a";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ana(cpu.a);
    }
  };

  struct xra_b : meta::describe_instruction<0xa8, 4, 1>
  {
    static constexpr auto name = "xra_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.b);
    }
  };

  struct xra_c : meta::describe_instruction<0xa9, 4, 1>
  {
    static constexpr auto name = "xra_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.c);
    }
  };

  struct xra_d : meta::describe_instruction<0xaa, 4, 1>
  {
    static constexpr auto name = "xra_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.d);
    }
  };

  struct xra_e : meta::describe_instruction<0xab, 4, 1>
  {
    static constexpr auto name = "xra_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.e);
    }
  };

  struct xra_h : meta::describe_instruction<0xac, 4, 1>
  {
    static constexpr auto name = "xra_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.h);
    }
  };

  struct xra_l : meta::describe_instruction<0xad, 4, 1>
  {
    static constexpr auto name = "xra_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.l);
    }
  };

  struct xra_m : meta::describe_instruction<0xae, 7, 1>
  {
    static constexpr auto name = "xra_m";

    void operator()(cpu& cpu) const
    {
      cpu.xra(cpu.read_hl());
    }
  };

  struct xra_a : meta::describe_instruction<0xaf, 4, 1>
  {
    static constexpr auto name = "xra_a";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.a);
    }
  };

  struct ora_b : meta::describe_instruction<0xb0, 4, 1>
  {
    static constexpr auto name = "ora_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.b);
    }
  };

  struct ora_c : meta::describe_instruction<0xb1, 4, 1>
  {
    static constexpr auto name = "ora_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.c);
    }
  };

  struct ora_d : meta::describe_instruction<0xb2, 4, 1>
  {
    static constexpr auto name = "ora_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.d);
    }
  };

  struct ora_e : meta::describe_instruction<0xb3, 4, 1>
  {
    static constexpr auto name = "ora_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.e);
    }
  };

  struct ora_h : meta::describe_instruction<0xb4, 4, 1>
  {
    static constexpr auto name = "ora_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.h);
    }
  };

  struct ora_l : meta::describe_instruction<0xb5, 4, 1>
  {
    static constexpr auto name = "ora_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.l);
    }
  };

  struct ora_m : meta::describe_instruction<0xb6, 7, 1>
  {
    static constexpr auto name = "ora_m";

    void operator()(cpu& cpu) const
    {
      cpu.ora(cpu.read_hl());
    }
  };

  struct ora_a : meta::describe_instruction<0xb7, 4, 1>
  {
    static constexpr auto name = "ora_a";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.a);
    }
  };

  struct cmp_b : meta::describe_instruction<0xb8, 4, 1>
  {
    static constexpr auto name = "cmp_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.b);
    }
  };

  struct cmp_c : meta::describe_instruction<0xb9, 4, 1>
  {
    static constexpr auto name = "cmp_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.c);
    }
  };

  struct cmp_d : meta::describe_instruction<0xba, 4, 1>
  {
    static constexpr auto name = "cmp_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.d);
    }
  };

  struct cmp_e : meta::describe_instruction<0xbb, 4, 1>
  {
    static constexpr auto name = "cmp_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.e);
    }
  };

  struct cmp_h : meta::describe_instruction<0xbc, 4, 1>
  {
    static constexpr auto name = "cmp_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.h);
    }
  };

  struct cmp_l : meta::describe_instruction<0xbd, 4, 1>
  {
    static constexpr auto name = "cmp_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.l);
    }
  };

  struct cmp_m : meta::describe_instruction<0xbe, 7, 1>
  {
    static constexpr auto name = "cmp_m";

    void operator()(cpu& cpu) const
    {
      cpu.cmp(cpu.read_hl());
    }
  };

  struct cmp_a : meta::describe_instruction<0xbf, 4, 1>
  {
    static constexpr auto name = "cmp_a";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.a);
    }
  };

  struct rnz : meta::describe_instruction<0xc0, 5, 1, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "rnz";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_ret(not cpu.flags.z);
    }
  };

  struct pop_b : meta::describe_instruction<0xc1, 10, 1>
  {
    static constexpr auto name = "pop_b";

    void operator()(cpu& cpu) const
    {
      std::tie(cpu.b, cpu.c) = cpu.pop();
    }
  };

  struct jnz : meta::describe_instruction<0xc2, 10, 3>
  {
    static constexpr auto name = "jnz";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.conditional_jump(cpu.operands_word(), not cpu.flags.z);
    }
  };

  struct jmp : meta::describe_instruction<0xc3, 10, 3>
  {
    static constexpr auto name = "jmp";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.jump(cpu.operands_word());
    }
  };

  struct cnz : meta::describe_instruction<0xc4, 11, 3, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "cnz";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_call(cpu.operands_word(), not cpu.flags.z);
    }
  };

  struct push_b : meta::describe_instruction<0xc5, 11, 1>
  {
    static constexpr auto name = "push_b";

    void operator()(cpu& cpu) const
    {
      cpu.push(cpu.b, cpu.c);
    }
  };

  struct adi : meta::describe_instruction<0xc6, 7, 2>
  {
    static constexpr auto name = "adi";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.op1(), 0);
    }
  };

  struct rst_0 : meta::describe_instruction<0xc7, 11, 1>
  {
    static constexpr auto name = "rst_0";

    void operator()(cpu& cpu) const
    {
      cpu.call(0x0000);
    }
  };

  struct rz : meta::describe_instruction<0xc8, 5, 1, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "rz";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_ret(cpu.flags.z);
    }
  };

  struct ret : meta::describe_instruction<0xc9, 10, 1>
  {
    static constexpr auto name = "ret";

    void operator()(cpu& cpu) const
    {
      cpu.ret();
    }
  };

  struct jz : meta::describe_instruction<0xca, 10, 3>
  {
    static constexpr auto name = "jz";

    void operator()(cpu& cpu) const
    {
      cpu.conditional_jump(cpu.operands_word(), cpu.flags.z);
    }
  };

  struct cz : meta::describe_instruction<0xcc, 11, 3, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "cz";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_call(cpu.operands_word(), cpu.flags.z);
    }
  };

  struct call : meta::describe_instruction<0xcd, 11, 3>
  {
    static constexpr auto name = "call";

    auto operator()(cpu& cpu) const
    {
      return cpu.call(cpu.operands_word());
    }
  };

  struct aci : meta::describe_instruction<0xce, 7, 2>
  {
    static constexpr auto name = "aci";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.op1(), cpu.flags.cy);
    }
  };

  struct rst_1 : meta::describe_instruction<0xcf, 11, 1>
  {
    static constexpr auto name = "rst_1";

    void operator()(cpu& cpu) const
    {
      cpu.call(0x0008);
    }
  };

  struct rnc : meta::describe_instruction<0xd0, 5, 1, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "rnc";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_ret(not cpu.flags.cy);
    }
  };

  struct pop_d : meta::describe_instruction<0xd1, 10, 1>
  {
    static constexpr auto name = "pop_d";

    void operator()(cpu& cpu) const
    {
      std::tie(cpu.d, cpu.e) = cpu.pop();
    }
  };

  struct jnc : meta::describe_instruction<0xd2, 10, 3>
  {
    static constexpr auto name = "jnc";

    void operator()(cpu& cpu) const
    {
      cpu.conditional_jump(cpu.operands_word(), not cpu.flags.cy);
    }
  };

  struct out : meta::describe_instruction<0xd3, 10, 2>
  {
    static constexpr auto name = "out";

    void operator()(cpu& cpu) const noexcept
    {}
  };

  struct cnc : meta::describe_instruction<0xd4, 11, 3, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "cnc";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_call(cpu.operands_word(), not cpu.flags.cy);
    }
  };

  struct push_d : meta::describe_instruction<0xd5, 11, 1>
  {
    static constexpr auto name = "push_d";

    void operator()(cpu& cpu) const
    {
      cpu.push(cpu.d, cpu.e);
    }
  };

  struct sui : meta::describe_instruction<0xd6, 7, 2>
  {
    static constexpr auto name = "sui";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.op1(), 0);
    }
  };

  struct rst_2 : meta::describe_instruction<0xd7, 11, 1>
  {
    static constexpr auto name = "rst_2";

    void operator()(cpu& cpu) const
    {
      cpu.call(0x0010);
    }
  };

  struct rc : meta::describe_instruction<0xd8, 5, 1, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "rc";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_ret(cpu.flags.cy);
    }
  };

  struct jc : meta::describe_instruction<0xda, 10, 3>
  {
    static constexpr auto name = "jc";

    void operator()(cpu& cpu) const
    {
      cpu.conditional_jump(cpu.operands_word(), cpu.flags.cy);
    }
  };

  struct in : meta::describe_instruction<0xdb, 10, 2>
  {
    static constexpr auto name = "in";

    void operator()(cpu& cpu) const noexcept
    {}
  };

  struct cc : meta::describe_instruction<0xdc, 11, 3, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "cc";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_call(cpu.operands_word(), cpu.flags.cy != 0);
    }
  };

  struct sbi : meta::describe_instruction<0xde, 7, 2>
  {
    static constexpr auto name = "sbi";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.op1(), cpu.flags.cy);
    }
  };

  struct rst_3 : meta::describe_instruction<0xdf, 11, 1>
  {
    static constexpr auto name = "rst_3";

    void operator()(cpu& cpu) const
    {
      cpu.call(0x0018);
    }
  };

  struct rpo : meta::describe_instruction<0xe0, 5, 1, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "rpo";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_ret(not cpu.flags.p);
    }
  };

  struct pop_h : meta::describe_instruction<0xe1, 10, 1>
  {
    static constexpr auto name = "pop_h";

    void operator()(cpu& cpu) const
    {
      std::tie(cpu.h, cpu.l) = cpu.pop();
    }
  };

  struct jpo : meta::describe_instruction<0xe2, 10, 3>
  {
    static constexpr auto name = "jpo";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.conditional_jump(cpu.operands_word(), not cpu.flags.p);
    }
  };

  struct xthl : meta::describe_instruction<0xe3, 18, 1>
  {
    static constexpr auto name = "xthl";

    void operator()(cpu& cpu) const
    {
      const auto h = cpu.h;
      const auto l = cpu.l;
      cpu.l = cpu.memory_read_byte(cpu.sp);
      cpu.h = cpu.memory_read_byte(cpu.sp + 1);
      cpu.memory_write_byte(cpu.sp, l);
      cpu.memory_write_byte(cpu.sp + 1, h);
    }
  };

  struct cpo : meta::describe_instruction<0xe4, 11, 3, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "cpo";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_call(cpu.operands_word(), not cpu.flags.p);
    }
  };

  struct push_h : meta::describe_instruction<0xe5, 11, 1>
  {
    static constexpr auto name = "push_h";

    void operator()(cpu& cpu) const
    {
      cpu.push(cpu.h, cpu.l);
    }
  };

  struct ani : meta::describe_instruction<0xe6, 7, 2>
  {
    static constexpr auto name = "ani";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ana(cpu.op1());
    }
  };

  struct rst_4 : meta::describe_instruction<0xe7, 11, 1>
  {
    static constexpr auto name = "rst_4";

    void operator()(cpu& cpu) const
    {
      cpu.call(0x0020);
    }
  };

  struct rpe : meta::describe_instruction<0xe8, 5, 1, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "rpe";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_ret(cpu.flags.p);
    }
  };

  struct pchl : meta::describe_instruction<0xe9, 5, 1>
  {
    static constexpr auto name = "pchl";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.jump(cpu.hl());
    }
  };

  struct jpe : meta::describe_instruction<0xea, 10, 3>
  {
    static constexpr auto name = "jpe";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.conditional_jump(cpu.operands_word(), cpu.flags.p);
    }
  };

  struct xchg : meta::describe_instruction<0xeb, 5, 1>
  {
    static constexpr auto name = "xchg";

    void operator()(cpu& cpu) const noexcept
    {
      std::swap(cpu.d, cpu.h);
      std::swap(cpu.e, cpu.l);
    }
  };

  struct cpe : meta::describe_instruction<0xec, 11, 3, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "cpe";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_call(cpu.operands_word(), cpu.flags.p != 0);
    }
  };

  struct xri : meta::describe_instruction<0xee, 7, 2>
  {
    static constexpr auto name = "xri";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.op1());
    }
  };

  struct rst_5 : meta::describe_instruction<0xef, 11, 1>
  {
    static constexpr auto name = "rst_5";

    void operator()(cpu& cpu) const
    {
      cpu.call(0x0028);
    }
  };

  struct rp : meta::describe_instruction<0xf0, 5, 1, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "rp";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_ret(not cpu.flags.s);
    }
  };

  struct pop_psw : meta::describe_instruction<0xf1, 10, 1>
  {
    static constexpr auto name = "pop_psw";

    void operator()(cpu& cpu) const
    {
      auto flags = std::uint8_t{};
      std::tie(cpu.a, flags) = cpu.pop();

      cpu.flags.s  = flags & 0b10000000 ? true : false;
      cpu.flags.z  = flags & 0b01000000 ? true : false;
      cpu.flags.ac = flags & 0b00010000 ? true : false;
      cpu.flags.p  = flags & 0b00000100 ? true : false;
      cpu.flags.cy = flags & 0b00000001 ? true : false;
    }
  };

  struct jp : meta::describe_instruction<0xf2, 10, 3>
  {
    static constexpr auto name = "jp";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.conditional_jump(cpu.operands_word(), not cpu.flags.s);
    }
  };

  struct di : meta::describe_instruction<0xf3, 4, 1>
  {
    static constexpr auto name = "di";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.disable_interrupt();
    }
  };

  struct cp : meta::describe_instruction<0xf4, 11, 3, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "cp";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_call(cpu.operands_word(), not cpu.flags.s);
    }
  };

  struct push_psw : meta::describe_instruction<0xf5, 11, 1>
  {
    static constexpr auto name = "push_psw";

    void operator()(cpu& cpu) const
    {
      auto flags = std::uint8_t{0};

      if (cpu.flags.s)  flags |= 0b10000000;
      if (cpu.flags.z)  flags |= 0b01000000;
      if (cpu.flags.ac) flags |= 0b00010000;
      if (cpu.flags.p)  flags |= 0b00000100;
      if (cpu.flags.cy) flags |= 0b00000001;

      flags |=  0b00000010; // bit 1 is always 1
      flags &= ~0b00001000; // bit 3 is always 0
      flags &= ~0b00100000; // bit 5 is always 0

      cpu.push(cpu.a, flags);
    }
  };

  struct ori : meta::describe_instruction<0xf6, 7, 2>
  {
    static constexpr auto name = "ori";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.op1());
    }
  };

  struct rst_6 : meta::describe_instruction<0xf7, 11, 1>
  {
    static constexpr auto name = "rst_6";

    void operator()(cpu& cpu) const
    {
      cpu.call(0x0030);
    }

  };

  struct rm : meta::describe_instruction<0xf8, 5, 1, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "rm";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_ret(cpu.flags.s);
    }
  };

  struct sphl : meta::describe_instruction<0xf9, 5, 1>
  {
    static constexpr auto name = "sphl";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.sp = cpu.hl();
    }
  };

  struct jm : meta::describe_instruction<0xfa, 10, 3>
  {
    static constexpr auto name = "jm";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.conditional_jump(cpu.operands_word(), cpu.flags.s);
    }
  };

  struct ei : meta::describe_instruction<0xfb, 4, 1>
  {
    static constexpr auto name = "ei";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.enable_interrupt();
    }
  };

  struct cm : meta::describe_instruction<0xfc, 11, 3, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "cm";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_call(cpu.operands_word(), cpu.flags.s != 0);
    }
  };

  struct cpi : meta::describe_instruction<0xfe, 7, 2>
  {
    static constexpr auto name = "cpi";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.op1());
    }
  };

  struct rst_7 : meta::describe_instruction<0xff, 11, 1>
  {
    static constexpr auto name = "rst_7";

    void operator()(cpu& cpu) const
    {
      cpu.call(0x0038);
    }
  };

  using instructions_8080 = meta::make_instructions<
    nop,
    lxi_b,
    stax_b,
    inx_b,
    inr_b,
    dcr_b,
    mvi_b,
    rlc,
    meta::unimplemented<0x08>,
    dad_b,
    ldax_b,
    dcx_b,
    inr_c,
    dcr_c,
    mvi_c,
    rrc,
    meta::unimplemented<0x10>,
    lxi_d,
    stax_d,
    inx_d,
    inr_d,
    dcr_d,
    mvi_d,
    ral,
    meta::unimplemented<0x18>,
    dad_d,
    ldax_d,
    dcx_d,
    inr_e,
    dcr_e,
    mvi_e,
    rar,
    meta::unimplemented<0x20>,
    lxi_h,
    shld,
    inx_h,
    inr_h,
    dcr_h,
    mvi_h,
    daa,
    meta::unimplemented<0x28>,
    dad_h,
    lhld,
    dcx_h,
    inr_l,
    dcr_l,
    mvi_l_d8,
    cma,
    meta::unimplemented<0x30>,
    lxi_sp,
    sta,
    inx_sp,
    inr_m,
    dcr_m,
    mvi_m,
    stc,
    meta::unimplemented<0x38>,
    dad_sp,
    lda,
    dcx_sp,
    inr_a,
    dcr_a,
    mvi_a,
    cmc,
    mov_b_b,
    mov_b_c,
    mov_b_d,
    mov_b_e,
    mov_b_h,
    mov_b_l,
    mov_b_m,
    mov_b_a,
    mov_c_b,
    mov_c_c,
    mov_c_d,
    mov_c_e,
    mov_c_h,
    mov_c_l,
    mov_c_m,
    mov_c_a,
    mov_d_b,
    mov_d_c,
    mov_d_d,
    mov_d_e,
    mov_d_h,
    mov_d_l,
    mov_d_m,
    mov_d_a,
    mov_e_b,
    mov_e_c,
    mov_e_d,
    mov_e_e,
    mov_e_h,
    mov_e_l,
    mov_e_m,
    mov_e_a,
    mov_h_b,
    mov_h_c,
    mov_h_d,
    mov_h_e,
    mov_h_h,
    mov_h_l,
    mov_h_m,
    mov_h_a,
    mov_l_b,
    mov_l_c,
    mov_l_d,
    mov_l_e,
    mov_l_h,
    mov_l_l,
    mov_l_m,
    mov_l_a,
    mov_m_b,
    mov_m_c,
    mov_m_d,
    mov_m_e,
    mov_m_h,
    mov_m_l,
    hlt,
    mov_m_a,
    mov_a_b,
    mov_a_c,
    mov_a_d,
    mov_a_e,
    mov_a_h,
    mov_a_l,
    mov_a_m,
    mov_a_a,
    add_b,
    add_c,
    add_d,
    add_e,
    add_h,
    add_l,
    add_m,
    add_a,
    adc_b,
    adc_c,
    adc_d,
    adc_e,
    adc_h,
    adc_l,
    adc_m,
    adc_a,
    sub_b,
    sub_c,
    sub_d,
    sub_e,
    sub_h,
    sub_l,
    sub_m,
    sub_a,
    sbb_b,
    sbb_c,
    sbb_d,
    sbb_e,
    sbb_h,
    sbb_l,
    sbb_m,
    sbb_a,
    ana_b,
    ana_c,
    ana_d,
    ana_e,
    ana_h,
    ana_l,
    ana_m,
    ana_a,
    xra_b,
    xra_c,
    xra_d,
    xra_e,
    xra_h,
    xra_l,
    xra_m,
    xra_a,
    ora_b,
    ora_c,
    ora_d,
    ora_e,
    ora_h,
    ora_l,
    ora_m,
    ora_a,
    cmp_b,
    cmp_c,
    cmp_d,
    cmp_e,
    cmp_h,
    cmp_l,
    cmp_m,
    cmp_a,
    rnz,
    pop_b,
    jnz,
    jmp,
    cnz,
    push_b,
    adi,
    rst_0,
    rz,
    ret,
    jz,
    meta::unimplemented<0xcb>,
    cz,
    call,
    aci,
    rst_1,
    rnc,
    pop_d,
    jnc,
    out,
    cnc,
    push_d,
    sui,
    rst_2,
    rc,
    meta::unimplemented<0xd9>,
    jc,
    in,
    cc,
    meta::unimplemented<0xdd>,
    sbi,
    rst_3,
    rpo,
    pop_h,
    jpo,
    xthl,
    cpo,
    push_h,
    ani,
    rst_4,
    rpe,
    pchl,
    jpe,
    xchg,
    cpe,
    meta::unimplemented<0xed>,
    xri,
    rst_5,
    rp,
    pop_psw,
    jp,
    di,
    cp,
    push_psw,
    ori,
    rst_6,
    rm,
    sphl,
    jm,
    ei,
    cm,
    meta::unimplemented<0xfd>,
    cpi,
    rst_7
  >;

private:

  using instructions = meta::override_instructions<
    instructions_8080,
    typename Machine::overrides
  >;

public:

  cpu(Machine& machine)
    : a{0}
    , b{0}
    , c{0}
    , d{0}
    , e{0}
    , h{0}
    , l{0}
    , sp{0}
    , flags{}
    , machine_{machine}
    , interrupt_{false}
    , cycles_{0}
    , pc_{}
  {}

  friend
  std::ostream&
  operator<<(std::ostream& os, const cpu& cpu)
  {
    return os
      << std::resetiosflags(std::ios_base::basefield)
      << " "
      << (cpu.flags.z ? "z" : ".")
      << (cpu.flags.s ? "s" : ".")
      << (cpu.flags.p ? "p" : ".")
      << (cpu.flags.cy ? "c" : ".")
      << (cpu.flags.ac ? "a" : ".")
      << std::hex
      << "  A $" << std::setfill('0') << std::setw(2) << +cpu.a
      << " B $"  << std::setfill('0') << std::setw(2) << +cpu.b
      << " C $"  << std::setfill('0') << std::setw(2) << +cpu.c
      << " D $"  << std::setfill('0') << std::setw(2) << +cpu.d
      << " E $"  << std::setfill('0') << std::setw(2) << +cpu.e
      << " H $"  << std::setfill('0') << std::setw(2) << +cpu.h
      << " L $"  << std::setfill('0') << std::setw(2) << +cpu.l
      << " SP "  << std::setfill('0') << std::setw(4) << +cpu.sp
      ;
  }

  std::uint64_t
  step()
  {
    return step(util::dummy{});
  }

  template <typename Fn>
  std::uint64_t
  step(Fn&& fn)
  {
    const auto opcode = memory_read_byte(pc_);
    pc_ += 1;
    const auto cycles = meta::step(instructions{}, opcode, *this, std::forward<Fn>(fn));
    increment_cycles(cycles);
    return cycles;
  }

  void
  memory_write_byte(std::uint16_t address, std::uint8_t value)
  {
    machine_.memory_write_byte(address, value);
  }

  [[nodiscard]]
  std::uint8_t
  memory_read_byte(std::uint16_t address)
  const
  {
    return machine_.memory_read_byte(address);
  }

  void
  write_hl(std::uint8_t value)
  {
    memory_write_byte(hl(), value);
  }

  [[nodiscard]]
  std::uint8_t
  read_hl()
  const
  {
    const auto offset = hl();
    return memory_read_byte(offset);
  }

  [[nodiscard]]
  std::uint16_t
  hl()
  const noexcept
  {
    return (h << 8) | l;
  }

  [[nodiscard]]
  std::uint16_t
  bc()
  const noexcept
  {
    return (b << 8) | c;
  }

  [[nodiscard]]
  std::uint16_t
  de()
  const noexcept
  {
    return (d << 8) | e;
  }

  void
  push(std::uint8_t high, std::uint8_t low)
  {
    memory_write_byte(sp - 1, high);
    memory_write_byte(sp - 2, low);
    sp -= 2;
  }

  [[nodiscard]]
  std::tuple<std::uint8_t, std::uint8_t>
  pop()
  {
    const auto low = memory_read_byte(sp);
    const auto high = memory_read_byte(sp + 1);
    sp += 2;
    return {high, low};
  }

  [[nodiscard]]
  std::uint16_t
  pop_word()
  {
    const auto low = memory_read_byte(sp);
    const auto high = memory_read_byte(sp + 1);
    sp += 2;
    return low | (high << 8);
  }

  void
  call(std::uint16_t addr)
  {
    push((pc_ >> 8) & 0x00ff, pc_ & 0x00ff);
    pc_ = addr;
  }

  [[nodiscard]]
  bool
  conditional_call(std::uint16_t addr, bool condition)
  {
    if (condition)
    {
      call(addr);
      return true;
    }
    else
    {
      return false;
    }
  }

  void
  jump(std::uint16_t addr)
  {
    pc_ = addr;
  }

  void
  conditional_jump(std::uint16_t addr, bool condition)
  {
    if (condition)
    {
      jump(addr);
    }
  }

  void
  ret()
  {
    pc_ = pop_word();
  }

  [[nodiscard]]
  bool
  conditional_ret(bool condition)
  {
    if (condition)
    {
      ret();
      return true;
    }
    else
    {
      return false;
    }
  }

  [[nodiscard]]
  std::uint8_t
  op1()
  {
    const auto op1 = memory_read_byte(pc_);
    pc_ += 1;
    return op1;
  }

  [[nodiscard]]
  std::tuple<std::uint8_t, std::uint8_t>
  operands()
  {
    const auto op1 = memory_read_byte(pc_ + 0);
    const auto op2 = memory_read_byte(pc_ + 1);
    pc_ += 2;
    return {op1, op2};
  }

  [[nodiscard]]
  std::uint16_t
  operands_word()
  {
    const auto [op1, op2] = operands();
    return op1 | (op2 << 8);
  }

  [[nodiscard]]
  Machine&
  machine()
  noexcept
  {
    return machine_;
  }

  [[nodiscard]]
  const Machine&
  machine()
  const noexcept
  {
    return machine_;
  }

  [[nodiscard]]
  std::uint16_t
  pc()
  const noexcept
  {
    return pc_;
  }

  void
  enable_interrupt()
  noexcept
  {
    interrupt_ = true;
  }

  void
  disable_interrupt()
  noexcept
  {
    interrupt_ = false;
  }

  [[nodiscard]]
  bool
  interrupt_enabled()
  const noexcept
  {
    return interrupt_;
  }

  void
  interrupt(std::uint16_t interrupt)
  {
    if (interrupt_enabled())
    {
      call(interrupt);
      disable_interrupt();
      increment_cycles(11);
    }
  }

  [[nodiscard]]
  std::uint64_t
  cycles()
  const noexcept
  {
    return cycles_;
  }

private:

  [[nodiscard]]
  std::uint8_t
  inr(std::uint8_t value)
  noexcept
  {
    const std::uint8_t res = value + 1;
    flags.ac = (res & 0x0f) == 0;
    flags.z = res == 0;
    flags.s = (res & 0b10000000) != 0;
    flags.p = util::parity(res);
    return res;
  }

  [[nodiscard]]
  std::uint8_t
  dcr(std::uint8_t value)
  noexcept
  {
    const std::uint8_t res = value - 1;
    flags.ac = not ((res & 0x0f) == 0x0f);
    flags.z = res == 0;
    flags.s = (res & 0b10000000) != 0;
    flags.p = util::parity(res);
    return res;
  }

  void
  adda(std::uint8_t val, bool carry)
  noexcept
  {
    const std::uint16_t res = a + val + carry;
    flags.z  = (res & 0xff) == 0;
    flags.s  = (res & 0b10000000) != 0;
    flags.cy = (res & 0b100000000) != 0;
    flags.ac = (a ^ res ^ val) & 0x10;
    flags.p  = util::parity(res & 0xff);
    a = res & 0xff;
  }

  void
  suba(std::uint8_t val, bool carry)
  noexcept
  {
    const std::int16_t res = a - val - carry;
    flags.z = (res & 0xff) == 0;
    flags.s = (res & 0b10000000) != 0;
    flags.cy = (res & 0b100000000) != 0;
    flags.ac = ~(a ^ res ^ val) & 0x10;
    flags.p = util::parity(res & 0xff);
    a = res & 0xff;
  }

  void
  ora(std::uint8_t val)
  noexcept
  {
    a |= val;
    flags.cy = false;
    flags.ac = false;
    flags.z = a == 0;
    flags.s = (a & 0x80) != 0;
    flags.p = util::parity(a);
  }

  void
  ana(std::uint8_t val)
  noexcept
  {
    const std::uint8_t res = a & val;
    flags.cy = false;
    flags.ac = ((a | val) & 0x08) != 0;
    flags.z = res == 0;
    flags.s = (res & 0x80) != 0;
    flags.p = util::parity(res);
    a = res;
  }

  void
  xra(std::uint8_t val)
  noexcept
  {
    a ^= val;
    flags.cy = false;
    flags.ac = false;
    flags.z = a == 0;
    flags.s = (a & 0x80) != 0;
    flags.p = util::parity(a);
  }

  void
  cmp(std::uint8_t val)
  noexcept
  {
    const std::int16_t res = a - val;
    flags.cy = (res & 0b100000000) != 0;
    flags.ac = ~(a ^ res ^ val) & 0x10;
    flags.z = (res & 0xff) == 0;
    flags.s = (res & 0x80) != 0;
    flags.p = util::parity(res);
  }

private:

  void
  increment_cycles(std::uint64_t nb_cycles)
  noexcept
  {
    cycles_ += nb_cycles;
  }

private:

  Machine& machine_;
  bool interrupt_;
  std::uint64_t cycles_;
  std::uint16_t pc_;
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::specific
