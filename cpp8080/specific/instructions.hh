#pragma once

#include <cstdint>
#include <tuple>
#include <utility>

#include "cpp8080/meta/make_instructions.hh"
#include "cpp8080/meta/unimplemented.hh"
#include "cpp8080/specific/halt.hh"
#include "cpp8080/specific/cpu_fwd.hh"
#include "cpp8080/util/parity.hh"

namespace cpp8080::specific {

/*------------------------------------------------------------------------------------------------*/

template <std::uint8_t, auto>
struct dcr;

template <std::uint8_t Opcode, typename Cpu, auto Cpu::* reg>
struct dcr<Opcode, reg> : meta::describe_instruction<Opcode, 5, 1>
{
  void operator()(Cpu& cpu) const noexcept
  {
    cpu.*reg = cpu.dcr(cpu.*reg);
  }
};

/*------------------------------------------------------------------------------------------------*/

template <std::uint8_t, auto, auto>
struct dcx;

template <std::uint8_t Opcode, typename Cpu, auto Cpu::* reg1, auto Cpu::* reg2>
struct dcx<Opcode, reg1, reg2> : meta::describe_instruction<Opcode, 5, 1>
{
  void operator()(Cpu& cpu) const noexcept
  {
    cpu.*reg1 -= 1;
    if (cpu.*reg1 == 0x00ff)
    {
      cpu.*reg2 -= 1;
    }
  }
};

/*------------------------------------------------------------------------------------------------*/

template <std::uint8_t, auto>
struct inr;

template <std::uint8_t Opcode, typename Cpu, auto Cpu::* reg>
struct inr<Opcode, reg> : meta::describe_instruction<Opcode, 5, 1>
{
  void operator()(Cpu& cpu) const noexcept
  {
    cpu.*reg = cpu.inr(cpu.*reg);
  }
};

/*------------------------------------------------------------------------------------------------*/

template <std::uint8_t, auto, auto>
struct inx;

template <std::uint8_t Opcode, typename Cpu, auto Cpu::* reg1, auto Cpu::* reg2>
struct inx<Opcode, reg1, reg2> : meta::describe_instruction<Opcode, 5, 1>
{
  void operator()(Cpu& cpu) const noexcept
  {
    cpu.*reg1 += 1;
    if (cpu.*reg1 == 0)
    {
      cpu.*reg2 += 1;
    }
  }
};

/*------------------------------------------------------------------------------------------------*/

template <std::uint8_t, auto, auto>
struct lxi;

template <std::uint8_t Opcode, typename Cpu, auto Cpu::* reg1, auto Cpu::* reg2>
struct lxi<Opcode, reg1, reg2> : meta::describe_instruction<Opcode, 10, 3>
{
  void operator()(Cpu& cpu) const
  {
    std::tie(cpu.*reg1, cpu.*reg2) = cpu.operands();
  }
};

/*------------------------------------------------------------------------------------------------*/

template <std::uint8_t, auto, auto>
struct mov;

template <std::uint8_t Opcode, typename Cpu, auto Cpu::* reg1, auto Cpu::* reg2>
struct mov<Opcode, reg1, reg2> : meta::describe_instruction<Opcode, 5, 1>
{
  void operator()(Cpu& cpu) const noexcept
  {
    cpu.*reg1 = cpu.*reg2;
  }
};

/*------------------------------------------------------------------------------------------------*/

struct nop : meta::describe_instruction<0x00, 4, 1>
{
  static constexpr auto name = "nop";

  template <typename Machine> void operator()(cpu<Machine>&) const noexcept
  {}
};

template <typename Cpu>
struct lxi_b : lxi<0x01, Cpu::register_c, Cpu::register_b>
{
  static constexpr auto name = "lxi_b";
};

struct stax_b : meta::describe_instruction<0x02, 7, 1>
{
  static constexpr auto name = "stax_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.memory_write_byte(cpu.bc(), cpu.a);
  }
};

template <typename Cpu>
struct inx_b : inx<0x03, Cpu::register_c, Cpu::register_b>
{
  static constexpr auto name = "inx_b";
};

template <typename Cpu>
struct inr_b : inr<0x04, Cpu::register_b>
{
  static constexpr auto name = "inr_b";
};

template <typename Cpu>
struct dcr_b : dcr<0x05, Cpu::register_b>
{
  static constexpr auto name = "dcr_b";
};

struct mvi_b : meta::describe_instruction<0x06, 7, 2>
{
  static constexpr auto name = "mvi_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.b = cpu.op1();
  }
};

struct rlc : meta::describe_instruction<0x07, 4, 1>
{
  static constexpr auto name = "rlc";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    const auto x = cpu.a;
    cpu.a = ((x & 0x80) >> 7) | (x << 1);
    cpu.flags.cy = (0x80 == (x & 0x80));
  }
};

struct dad_b : meta::describe_instruction<0x09, 10, 1>
{
  static constexpr auto name = "dad_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
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

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.a = cpu.memory_read_byte(cpu.bc());
  }
};

template <typename Cpu>
struct dcx_b : dcx<0x0b, Cpu::register_c, Cpu::register_b>
{
  static constexpr auto name = "dcx_b";
};

template <typename Cpu>
struct inr_c : inr<0x0c, Cpu::register_c>
{
  static constexpr auto name = "inr_c";
};

template <typename Cpu>
struct dcr_c : dcr<0x0d, Cpu::register_c>
{
  static constexpr auto name = "dcr_b";
};

struct mvi_c : meta::describe_instruction<0x0e, 7, 2>
{
  static constexpr auto name = "mvi_c";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.c = cpu.op1();
  }
};

struct rrc : meta::describe_instruction<0x0f, 4, 1>
{
  static constexpr auto name = "rrc";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    const auto x = cpu.a;
    cpu.a = ((x & 1) << 7) | (x >> 1);
    cpu.flags.cy = (1 == (x & 1));
  }
};

template <typename Cpu>
struct lxi_d : lxi<0x11, Cpu::register_e, Cpu::register_d>
{
  static constexpr auto name = "lxi_d";
};

struct stax_d : meta::describe_instruction<0x12, 7, 1>
{
  static constexpr auto name = "stax_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.memory_write_byte(cpu.de(), cpu.a);
  }
};

template <typename Cpu>
struct inx_d : inx<0x13, Cpu::register_e, Cpu::register_d>
{
  static constexpr auto name = "inx_b";
};

template <typename Cpu>
struct inr_d : inr<0x14, Cpu::register_d>
{
  static constexpr auto name = "inr_d";
};


template <typename Cpu>
struct dcr_d : dcr<0x15, Cpu::register_d>
{
  static constexpr auto name = "dcr_b";
};

struct mvi_d : meta::describe_instruction<0x16, 7, 2>
{
  static constexpr auto name = "mvi_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.d = cpu.op1();
  }
};

struct ral : meta::describe_instruction<0x17, 4, 1>
{
  static constexpr auto name = "ral";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    const auto a = cpu.a;
    cpu.a = cpu.flags.cy | (a << 1);
    cpu.flags.cy = (0x80 == (a & 0x80));
  }
};

struct dad_d : meta::describe_instruction<0x19, 10, 1>
{
  static constexpr auto name = "dad_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
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

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.a = cpu.memory_read_byte(cpu.de());
  }
};

template <typename Cpu>
struct dcx_d : dcx<0x1b, Cpu::register_e, Cpu::register_d>
{
  static constexpr auto name = "dcx_d";
};

template <typename Cpu>
struct inr_e : inr<0x1c, Cpu::register_e>
{
  static constexpr auto name = "inr_e";
};

template <typename Cpu>
struct dcr_e : dcr<0x1d, Cpu::register_e>
{
  static constexpr auto name = "dcr_b";
};

struct mvi_e : meta::describe_instruction<0x1e, 7, 2>
{
  static constexpr auto name = "mvi_e";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.e = cpu.op1();
  }
};

struct rar : meta::describe_instruction<0x1f, 4, 1>
{
  static constexpr auto name = "rar";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    const auto a = cpu.a;
    cpu.a = (cpu.flags.cy << 7) | (a >> 1);
    cpu.flags.cy = (1 == (a & 1));
  }
};

template <typename Cpu>
struct lxi_h : lxi<0x21, Cpu::register_l, Cpu::register_h>
{
  static constexpr auto name = "lxi_b";
};

struct shld : meta::describe_instruction<0x22, 16, 3>
{
  static constexpr auto name = "shld";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    const std::uint16_t offset = cpu.operands_word();
    cpu.memory_write_byte(offset, cpu.l);
    cpu.memory_write_byte(offset + 1, cpu.h);
  }
};

template <typename Cpu>
struct inx_h : inx<0x23, Cpu::register_l, Cpu::register_h>
{
  static constexpr auto name = "inx_h";
};

template <typename Cpu>
struct inr_h : inr<0x24, Cpu::register_h>
{
  static constexpr auto name = "inr_h";
};

template <typename Cpu>
struct dcr_h : dcr<0x25, Cpu::register_h>
{
  static constexpr auto name = "dcr_h";
};

struct mvi_h : meta::describe_instruction<0x26, 7, 2>
{
  static constexpr auto name = "mvi_h";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.h = cpu.op1();
  }
};

struct daa : meta::describe_instruction<0x27, 4, 1>
{
  static constexpr auto name = "daa";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
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

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
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

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    const std::uint16_t offset = cpu.operands_word();
    cpu.l = cpu.memory_read_byte(offset);
    cpu.h = cpu.memory_read_byte(offset + 1);
  }
};

template <typename Cpu>
struct dcx_h : dcx<0x2b, Cpu::register_l, Cpu::register_h>
{
  static constexpr auto name = "dcx_b";
};

template <typename Cpu>
struct inr_l : inr<0x2c, Cpu::register_l>
{
  static constexpr auto name = "inr_l";
};

template <typename Cpu>
struct dcr_l : dcr<0x2d, Cpu::register_l>
{
  static constexpr auto name = "dcr_l";
};

struct mvi_l_d8 : meta::describe_instruction<0x2e, 7, 2>
{
  static constexpr auto name = "mvi_l";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.l = cpu.op1();
  }
};

struct cma : meta::describe_instruction<0x2f, 4, 1>
{
  static constexpr auto name = "cma";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.a ^= 0xff;
  }
};

struct lxi_sp : meta::describe_instruction<0x31, 10, 3>
{
  static constexpr auto name = "lxi_sp";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.sp = cpu.operands_word();
  }
};

struct sta : meta::describe_instruction<0x32, 13, 3>
{
  static constexpr auto name = "sta";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.memory_write_byte(cpu.operands_word(), cpu.a);
  }
};

struct inx_sp : meta::describe_instruction<0x33, 5, 1>
{
  static constexpr auto name = "inx_sp";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.sp += 1;
  }
};

struct inr_m : meta::describe_instruction<0x34, 10, 1>
{
  static constexpr auto name = "inr_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    const auto res = cpu.inr(cpu.read_hl());
    cpu.write_hl(res);
  }
};

struct dcr_m : meta::describe_instruction<0x35, 10, 1>
{
  static constexpr auto name = "dcr_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    const auto res = cpu.dcr(cpu.read_hl());
    cpu.write_hl(res);
  }
};

struct mvi_m : meta::describe_instruction<0x36, 10, 2>
{
  static constexpr auto name = "mvi_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.write_hl(cpu.op1());
  }
};

struct stc : meta::describe_instruction<0x37, 4, 1>
{
  static constexpr auto name = "stc";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.flags.cy = true;
  }
};

struct dad_sp : meta::describe_instruction<0x39, 10, 1>
{
  static constexpr auto name = "dad_sp";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
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

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    const std::uint16_t offset = cpu.operands_word();
    cpu.a = cpu.memory_read_byte(offset);
  }
};

struct dcx_sp : meta::describe_instruction<0x3b, 5, 1>
{
  static constexpr auto name = "dcx_sp";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.sp -= 1;
  }
};

struct inr_a : meta::describe_instruction<0x3c, 5, 1>
{
  static constexpr auto name = "inr_a";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.a = cpu.inr(cpu.a);
  }
};

template <typename Cpu>
struct dcr_a : dcr<0x3d, Cpu::register_a>
{
  static constexpr auto name = "dcr_a";
};

struct mvi_a : meta::describe_instruction<0x3e, 7, 2>
{
  static constexpr auto name = "mvi_a";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.a = cpu.op1();
  }
};

struct cmc : meta::describe_instruction<0x3f, 4, 1>
{
  static constexpr auto name = "cmc";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.flags.cy = not(cpu.flags.cy);
  }
};

template <typename Cpu>
struct mov_b_b : mov<0x40, Cpu::register_b, Cpu::register_b>
{
  static constexpr auto name = "mov_b_b";
};

template <typename Cpu>
struct mov_b_c : mov<0x41, Cpu::register_b, Cpu::register_c>
{
  static constexpr auto name = "mov_b_c";
};

template <typename Cpu>
struct mov_b_d : mov<0x42, Cpu::register_b, Cpu::register_d>
{
  static constexpr auto name = "mov_b_d";
};

template <typename Cpu>
struct mov_b_e : mov<0x43, Cpu::register_b, Cpu::register_e>
{
  static constexpr auto name = "mov_b_e";
};

template <typename Cpu>
struct mov_b_h : mov<0x44, Cpu::register_b, Cpu::register_h>
{
  static constexpr auto name = "mov_b_h";
};

template <typename Cpu>
struct mov_b_l : mov<0x45, Cpu::register_b, Cpu::register_l>
{
  static constexpr auto name = "mov_b_l";
};

struct mov_b_m : meta::describe_instruction<0x46, 7, 1>
{
  static constexpr auto name = "mov_b_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.b = cpu.read_hl();
  }
};

template <typename Cpu>
struct mov_b_a : mov<0x47, Cpu::register_b, Cpu::register_a>
{
  static constexpr auto name = "mov_b_a";
};

template <typename Cpu>
struct mov_c_b : mov<0x48, Cpu::register_c, Cpu::register_b>
{
  static constexpr auto name = "mov_c_b";
};

template <typename Cpu>
struct mov_c_c : mov<0x49, Cpu::register_c, Cpu::register_c>
{
  static constexpr auto name = "mov_c_c";
};

template <typename Cpu>
struct mov_c_d : mov<0x4a, Cpu::register_c, Cpu::register_d>
{
  static constexpr auto name = "mov_c_d";
};

template <typename Cpu>
struct mov_c_e : mov<0x4b, Cpu::register_c, Cpu::register_e>
{
  static constexpr auto name = "mov_c_e";
};

template <typename Cpu>
struct mov_c_h : mov<0x4c, Cpu::register_c, Cpu::register_h>
{
  static constexpr auto name = "mov_c_h";
};

template <typename Cpu>
struct mov_c_l : mov<0x4d, Cpu::register_c, Cpu::register_l>
{
  static constexpr auto name = "mov_c_l";
};

struct mov_c_m : meta::describe_instruction<0x4e, 7, 1>
{
  static constexpr auto name = "mov_c_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.c = cpu.read_hl();
  }
};

template <typename Cpu>
struct mov_c_a : mov<0x4f, Cpu::register_c, Cpu::register_a>
{
  static constexpr auto name = "mov_c_a";
};

template <typename Cpu>
struct mov_d_b : mov<0x50, Cpu::register_d, Cpu::register_b>
{
  static constexpr auto name = "mov_d_b";
};

template <typename Cpu>
struct mov_d_c : mov<0x51, Cpu::register_d, Cpu::register_c>
{
  static constexpr auto name = "mov_d_c";
};

template <typename Cpu>
struct mov_d_d : mov<0x52, Cpu::register_b, Cpu::register_b>
{
  static constexpr auto name = "mov_d_d";
};

template <typename Cpu>
struct mov_d_e : mov<0x53, Cpu::register_d, Cpu::register_e>
{
  static constexpr auto name = "mov_d_e";
};

template <typename Cpu>
struct mov_d_h : mov<0x54, Cpu::register_d, Cpu::register_h>
{
  static constexpr auto name = "mov_d_h";
};

template <typename Cpu>
struct mov_d_l : mov<0x55, Cpu::register_d, Cpu::register_l>
{
  static constexpr auto name = "mov_d_l";
};

struct mov_d_m : meta::describe_instruction<0x56, 7, 1>
{
  static constexpr auto name = "mov_d_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.d = cpu.read_hl();
  }
};

template <typename Cpu>
struct mov_d_a : mov<0x57, Cpu::register_d, Cpu::register_a>
{
  static constexpr auto name = "mov_d_a";
};

template <typename Cpu>
struct mov_e_b : mov<0x58, Cpu::register_e, Cpu::register_b>
{
  static constexpr auto name = "mov_e_b";
};

template <typename Cpu>
struct mov_e_c : mov<0x59, Cpu::register_e, Cpu::register_c>
{
  static constexpr auto name = "mov_e_c";
};

template <typename Cpu>
struct mov_e_d : mov<0x5a, Cpu::register_e, Cpu::register_d>
{
  static constexpr auto name = "mov_e_d";
};

template <typename Cpu>
struct mov_e_e : mov<0x5b, Cpu::register_e, Cpu::register_e>
{
  static constexpr auto name = "mov_e_e";
};

template <typename Cpu>
struct mov_e_h : mov<0x5c, Cpu::register_e, Cpu::register_h>
{
  static constexpr auto name = "mov_e_h";
};

template <typename Cpu>
struct mov_e_l : mov<0x5d, Cpu::register_e, Cpu::register_l>
{
  static constexpr auto name = "mov_e_l";
};

struct mov_e_m : meta::describe_instruction<0x5e, 7, 1>
{
  static constexpr auto name = "mov_e_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.e = cpu.read_hl();
  }
};

template <typename Cpu>
struct mov_e_a : mov<0x5f, Cpu::register_e, Cpu::register_a>
{
  static constexpr auto name = "mov_e_a";
};

template <typename Cpu>
struct mov_h_b : mov<0x60, Cpu::register_h, Cpu::register_b>
{
  static constexpr auto name = "mov_h_b";
};

template <typename Cpu>
struct mov_h_c : mov<0x61, Cpu::register_h, Cpu::register_c>
{
  static constexpr auto name = "mov_h_c";
};

template <typename Cpu>
struct mov_h_d : mov<0x62, Cpu::register_h, Cpu::register_d>
{
  static constexpr auto name = "mov_h_d";
};

template <typename Cpu>
struct mov_h_e : mov<0x63, Cpu::register_h, Cpu::register_e>
{
  static constexpr auto name = "mov_h_e";
};

template <typename Cpu>
struct mov_h_h : mov<0x64, Cpu::register_h, Cpu::register_h>
{
  static constexpr auto name = "mov_h_h";
};

template <typename Cpu>
struct mov_h_l : mov<0x65, Cpu::register_h, Cpu::register_l>
{
  static constexpr auto name = "mov_h_l";
};

struct mov_h_m : meta::describe_instruction<0x66, 7, 1>
{
  static constexpr auto name = "mov_h_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.h = cpu.read_hl();
  }
};

template <typename Cpu>
struct mov_h_a : mov<0x67, Cpu::register_h, Cpu::register_a>
{
  static constexpr auto name = "mov_h_a";
};

template <typename Cpu>
struct mov_l_b : mov<0x68, Cpu::register_l, Cpu::register_b>
{
  static constexpr auto name = "mov_l_b";
};

template <typename Cpu>
struct mov_l_c : mov<0x69, Cpu::register_l, Cpu::register_c>
{
  static constexpr auto name = "mov_l_c";
};

template <typename Cpu>
struct mov_l_d : mov<0x6a, Cpu::register_l, Cpu::register_d>
{
  static constexpr auto name = "mov_l_d";
};

template <typename Cpu>
struct mov_l_e : mov<0x6b, Cpu::register_l, Cpu::register_e>
{
  static constexpr auto name = "mov_l_e";
};

template <typename Cpu>
struct mov_l_h : mov<0x6c, Cpu::register_l, Cpu::register_h>
{
  static constexpr auto name = "mov_l_h";
};

template <typename Cpu>
struct mov_l_l : mov<0x40, Cpu::register_l, Cpu::register_l>
{
  static constexpr auto name = "mov_l_l";
};

struct mov_l_m : meta::describe_instruction<0x6e, 7, 1>
{
  static constexpr auto name = "mov_l_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.l = cpu.read_hl();
  }
};

template <typename Cpu>
struct mov_l_a : mov<0x6f, Cpu::register_l, Cpu::register_a>
{
  static constexpr auto name = "mov_l_a";
};

struct mov_m_b : meta::describe_instruction<0x70, 7, 1>
{
  static constexpr auto name = "mov_m_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.write_hl(cpu.b);
  }
};

struct mov_m_c : meta::describe_instruction<0x71, 7, 1>
{
  static constexpr auto name = "mov_m_c";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.write_hl(cpu.c);
  }
};

struct mov_m_d : meta::describe_instruction<0x72, 7, 1>
{
  static constexpr auto name = "mov_m_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.write_hl(cpu.d);
  }
};

struct mov_m_e : meta::describe_instruction<0x73, 7, 1>
{
  static constexpr auto name = "mov_m_e";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.write_hl(cpu.e);
  }
};

struct mov_m_h : meta::describe_instruction<0x74, 7, 1>
{
  static constexpr auto name = "mov_m_h";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.write_hl(cpu.h);
  }
};

struct mov_m_l : meta::describe_instruction<0x75, 7, 1>
{
  static constexpr auto name = "mov_m_l";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.write_hl(cpu.l);
  }
};

struct hlt : meta::describe_instruction<0x76, 7, 1>
{
  static constexpr auto name = "hlt";

  template <typename Machine> void operator()(cpu<Machine>&) const
  {
    throw halt{"HLT"};
  }
};

struct mov_m_a : meta::describe_instruction<0x77, 7, 1>
{
  static constexpr auto name = "mov_m_a";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.write_hl(cpu.a);
  }
};

template <typename Cpu>
struct mov_a_b : mov<0x78, Cpu::register_a, Cpu::register_b>
{
  static constexpr auto name = "mov_a_b";
};

template <typename Cpu>
struct mov_a_c : mov<0x79, Cpu::register_a, Cpu::register_c>
{
  static constexpr auto name = "mov_a_c";
};

template <typename Cpu>
struct mov_a_d : mov<0x7a, Cpu::register_a, Cpu::register_d>
{
  static constexpr auto name = "mov_a_d";
};

template <typename Cpu>
struct mov_a_e : mov<0x7b, Cpu::register_a, Cpu::register_e>
{
  static constexpr auto name = "mov_a_e";
};

template <typename Cpu>
struct mov_a_h : mov<0x7c, Cpu::register_a, Cpu::register_h>
{
  static constexpr auto name = "mov_a_h";
};

template <typename Cpu>
struct mov_a_l : mov<0x7d, Cpu::register_a, Cpu::register_l>
{
  static constexpr auto name = "mov_a_l";
};

struct mov_a_m : meta::describe_instruction<0x7e, 7, 1>
{
  static constexpr auto name = "mov_a_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.a = cpu.read_hl();
  }
};

template <typename Cpu>
struct mov_a_a : mov<0x7f, Cpu::register_a, Cpu::register_a>
{
  static constexpr auto name = "mov_a_a";
};

struct add_b : meta::describe_instruction<0x80, 4, 1>
{
  static constexpr auto name = "add_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.b, 0);
  }
};

struct add_c : meta::describe_instruction<0x81, 4, 1>
{
  static constexpr auto name = "add_c";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.c, 0);
  }
};

struct add_d : meta::describe_instruction<0x82, 4, 1>
{
  static constexpr auto name = "add_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.d, 0);
  }
};

struct add_e : meta::describe_instruction<0x83, 4, 1>
{
  static constexpr auto name = "add_e";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.e, 0);
  }
};

struct add_h : meta::describe_instruction<0x84, 4, 1>
{
  static constexpr auto name = "add_h";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.h, 0);
  }
};

struct add_l : meta::describe_instruction<0x85, 4, 1>
{
  static constexpr auto name = "add_l";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.l, 0);
  }
};

struct add_m : meta::describe_instruction<0x86, 7, 1>
{
  static constexpr auto name = "add_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.adda(cpu.read_hl(), 0);
  }
};

struct add_a : meta::describe_instruction<0x87, 4, 1>
{
  static constexpr auto name = "add_a";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.a, 0);
  }
};

struct adc_b : meta::describe_instruction<0x88, 4, 1>
{
  static constexpr auto name = "adc_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.b, cpu.flags.cy);
  }
};

struct adc_c : meta::describe_instruction<0x89, 4, 1>
{
  static constexpr auto name = "adc_c";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.c, cpu.flags.cy);
  }
};

struct adc_d : meta::describe_instruction<0x8a, 4, 1>
{
  static constexpr auto name = "adc_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.d, cpu.flags.cy);
  }
};

struct adc_e : meta::describe_instruction<0x8b, 4, 1>
{
  static constexpr auto name = "adc_e";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.e, cpu.flags.cy);
  }
};

struct adc_h : meta::describe_instruction<0x8c, 4, 1>
{
  static constexpr auto name = "adc_h";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.h, cpu.flags.cy);
  }
};

struct adc_l : meta::describe_instruction<0x8d, 4, 1>
{
  static constexpr auto name = "adc_l";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.l, cpu.flags.cy);
  }
};

struct adc_m : meta::describe_instruction<0x8e, 7, 1>
{
  static constexpr auto name = "adc_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.adda(cpu.read_hl(), cpu.flags.cy);
  }
};

struct adc_a : meta::describe_instruction<0x8f, 4, 1>
{
  static constexpr auto name = "adc_a";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.a, cpu.flags.cy);
  }
};

struct sub_b : meta::describe_instruction<0x90, 4, 1>
{
  static constexpr auto name = "sub_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.b, 0);
  }
};

struct sub_c : meta::describe_instruction<0x91, 4, 1>
{
  static constexpr auto name = "sub_c";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.c, 0);
  }
};

struct sub_d : meta::describe_instruction<0x92, 4, 1>
{
  static constexpr auto name = "sub_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.d, 0);
  }
};

struct sub_e : meta::describe_instruction<0x93, 4, 1>
{
  static constexpr auto name = "sub_e";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.e, 0);
  }
};

struct sub_h : meta::describe_instruction<0x94, 4, 1>
{
  static constexpr auto name = "sub_h";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.h, 0);
  }
};

struct sub_l : meta::describe_instruction<0x95, 4, 1>
{
  static constexpr auto name = "sub_l";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.l, 0);
  }
};

struct sub_m : meta::describe_instruction<0x96, 7, 1>
{
  static constexpr auto name = "sub_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.suba(cpu.read_hl(), 0);
  }
};

struct sub_a : meta::describe_instruction<0x97, 4, 1>
{
  static constexpr auto name = "sub_a";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.a, 0);
  }
};

struct sbb_b : meta::describe_instruction<0x98, 4, 1>
{
  static constexpr auto name = "sbb_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.b, cpu.flags.cy);
  }
};

struct sbb_c : meta::describe_instruction<0x99, 4, 1>
{
  static constexpr auto name = "sbb_c";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.c, cpu.flags.cy);
  }
};

struct sbb_d : meta::describe_instruction<0x9a, 4, 1>
{
  static constexpr auto name = "sbb_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.d, cpu.flags.cy);
  }
};

struct sbb_e : meta::describe_instruction<0x9b, 4, 1>
{
  static constexpr auto name = "sbb_e";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.e, cpu.flags.cy);
  }
};

struct sbb_h : meta::describe_instruction<0x9c, 4, 1>
{
  static constexpr auto name = "sbb_h";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.h, cpu.flags.cy);
  }
};

struct sbb_l : meta::describe_instruction<0x9d, 4, 1>
{
  static constexpr auto name = "sbb_l";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.l, cpu.flags.cy);
  }
};

struct sbb_m : meta::describe_instruction<0x9e, 7, 1>
{
  static constexpr auto name = "sbb_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.suba(cpu.read_hl(), cpu.flags.cy);
  }
};

struct sbb_a : meta::describe_instruction<0x9f, 4, 1>
{
  static constexpr auto name = "sbb_a";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.a, cpu.flags.cy);
  }
};

struct ana_b : meta::describe_instruction<0xa0, 4, 1>
{
  static constexpr auto name = "ana_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ana(cpu.b);
  }
};

struct ana_c : meta::describe_instruction<0xa1, 4, 1>
{
  static constexpr auto name = "ana_c";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ana(cpu.c);
  }
};

struct ana_d : meta::describe_instruction<0xa2, 4, 1>
{
  static constexpr auto name = "ana_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ana(cpu.d);
  }
};

struct ana_e : meta::describe_instruction<0xa3, 4, 1>
{
  static constexpr auto name = "ana_e";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ana(cpu.e);
  }
};

struct ana_h : meta::describe_instruction<0xa4, 4, 1>
{
  static constexpr auto name = "ana_h";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ana(cpu.h);
  }
};

struct ana_l : meta::describe_instruction<0xa5, 4, 1>
{
  static constexpr auto name = "ana_l";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ana(cpu.l);
  }
};

struct ana_m : meta::describe_instruction<0xa6, 7, 1>
{
  static constexpr auto name = "ana_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.ana(cpu.read_hl());
  }
};

struct ana_a : meta::describe_instruction<0xa7, 4, 1>
{
  static constexpr auto name = "ana_a";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ana(cpu.a);
  }
};

struct xra_b : meta::describe_instruction<0xa8, 4, 1>
{
  static constexpr auto name = "xra_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.xra(cpu.b);
  }
};

struct xra_c : meta::describe_instruction<0xa9, 4, 1>
{
  static constexpr auto name = "xra_c";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.xra(cpu.c);
  }
};

struct xra_d : meta::describe_instruction<0xaa, 4, 1>
{
  static constexpr auto name = "xra_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.xra(cpu.d);
  }
};

struct xra_e : meta::describe_instruction<0xab, 4, 1>
{
  static constexpr auto name = "xra_e";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.xra(cpu.e);
  }
};

struct xra_h : meta::describe_instruction<0xac, 4, 1>
{
  static constexpr auto name = "xra_h";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.xra(cpu.h);
  }
};

struct xra_l : meta::describe_instruction<0xad, 4, 1>
{
  static constexpr auto name = "xra_l";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.xra(cpu.l);
  }
};

struct xra_m : meta::describe_instruction<0xae, 7, 1>
{
  static constexpr auto name = "xra_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.xra(cpu.read_hl());
  }
};

struct xra_a : meta::describe_instruction<0xaf, 4, 1>
{
  static constexpr auto name = "xra_a";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.xra(cpu.a);
  }
};

struct ora_b : meta::describe_instruction<0xb0, 4, 1>
{
  static constexpr auto name = "ora_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ora(cpu.b);
  }
};

struct ora_c : meta::describe_instruction<0xb1, 4, 1>
{
  static constexpr auto name = "ora_c";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ora(cpu.c);
  }
};

struct ora_d : meta::describe_instruction<0xb2, 4, 1>
{
  static constexpr auto name = "ora_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ora(cpu.d);
  }
};

struct ora_e : meta::describe_instruction<0xb3, 4, 1>
{
  static constexpr auto name = "ora_e";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ora(cpu.e);
  }
};

struct ora_h : meta::describe_instruction<0xb4, 4, 1>
{
  static constexpr auto name = "ora_h";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ora(cpu.h);
  }
};

struct ora_l : meta::describe_instruction<0xb5, 4, 1>
{
  static constexpr auto name = "ora_l";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ora(cpu.l);
  }
};

struct ora_m : meta::describe_instruction<0xb6, 7, 1>
{
  static constexpr auto name = "ora_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.ora(cpu.read_hl());
  }
};

struct ora_a : meta::describe_instruction<0xb7, 4, 1>
{
  static constexpr auto name = "ora_a";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ora(cpu.a);
  }
};

struct cmp_b : meta::describe_instruction<0xb8, 4, 1>
{
  static constexpr auto name = "cmp_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.cmp(cpu.b);
  }
};

struct cmp_c : meta::describe_instruction<0xb9, 4, 1>
{
  static constexpr auto name = "cmp_c";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.cmp(cpu.c);
  }
};

struct cmp_d : meta::describe_instruction<0xba, 4, 1>
{
  static constexpr auto name = "cmp_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.cmp(cpu.d);
  }
};

struct cmp_e : meta::describe_instruction<0xbb, 4, 1>
{
  static constexpr auto name = "cmp_e";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.cmp(cpu.e);
  }
};

struct cmp_h : meta::describe_instruction<0xbc, 4, 1>
{
  static constexpr auto name = "cmp_h";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.cmp(cpu.h);
  }
};

struct cmp_l : meta::describe_instruction<0xbd, 4, 1>
{
  static constexpr auto name = "cmp_l";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.cmp(cpu.l);
  }
};

struct cmp_m : meta::describe_instruction<0xbe, 7, 1>
{
  static constexpr auto name = "cmp_m";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.cmp(cpu.read_hl());
  }
};

struct cmp_a : meta::describe_instruction<0xbf, 4, 1>
{
  static constexpr auto name = "cmp_a";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.cmp(cpu.a);
  }
};

struct rnz : meta::describe_instruction<0xc0, 5, 1, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "rnz";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_ret(not cpu.flags.z);
  }
};

struct pop_b : meta::describe_instruction<0xc1, 10, 1>
{
  static constexpr auto name = "pop_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    std::tie(cpu.b, cpu.c) = cpu.pop();
  }
};

struct jnz : meta::describe_instruction<0xc2, 10, 3>
{
  static constexpr auto name = "jnz";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.conditional_jump(cpu.operands_word(), not cpu.flags.z);
  }
};

struct jmp : meta::describe_instruction<0xc3, 10, 3>
{
  static constexpr auto name = "jmp";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.jump(cpu.operands_word());
  }
};

struct cnz : meta::describe_instruction<0xc4, 11, 3, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "cnz";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_call(cpu.operands_word(), not cpu.flags.z);
  }
};

struct push_b : meta::describe_instruction<0xc5, 11, 1>
{
  static constexpr auto name = "push_b";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.push(cpu.b, cpu.c);
  }
};

struct adi : meta::describe_instruction<0xc6, 7, 2>
{
  static constexpr auto name = "adi";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.op1(), 0);
  }
};

struct rst_0 : meta::describe_instruction<0xc7, 11, 1>
{
  static constexpr auto name = "rst_0";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.call(0x0000);
  }
};

struct rz : meta::describe_instruction<0xc8, 5, 1, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "rz";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_ret(cpu.flags.z);
  }
};

struct ret : meta::describe_instruction<0xc9, 10, 1>
{
  static constexpr auto name = "ret";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.ret();
  }
};

struct jz : meta::describe_instruction<0xca, 10, 3>
{
  static constexpr auto name = "jz";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.conditional_jump(cpu.operands_word(), cpu.flags.z);
  }
};

struct cz : meta::describe_instruction<0xcc, 11, 3, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "cz";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_call(cpu.operands_word(), cpu.flags.z);
  }
};

struct call : meta::describe_instruction<0xcd, 11, 3>
{
  static constexpr auto name = "call";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.call(cpu.operands_word());
  }
};

struct aci : meta::describe_instruction<0xce, 7, 2>
{
  static constexpr auto name = "aci";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.adda(cpu.op1(), cpu.flags.cy);
  }
};

struct rst_1 : meta::describe_instruction<0xcf, 11, 1>
{
  static constexpr auto name = "rst_1";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.call(0x0008);
  }
};

struct rnc : meta::describe_instruction<0xd0, 5, 1, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "rnc";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_ret(not cpu.flags.cy);
  }
};

struct pop_d : meta::describe_instruction<0xd1, 10, 1>
{
  static constexpr auto name = "pop_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    std::tie(cpu.d, cpu.e) = cpu.pop();
  }
};

struct jnc : meta::describe_instruction<0xd2, 10, 3>
{
  static constexpr auto name = "jnc";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.conditional_jump(cpu.operands_word(), not cpu.flags.cy);
  }
};

struct out : meta::describe_instruction<0xd3, 10, 2>
{
  static constexpr auto name = "out";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {}
};

struct cnc : meta::describe_instruction<0xd4, 11, 3, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "cnc";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_call(cpu.operands_word(), not cpu.flags.cy);
  }
};

struct push_d : meta::describe_instruction<0xd5, 11, 1>
{
  static constexpr auto name = "push_d";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.push(cpu.d, cpu.e);
  }
};

struct sui : meta::describe_instruction<0xd6, 7, 2>
{
  static constexpr auto name = "sui";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.op1(), 0);
  }
};

struct rst_2 : meta::describe_instruction<0xd7, 11, 1>
{
  static constexpr auto name = "rst_2";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.call(0x0010);
  }
};

struct rc : meta::describe_instruction<0xd8, 5, 1, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "rc";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_ret(cpu.flags.cy);
  }
};

struct jc : meta::describe_instruction<0xda, 10, 3>
{
  static constexpr auto name = "jc";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.conditional_jump(cpu.operands_word(), cpu.flags.cy);
  }
};

struct in : meta::describe_instruction<0xdb, 10, 2>
{
  static constexpr auto name = "in";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {}
};

struct cc : meta::describe_instruction<0xdc, 11, 3, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "cc";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_call(cpu.operands_word(), cpu.flags.cy != 0);
  }
};

struct sbi : meta::describe_instruction<0xde, 7, 2>
{
  static constexpr auto name = "sbi";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.suba(cpu.op1(), cpu.flags.cy);
  }
};

struct rst_3 : meta::describe_instruction<0xdf, 11, 1>
{
  static constexpr auto name = "rst_3";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.call(0x0018);
  }
};

struct rpo : meta::describe_instruction<0xe0, 5, 1, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "rpo";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_ret(not cpu.flags.p);
  }
};

struct pop_h : meta::describe_instruction<0xe1, 10, 1>
{
  static constexpr auto name = "pop_h";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    std::tie(cpu.h, cpu.l) = cpu.pop();
  }
};

struct jpo : meta::describe_instruction<0xe2, 10, 3>
{
  static constexpr auto name = "jpo";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.conditional_jump(cpu.operands_word(), not cpu.flags.p);
  }
};

struct xthl : meta::describe_instruction<0xe3, 18, 1>
{
  static constexpr auto name = "xthl";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
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

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_call(cpu.operands_word(), not cpu.flags.p);
  }
};

struct push_h : meta::describe_instruction<0xe5, 11, 1>
{
  static constexpr auto name = "push_h";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.push(cpu.h, cpu.l);
  }
};

struct ani : meta::describe_instruction<0xe6, 7, 2>
{
  static constexpr auto name = "ani";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ana(cpu.op1());
  }
};

struct rst_4 : meta::describe_instruction<0xe7, 11, 1>
{
  static constexpr auto name = "rst_4";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.call(0x0020);
  }
};

struct rpe : meta::describe_instruction<0xe8, 5, 1, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "rpe";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_ret(cpu.flags.p);
  }
};

struct pchl : meta::describe_instruction<0xe9, 5, 1>
{
  static constexpr auto name = "pchl";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.jump(cpu.hl());
  }
};

struct jpe : meta::describe_instruction<0xea, 10, 3>
{
  static constexpr auto name = "jpe";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.conditional_jump(cpu.operands_word(), cpu.flags.p);
  }
};

struct xchg : meta::describe_instruction<0xeb, 5, 1>
{
  static constexpr auto name = "xchg";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    std::swap(cpu.d, cpu.h);
    std::swap(cpu.e, cpu.l);
  }
};

struct cpe : meta::describe_instruction<0xec, 11, 3, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "cpe";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_call(cpu.operands_word(), cpu.flags.p != 0);
  }
};

struct xri : meta::describe_instruction<0xee, 7, 2>
{
  static constexpr auto name = "xri";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.xra(cpu.op1());
  }
};

struct rst_5 : meta::describe_instruction<0xef, 11, 1>
{
  static constexpr auto name = "rst_5";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.call(0x0028);
  }
};

struct rp : meta::describe_instruction<0xf0, 5, 1, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "rp";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_ret(not cpu.flags.s);
  }
};

struct pop_psw : meta::describe_instruction<0xf1, 10, 1>
{
  static constexpr auto name = "pop_psw";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
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

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.conditional_jump(cpu.operands_word(), not cpu.flags.s);
  }
};

struct di : meta::describe_instruction<0xf3, 4, 1>
{
  static constexpr auto name = "di";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.disable_interrupt();
  }
};

struct cp : meta::describe_instruction<0xf4, 11, 3, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "cp";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_call(cpu.operands_word(), not cpu.flags.s);
  }
};

struct push_psw : meta::describe_instruction<0xf5, 11, 1>
{
  static constexpr auto name = "push_psw";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
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

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.ora(cpu.op1());
  }
};

struct rst_6 : meta::describe_instruction<0xf7, 11, 1>
{
  static constexpr auto name = "rst_6";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.call(0x0030);
  }

};

struct rm : meta::describe_instruction<0xf8, 5, 1, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "rm";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_ret(cpu.flags.s);
  }
};

struct sphl : meta::describe_instruction<0xf9, 5, 1>
{
  static constexpr auto name = "sphl";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.sp = cpu.hl();
  }
};

struct jm : meta::describe_instruction<0xfa, 10, 3>
{
  static constexpr auto name = "jm";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.conditional_jump(cpu.operands_word(), cpu.flags.s);
  }
};

struct ei : meta::describe_instruction<0xfb, 4, 1>
{
  static constexpr auto name = "ei";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.enable_interrupt();
  }
};

struct cm : meta::describe_instruction<0xfc, 11, 3, meta::constant_instruction<false, 6>>
{
  static constexpr auto name = "cm";

  template <typename Machine> auto operator()(cpu<Machine>& cpu) const
  {
    return cpu.conditional_call(cpu.operands_word(), cpu.flags.s != 0);
  }
};

struct cpi : meta::describe_instruction<0xfe, 7, 2>
{
  static constexpr auto name = "cpi";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const noexcept
  {
    cpu.cmp(cpu.op1());
  }
};

struct rst_7 : meta::describe_instruction<0xff, 11, 1>
{
  static constexpr auto name = "rst_7";

  template <typename Machine> void operator()(cpu<Machine>& cpu) const
  {
    cpu.call(0x0038);
  }
};

/*------------------------------------------------------------------------------------------------*/

template <typename Cpu>
using instructions_8080 = meta::make_instructions<
  nop,
  lxi_b<Cpu>,
  stax_b,
  inx_b<Cpu>,
  inr_b<Cpu>,
  dcr_b<Cpu>,
  mvi_b,
  rlc,
  meta::unimplemented<0x08>,
  dad_b,
  ldax_b,
  dcx_b<Cpu>,
  inr_c<Cpu>,
  dcr_c<Cpu>,
  mvi_c,
  rrc,
  meta::unimplemented<0x10>,
  lxi_d<Cpu>,
  stax_d,
  inx_d<Cpu>,
  inr_d<Cpu>,
  dcr_d<Cpu>,
  mvi_d,
  ral,
  meta::unimplemented<0x18>,
  dad_d,
  ldax_d,
  dcx_d<Cpu>,
  inr_e<Cpu>,
  dcr_e<Cpu>,
  mvi_e,
  rar,
  meta::unimplemented<0x20>,
  lxi_h<Cpu>,
  shld,
  inx_h<Cpu>,
  inr_h<Cpu>,
  dcr_h<Cpu>,
  mvi_h,
  daa,
  meta::unimplemented<0x28>,
  dad_h,
  lhld,
  dcx_h<Cpu>,
  inr_l<Cpu>,
  dcr_l<Cpu>,
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
  dcr_a<Cpu>,
  mvi_a,
  cmc,
  mov_b_b<Cpu>,
  mov_b_c<Cpu>,
  mov_b_d<Cpu>,
  mov_b_e<Cpu>,
  mov_b_h<Cpu>,
  mov_b_l<Cpu>,
  mov_b_m,
  mov_b_a<Cpu>,
  mov_c_b<Cpu>,
  mov_c_c<Cpu>,
  mov_c_d<Cpu>,
  mov_c_e<Cpu>,
  mov_c_h<Cpu>,
  mov_c_l<Cpu>,
  mov_c_m,
  mov_c_a<Cpu>,
  mov_d_b<Cpu>,
  mov_d_c<Cpu>,
  mov_d_d<Cpu>,
  mov_d_e<Cpu>,
  mov_d_h<Cpu>,
  mov_d_l<Cpu>,
  mov_d_m,
  mov_d_a<Cpu>,
  mov_e_b<Cpu>,
  mov_e_c<Cpu>,
  mov_e_d<Cpu>,
  mov_e_e<Cpu>,
  mov_e_h<Cpu>,
  mov_e_l<Cpu>,
  mov_e_m,
  mov_e_a<Cpu>,
  mov_h_b<Cpu>,
  mov_h_c<Cpu>,
  mov_h_d<Cpu>,
  mov_h_e<Cpu>,
  mov_h_h<Cpu>,
  mov_h_l<Cpu>,
  mov_h_m,
  mov_h_a<Cpu>,
  mov_l_b<Cpu>,
  mov_l_c<Cpu>,
  mov_l_d<Cpu>,
  mov_l_e<Cpu>,
  mov_l_h<Cpu>,
  mov_l_l<Cpu>,
  mov_l_m,
  mov_l_a<Cpu>,
  mov_m_b,
  mov_m_c,
  mov_m_d,
  mov_m_e,
  mov_m_h,
  mov_m_l,
  hlt,
  mov_m_a,
  mov_a_b<Cpu>,
  mov_a_c<Cpu>,
  mov_a_d<Cpu>,
  mov_a_e<Cpu>,
  mov_a_h<Cpu>,
  mov_a_l<Cpu>,
  mov_a_m,
  mov_a_a<Cpu>,
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

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::specific
