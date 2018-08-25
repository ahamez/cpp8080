#pragma once

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <tuple>

#include "cpp8080/meta/make_instructions.hh"
#include "cpp8080/specific/cpu_fwd.hh"
#include "cpp8080/specific/halt.hh"
#include "cpp8080/util/concat.hh"
#include "cpp8080/util/hooks.hh"
#include "cpp8080/util/parity.hh"

namespace cpp8080::specific {

/*------------------------------------------------------------------------------------------------*/

template <typename Machine>
class cpu final
{
private:

  std::uint8_t a_;
  std::uint8_t b_;
  std::uint8_t c_;
  std::uint8_t d_;
  std::uint8_t e_;
  std::uint8_t h_;
  std::uint8_t l_;
  std::uint16_t sp_;
  bool cy_;
  bool p_;
  bool ac_;
  bool z_;
  bool s_;
  Machine& machine_;
  bool interrupt_;
  std::uint64_t cycles_;
  std::uint16_t pc_;

private:

  template <std::uint8_t Opcode, auto cpu::* reg>
  struct i_adda : meta::describe_instruction<Opcode, 4, 1>
  {
    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.*reg, 0);
    }
  };

  template <std::uint8_t Opcode, auto cpu::* reg>
  struct i_ana : meta::describe_instruction<Opcode, 4, 1>
  {
    void operator()(cpu& cpu) const noexcept
    {
      cpu.ana(cpu.*reg);
    }
  };

  template <std::uint8_t Opcode, auto cpu::* reg>
  struct i_dcr : meta::describe_instruction<Opcode, 5, 1>
  {
    void operator()(cpu& cpu) const noexcept
    {
      cpu.*reg = cpu.dcr(cpu.*reg);
    }
  };

  template <std::uint8_t Opcode, auto cpu::* reg1, auto cpu::* reg2>
  struct i_dcx : meta::describe_instruction<Opcode, 5, 1>
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
  struct i_inr : meta::describe_instruction<Opcode, 5, 1>
  {
    void operator()(cpu& cpu) const noexcept
    {
      cpu.*reg = cpu.inr(cpu.*reg);
    }
  };

  template <std::uint8_t Opcode, auto cpu::* reg1, auto cpu::* reg2>
  struct i_inx : meta::describe_instruction<Opcode, 5, 1>
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
  struct i_lxi : meta::describe_instruction<Opcode, 10, 3>
  {
    void operator()(cpu& cpu) const
    {
      std::tie(cpu.*reg1, cpu.*reg2) = cpu.operands();
    }
  };

  template <std::uint8_t Opcode, auto cpu::* reg1, auto cpu::* reg2>
  struct i_mov : meta::describe_instruction<Opcode, 5, 1>
  {
    void operator()(cpu& cpu) const noexcept
    {
      cpu.*reg1 = cpu.*reg2;
    }
  };

  template <std::uint8_t Opcode, auto cpu::* reg>
  struct i_mov_m : meta::describe_instruction<Opcode, 7, 1>
  {
    void operator()(cpu& cpu) const
    {
      cpu.*reg = cpu.read_hl();
    }
  };

  template <std::uint8_t Opcode, auto cpu::* reg>
  struct i_mov_to_m : meta::describe_instruction<Opcode, 7, 1>
  {
    void operator()(cpu& cpu) const
    {
      cpu.write_hl(cpu.*reg);
    }
  };

  template <std::uint8_t Opcode, auto cpu::* reg>
  struct i_mvi : meta::describe_instruction<Opcode, 7, 2>
  {
    void operator()(cpu& cpu) const noexcept
    {
      cpu.*reg = cpu.op1();
    }
  };

  template <std::uint8_t Opcode>
  struct unimplemented : meta::describe_instruction<Opcode, 255, 1>
  {
    static constexpr auto name = "unimplemented";

    void operator()(const cpu&) const
    {
      throw std::runtime_error{util::concat("Unimplemented instruction ", std::hex, +Opcode)};
    }
  };

private:

  struct nop : meta::describe_instruction<0x00, 4, 1>
  {
    static constexpr auto name = "nop";
    void operator()(cpu&) const noexcept {}
  };

  struct hlt : meta::describe_instruction<0x76, 7, 1>
  {
    static constexpr auto name = "hlt";

    void operator()(cpu&) const
    {
      throw halt{"HLT"};
    }
  };

  struct lxi_b : i_lxi<0x01, &cpu::c_, &cpu::b_> {static constexpr auto name = "lxi_b";};

  struct stax_b : meta::describe_instruction<0x02, 7, 1>
  {
    static constexpr auto name = "stax_b";

    void operator()(cpu& cpu) const
    {
      cpu.memory_write_byte(cpu.bc(), cpu.a_);
    }
  };

  struct inx_b : i_inx<0x03, &cpu::c_, &cpu::b_> {static constexpr auto name = "inx_b";};
  struct inr_b : i_inr<0x04, &cpu::b_> {static constexpr auto name = "inr_b";};
  struct dcr_b : i_dcr<0x05, &cpu::b_> {static constexpr auto name = "dcr_b";};
  struct mvi_b : i_mvi<0x06, &cpu::b_> {static constexpr auto name = "mvi_b";};

  struct rlc : meta::describe_instruction<0x07, 4, 1>
  {
    static constexpr auto name = "rlc";

    void operator()(cpu& cpu) const noexcept
    {
      const auto x = cpu.a_;
      cpu.a_ = ((x & 0x80) >> 7) | (x << 1);
      cpu.cy_ = (0x80 == (x & 0x80));
    }
  };

  struct dad_b : meta::describe_instruction<0x09, 10, 1>
  {
    static constexpr auto name = "dad_b";

    void operator()(cpu& cpu) const noexcept
    {
      const std::uint32_t res = cpu.hl() + cpu.bc();
      cpu.h_= (res & 0xff00) >> 8;
      cpu.l_= res & 0x00ff;
      cpu.cy_ = ((res & 0xffff0000) != 0);
    }
  };

  struct ldax_b : meta::describe_instruction<0x0a, 7, 1>
  {
    static constexpr auto name = "ldax_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.a_ = cpu.memory_read_byte(cpu.bc());
    }
  };

  struct dcx_b : i_dcx<0x0b, &cpu::c_, &cpu::b_> {static constexpr auto name = "dcx_b";};
  struct inr_c : i_inr<0x0c, &cpu::c_> {static constexpr auto name = "inr_c";};
  struct dcr_c : i_dcr<0x0d, &cpu::c_> {static constexpr auto name = "dcr_b";};
  struct mvi_c : i_mvi<0x0e, &cpu::c_> {static constexpr auto name = "mvi_c";};

  struct rrc : meta::describe_instruction<0x0f, 4, 1>
  {
    static constexpr auto name = "rrc";

    void operator()(cpu& cpu) const noexcept
    {
      const auto x = cpu.a_;
      cpu.a_ = ((x & 1) << 7) | (x >> 1);
      cpu.cy_ = (1 == (x & 1));
    }
  };

  struct lxi_d : i_lxi<0x11, &cpu::e_, &cpu::d_> {static constexpr auto name = "lxi_d";};

  struct stax_d : meta::describe_instruction<0x12, 7, 1>
  {
    static constexpr auto name = "stax_d";

    void operator()(cpu& cpu) const
    {
      cpu.memory_write_byte(cpu.de(), cpu.a_);
    }
  };

  struct inx_d : i_inx<0x13, &cpu::e_, &cpu::d_> {static constexpr auto name = "inx_b";};
  struct inr_d : i_inr<0x14, &cpu::d_> {static constexpr auto name = "inr_d";};
  struct dcr_d : i_dcr<0x15, &cpu::d_> {static constexpr auto name = "dcr_b";};
  struct mvi_d : i_mvi<0x16, &cpu::d_> {static constexpr auto name = "mvi_d";};

  struct ral : meta::describe_instruction<0x17, 4, 1>
  {
    static constexpr auto name = "ral";

    void operator()(cpu& cpu) const noexcept
    {
      const auto a = cpu.a_;
      cpu.a_ = cpu.cy_ | (a << 1);
      cpu.cy_ = (0x80 == (a & 0x80));
    }
  };

  struct dad_d : meta::describe_instruction<0x19, 10, 1>
  {
    static constexpr auto name = "dad_d";

    void operator()(cpu& cpu) const noexcept
    {
      const std::uint32_t res = cpu.hl() + cpu.de();
      cpu.h_= (res & 0xff00) >> 8;
      cpu.l_= res & 0x00ff;
      cpu.cy_ = ((res & 0xffff0000) != 0);
    }
  };

  struct ldax_d : meta::describe_instruction<0x1a, 7, 1>
  {
    static constexpr auto name = "ldax_d";

    void operator()(cpu& cpu) const
    {
      cpu.a_ = cpu.memory_read_byte(cpu.de());
    }
  };

  struct dcx_d : i_dcx<0x1b, &cpu::e_, &cpu::d_> {static constexpr auto name = "dcx_d";};

  struct inr_e : i_inr<0x1c, &cpu::e_> {static constexpr auto name = "inr_e";};
  struct dcr_e : i_dcr<0x1d, &cpu::e_> {static constexpr auto name = "dcr_e";};
  struct mvi_e : i_mvi<0x1e, &cpu::e_> {static constexpr auto name = "mvi_e";};

  struct rar : meta::describe_instruction<0x1f, 4, 1>
  {
    static constexpr auto name = "rar";

    void operator()(cpu& cpu) const noexcept
    {
      const auto a = cpu.a_;
      cpu.a_ = (cpu.cy_ << 7) | (a >> 1);
      cpu.cy_ = (1 == (a & 1));
    }
  };

  struct lxi_h : i_lxi<0x21, &cpu::l_, &cpu::h_> {static constexpr auto name = "lxi_b";};

  struct shld : meta::describe_instruction<0x22, 16, 3>
  {
    static constexpr auto name = "shld";

    void operator()(cpu& cpu) const
    {
      const std::uint16_t offset = cpu.operands_word();
      cpu.memory_write_byte(offset, cpu.l_);
      cpu.memory_write_byte(offset + 1, cpu.h_);
    }
  };

  struct inx_h : i_inx<0x23, &cpu::l_, &cpu::h_> {static constexpr auto name = "inx_h";};
  struct inr_h : i_inr<0x24, &cpu::h_> {static constexpr auto name = "inr_h";};
  struct dcr_h : i_dcr<0x25, &cpu::h_> {static constexpr auto name = "dcr_h";};
  struct mvi_h : i_mvi<0x26, &cpu::h_> {static constexpr auto name = "mvi_h";};

  struct daa : meta::describe_instruction<0x27, 4, 1>
  {
    static constexpr auto name = "daa";

    void operator()(cpu& cpu) const noexcept
    {
      auto cy = cpu.cy_;
      auto value_to_add = std::uint8_t{0};

      const std::uint8_t lsb = cpu.a_ & 0x0f;
      const std::uint8_t msb = cpu.a_ >> 4;

      if (cpu.ac_ or lsb > 9)
      {
        value_to_add += 0x06;
      }
      if (cpu.cy_ or msb > 9 or (msb >= 9 and lsb > 9))
      {
        value_to_add += 0x60;
        cy = true;
      }
      cpu.adda(value_to_add, 0);
      cpu.p_ = util::parity(cpu.a_);
      cpu.cy_ = cy;

    }
  };

  struct dad_h : meta::describe_instruction<0x29, 10, 1>
  {
    static constexpr auto name = "dad_h";

    void operator()(cpu& cpu) const noexcept
    {
      const std::uint32_t res = 2 * cpu.hl();
      cpu.h_= (res & 0xff00) >> 8;
      cpu.l_= res & 0x00ff;
      cpu.cy_ = ((res & 0xffff0000) != 0);
    }
  };

  struct lhld : meta::describe_instruction<0x2a, 16, 3>
  {
    static constexpr auto name = "lhld";

    void operator()(cpu& cpu) const
    {
      const std::uint16_t offset = cpu.operands_word();
      cpu.l_= cpu.memory_read_byte(offset);
      cpu.h_= cpu.memory_read_byte(offset + 1);
    }
  };

  struct dcx_h : i_dcx<0x2b, &cpu::l_, &cpu::h_> {static constexpr auto name = "dcx_b";};

  struct inr_l : i_inr<0x2c, &cpu::l_> {static constexpr auto name = "inr_l";};
  struct dcr_l : i_dcr<0x2d, &cpu::l_> {static constexpr auto name = "dcr_l";};
  struct mvi_l : i_mvi<0x2e, &cpu::l_> {static constexpr auto name = "mvi_l";};

  struct cma : meta::describe_instruction<0x2f, 4, 1>
  {
    static constexpr auto name = "cma";

    void operator()(cpu& cpu) const
    {
      cpu.a_ ^= 0xff;
    }
  };

  struct lxi_sp : meta::describe_instruction<0x31, 10, 3>
  {
    static constexpr auto name = "lxi_sp";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.sp_ = cpu.operands_word();
    }
  };

  struct sta : meta::describe_instruction<0x32, 13, 3>
  {
    static constexpr auto name = "sta";

    void operator()(cpu& cpu) const
    {
      cpu.memory_write_byte(cpu.operands_word(), cpu.a_);
    }
  };

  struct inx_sp : meta::describe_instruction<0x33, 5, 1>
  {
    static constexpr auto name = "inx_sp";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.sp_ += 1;
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
      cpu.cy_ = true;
    }
  };

  struct dad_sp : meta::describe_instruction<0x39, 10, 1>
  {
    static constexpr auto name = "dad_sp";

    void operator()(cpu& cpu) const
    {
      const std::uint32_t res = cpu.hl() + cpu.sp_;
      cpu.h_= (res & 0xff00) >> 8;
      cpu.l_= res & 0x00ff;
      cpu.cy_ = ((res & 0xffff0000) > 0);
    }
  };

  struct lda : meta::describe_instruction<0x3a, 13, 3>
  {
    static constexpr auto name = "lda";

    void operator()(cpu& cpu) const
    {
      const std::uint16_t offset = cpu.operands_word();
      cpu.a_ = cpu.memory_read_byte(offset);
    }
  };

  struct dcx_sp : meta::describe_instruction<0x3b, 5, 1>
  {
    static constexpr auto name = "dcx_sp";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.sp_ -= 1;
    }
  };

  struct inr_a : i_inr<0x3c, &cpu::a_> {static constexpr auto name = "inr_a";};
  struct dcr_a : i_dcr<0x3d, &cpu::a_> {static constexpr auto name = "dcr_a";};
  struct mvi_a : i_mvi<0x3e, &cpu::a_> {static constexpr auto name = "mvi_a";};

  struct cmc : meta::describe_instruction<0x3f, 4, 1>
  {
    static constexpr auto name = "cmc";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cy_ = not(cpu.cy_);
    }
  };

  struct mov_b_b : i_mov<0x40, &cpu::b_, &cpu::b_> {static constexpr auto name = "mov_b_b";};
  struct mov_b_c : i_mov<0x41, &cpu::b_, &cpu::c_> {static constexpr auto name = "mov_b_c";};
  struct mov_b_d : i_mov<0x42, &cpu::b_, &cpu::d_> {static constexpr auto name = "mov_b_d";};
  struct mov_b_e : i_mov<0x43, &cpu::b_, &cpu::e_> {static constexpr auto name = "mov_b_e";};
  struct mov_b_h : i_mov<0x44, &cpu::b_, &cpu::h_> {static constexpr auto name = "mov_b_h";};
  struct mov_b_l : i_mov<0x45, &cpu::b_, &cpu::l_> {static constexpr auto name = "mov_b_l";};
  struct mov_b_m : i_mov_m<0x46, &cpu::b_> {static constexpr auto name = "mov_b_m";};
  struct mov_b_a : i_mov<0x47, &cpu::b_, &cpu::a_> {static constexpr auto name = "mov_b_a";};
  struct mov_c_b : i_mov<0x48, &cpu::c_, &cpu::b_> {static constexpr auto name = "mov_c_b";};
  struct mov_c_c : i_mov<0x49, &cpu::c_, &cpu::c_> {static constexpr auto name = "mov_c_c";};
  struct mov_c_d : i_mov<0x4a, &cpu::c_, &cpu::d_> {static constexpr auto name = "mov_c_d";};
  struct mov_c_e : i_mov<0x4b, &cpu::c_, &cpu::e_> {static constexpr auto name = "mov_c_e";};
  struct mov_c_h : i_mov<0x4c, &cpu::c_, &cpu::h_> {static constexpr auto name = "mov_c_h";};
  struct mov_c_l : i_mov<0x4d, &cpu::c_, &cpu::l_> {static constexpr auto name = "mov_c_l";};
  struct mov_c_m : i_mov_m<0x4e, &cpu::c_> {static constexpr auto name = "mov_c_m";};
  struct mov_c_a : i_mov<0x4f, &cpu::c_, &cpu::a_> {static constexpr auto name = "mov_c_a";};
  struct mov_d_b : i_mov<0x50, &cpu::d_, &cpu::b_> {static constexpr auto name = "mov_d_b";};
  struct mov_d_c : i_mov<0x51, &cpu::d_, &cpu::c_> {static constexpr auto name = "mov_d_c";};
  struct mov_d_d : i_mov<0x52, &cpu::b_, &cpu::b_> {static constexpr auto name = "mov_d_d";};
  struct mov_d_e : i_mov<0x53, &cpu::d_, &cpu::e_> {static constexpr auto name = "mov_d_e";};
  struct mov_d_h : i_mov<0x54, &cpu::d_, &cpu::h_> {static constexpr auto name = "mov_d_h";};
  struct mov_d_l : i_mov<0x55, &cpu::d_, &cpu::l_> {static constexpr auto name = "mov_d_l";};
  struct mov_d_m : i_mov_m<0x56, &cpu::d_> {static constexpr auto name = "mov_d_m";};
  struct mov_d_a : i_mov<0x57, &cpu::d_, &cpu::a_> {static constexpr auto name = "mov_d_a";};
  struct mov_e_b : i_mov<0x58, &cpu::e_, &cpu::b_> {static constexpr auto name = "mov_e_b";};
  struct mov_e_c : i_mov<0x59, &cpu::e_, &cpu::c_> {static constexpr auto name = "mov_e_c";};
  struct mov_e_d : i_mov<0x5a, &cpu::e_, &cpu::d_> {static constexpr auto name = "mov_e_d";};
  struct mov_e_e : i_mov<0x5b, &cpu::e_, &cpu::e_> {static constexpr auto name = "mov_e_e";};
  struct mov_e_h : i_mov<0x5c, &cpu::e_, &cpu::h_> {static constexpr auto name = "mov_e_h";};
  struct mov_e_l : i_mov<0x5d, &cpu::e_, &cpu::l_> {static constexpr auto name = "mov_e_l";};
  struct mov_e_m : i_mov_m<0x5e, &cpu::e_> {static constexpr auto name = "mov_e_m";};
  struct mov_e_a : i_mov<0x5f, &cpu::e_, &cpu::a_> {static constexpr auto name = "mov_e_a";};
  struct mov_h_b : i_mov<0x60, &cpu::h_, &cpu::b_> {static constexpr auto name = "mov_h_b";};
  struct mov_h_c : i_mov<0x61, &cpu::h_, &cpu::c_> {static constexpr auto name = "mov_h_c";};
  struct mov_h_d : i_mov<0x62, &cpu::h_, &cpu::d_> {static constexpr auto name = "mov_h_d";};
  struct mov_h_e : i_mov<0x63, &cpu::h_, &cpu::e_> {static constexpr auto name = "mov_h_e";};
  struct mov_h_h : i_mov<0x64, &cpu::h_, &cpu::h_> {static constexpr auto name = "mov_h_h";};
  struct mov_h_l : i_mov<0x65, &cpu::h_, &cpu::l_> {static constexpr auto name = "mov_h_l";};
  struct mov_h_m : i_mov_m<0x66, &cpu::h_> {static constexpr auto name = "mov_h_m";};
  struct mov_h_a : i_mov<0x67, &cpu::h_, &cpu::a_> {static constexpr auto name = "mov_h_a";};
  struct mov_l_b : i_mov<0x68, &cpu::l_, &cpu::b_> {static constexpr auto name = "mov_l_b";};
  struct mov_l_c : i_mov<0x69, &cpu::l_, &cpu::c_> {static constexpr auto name = "mov_l_c";};
  struct mov_l_d : i_mov<0x6a, &cpu::l_, &cpu::d_> {static constexpr auto name = "mov_l_d";};
  struct mov_l_e : i_mov<0x6b, &cpu::l_, &cpu::e_> {static constexpr auto name = "mov_l_e";};
  struct mov_l_h : i_mov<0x6c, &cpu::l_, &cpu::h_> {static constexpr auto name = "mov_l_h";};
  struct mov_l_l : i_mov<0x40, &cpu::l_, &cpu::l_> {static constexpr auto name = "mov_l_l";};
  struct mov_l_m : i_mov_m<0x6e, &cpu::l_> {static constexpr auto name = "mov_l_m";};
  struct mov_l_a : i_mov<0x6f, &cpu::l_, &cpu::a_> {static constexpr auto name = "mov_l_a";};
  struct mov_m_b : i_mov_to_m<0x70, &cpu::b_> {static constexpr auto name = "mov_m_b";};
  struct mov_m_c : i_mov_to_m<0x71, &cpu::c_> {static constexpr auto name = "mov_m_c";};
  struct mov_m_d : i_mov_to_m<0x72, &cpu::d_> {static constexpr auto name = "mov_m_d";};
  struct mov_m_e : i_mov_to_m<0x73, &cpu::e_> {static constexpr auto name = "mov_m_e";};
  struct mov_m_h : i_mov_to_m<0x74, &cpu::h_> {static constexpr auto name = "mov_m_h";};
  struct mov_m_l : i_mov_to_m<0x75, &cpu::l_> {static constexpr auto name = "mov_m_l";};
  struct mov_m_a : i_mov_to_m<0x77, &cpu::a_> {static constexpr auto name = "mov_m_a";};
  struct mov_a_b : i_mov<0x78, &cpu::a_, &cpu::b_> {static constexpr auto name = "mov_a_b";};
  struct mov_a_c : i_mov<0x79, &cpu::a_, &cpu::c_> {static constexpr auto name = "mov_a_c";};
  struct mov_a_d : i_mov<0x7a, &cpu::a_, &cpu::d_> {static constexpr auto name = "mov_a_d";};
  struct mov_a_e : i_mov<0x7b, &cpu::a_, &cpu::e_> {static constexpr auto name = "mov_a_e";};
  struct mov_a_h : i_mov<0x7c, &cpu::a_, &cpu::h_> {static constexpr auto name = "mov_a_h";};
  struct mov_a_l : i_mov<0x7d, &cpu::a_, &cpu::l_> {static constexpr auto name = "mov_a_l";};
  struct mov_a_m : i_mov_m<0x7e, &cpu::a_> {static constexpr auto name = "mov_a_m";};
  struct mov_a_a : i_mov<0x7f, &cpu::a_, &cpu::a_> {static constexpr auto name = "mov_a_a";};

  struct add_b : i_adda<0x80, &cpu::b_>{static constexpr auto name = "add_b";};
  struct add_c : i_adda<0x81, &cpu::c_>{static constexpr auto name = "add_c";};
  struct add_d : i_adda<0x82, &cpu::d_>{static constexpr auto name = "add_d";};
  struct add_e : i_adda<0x83, &cpu::e_>{static constexpr auto name = "add_e";};
  struct add_h : i_adda<0x84, &cpu::h_>{static constexpr auto name = "add_h";};
  struct add_l : i_adda<0x85, &cpu::l_>{static constexpr auto name = "add_l";};
  struct add_m : meta::describe_instruction<0x86, 7, 1>
  {
    static constexpr auto name = "add_m";
    void operator()(cpu& cpu) const {cpu.adda(cpu.read_hl(), 0);}
  };
  struct add_a : i_adda<0x87, &cpu::a_>{static constexpr auto name = "add_a";};

  struct adc_b : meta::describe_instruction<0x88, 4, 1>
  {
    static constexpr auto name = "adc_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.b_, cpu.cy_);
    }
  };

  struct adc_c : meta::describe_instruction<0x89, 4, 1>
  {
    static constexpr auto name = "adc_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.c_, cpu.cy_);
    }
  };

  struct adc_d : meta::describe_instruction<0x8a, 4, 1>
  {
    static constexpr auto name = "adc_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.d_, cpu.cy_);
    }
  };

  struct adc_e : meta::describe_instruction<0x8b, 4, 1>
  {
    static constexpr auto name = "adc_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.e_, cpu.cy_);
    }
  };

  struct adc_h : meta::describe_instruction<0x8c, 4, 1>
  {
    static constexpr auto name = "adc_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.h_, cpu.cy_);
    }
  };

  struct adc_l : meta::describe_instruction<0x8d, 4, 1>
  {
    static constexpr auto name = "adc_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.l_, cpu.cy_);
    }
  };

  struct adc_m : meta::describe_instruction<0x8e, 7, 1>
  {
    static constexpr auto name = "adc_m";

    void operator()(cpu& cpu) const
    {
      cpu.adda(cpu.read_hl(), cpu.cy_);
    }
  };

  struct adc_a : meta::describe_instruction<0x8f, 4, 1>
  {
    static constexpr auto name = "adc_a";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.adda(cpu.a_, cpu.cy_);
    }
  };

  struct sub_b : meta::describe_instruction<0x90, 4, 1>
  {
    static constexpr auto name = "sub_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.b_, 0);
    }
  };

  struct sub_c : meta::describe_instruction<0x91, 4, 1>
  {
    static constexpr auto name = "sub_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.c_, 0);
    }
  };

  struct sub_d : meta::describe_instruction<0x92, 4, 1>
  {
    static constexpr auto name = "sub_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.d_, 0);
    }
  };

  struct sub_e : meta::describe_instruction<0x93, 4, 1>
  {
    static constexpr auto name = "sub_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.e_, 0);
    }
  };

  struct sub_h : meta::describe_instruction<0x94, 4, 1>
  {
    static constexpr auto name = "sub_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.h_, 0);
    }
  };

  struct sub_l : meta::describe_instruction<0x95, 4, 1>
  {
    static constexpr auto name = "sub_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.l_, 0);
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
      cpu.suba(cpu.a_, 0);
    }
  };

  struct sbb_b : meta::describe_instruction<0x98, 4, 1>
  {
    static constexpr auto name = "sbb_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.b_, cpu.cy_);
    }
  };

  struct sbb_c : meta::describe_instruction<0x99, 4, 1>
  {
    static constexpr auto name = "sbb_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.c_, cpu.cy_);
    }
  };

  struct sbb_d : meta::describe_instruction<0x9a, 4, 1>
  {
    static constexpr auto name = "sbb_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.d_, cpu.cy_);
    }
  };

  struct sbb_e : meta::describe_instruction<0x9b, 4, 1>
  {
    static constexpr auto name = "sbb_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.e_, cpu.cy_);
    }
  };

  struct sbb_h : meta::describe_instruction<0x9c, 4, 1>
  {
    static constexpr auto name = "sbb_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.h_, cpu.cy_);
    }
  };

  struct sbb_l : meta::describe_instruction<0x9d, 4, 1>
  {
    static constexpr auto name = "sbb_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.l_, cpu.cy_);
    }
  };

  struct sbb_m : meta::describe_instruction<0x9e, 7, 1>
  {
    static constexpr auto name = "sbb_m";

    void operator()(cpu& cpu) const
    {
      cpu.suba(cpu.read_hl(), cpu.cy_);
    }
  };

  struct sbb_a : meta::describe_instruction<0x9f, 4, 1>
  {
    static constexpr auto name = "sbb_a";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.a_, cpu.cy_);
    }
  };

  struct ana_b : i_ana<0xa0, &cpu::b_> {static constexpr auto name = "ana_b";};
  struct ana_c : i_ana<0xa1, &cpu::c_> {static constexpr auto name = "ana_c";};
  struct ana_d : i_ana<0xa2, &cpu::d_> {static constexpr auto name = "ana_d";};
  struct ana_e : i_ana<0xa3, &cpu::e_> {static constexpr auto name = "ana_e";};
  struct ana_h : i_ana<0xa4, &cpu::h_> {static constexpr auto name = "ana_h";};
  struct ana_l : i_ana<0xa5, &cpu::l_> {static constexpr auto name = "ana_l";};
  struct ana_m : meta::describe_instruction<0xa6, 7, 1>
  {
    static constexpr auto name = "ana_m";
    void operator()(cpu& cpu) const {cpu.ana(cpu.read_hl());}
  };
  struct ana_a : i_ana<0xa7, &cpu::a_> {static constexpr auto name = "ana_a";};

  struct xra_b : meta::describe_instruction<0xa8, 4, 1>
  {
    static constexpr auto name = "xra_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.b_);
    }
  };

  struct xra_c : meta::describe_instruction<0xa9, 4, 1>
  {
    static constexpr auto name = "xra_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.c_);
    }
  };

  struct xra_d : meta::describe_instruction<0xaa, 4, 1>
  {
    static constexpr auto name = "xra_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.d_);
    }
  };

  struct xra_e : meta::describe_instruction<0xab, 4, 1>
  {
    static constexpr auto name = "xra_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.e_);
    }
  };

  struct xra_h : meta::describe_instruction<0xac, 4, 1>
  {
    static constexpr auto name = "xra_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.h_);
    }
  };

  struct xra_l : meta::describe_instruction<0xad, 4, 1>
  {
    static constexpr auto name = "xra_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.xra(cpu.l_);
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
      cpu.xra(cpu.a_);
    }
  };

  struct ora_b : meta::describe_instruction<0xb0, 4, 1>
  {
    static constexpr auto name = "ora_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.b_);
    }
  };

  struct ora_c : meta::describe_instruction<0xb1, 4, 1>
  {
    static constexpr auto name = "ora_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.c_);
    }
  };

  struct ora_d : meta::describe_instruction<0xb2, 4, 1>
  {
    static constexpr auto name = "ora_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.d_);
    }
  };

  struct ora_e : meta::describe_instruction<0xb3, 4, 1>
  {
    static constexpr auto name = "ora_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.e_);
    }
  };

  struct ora_h : meta::describe_instruction<0xb4, 4, 1>
  {
    static constexpr auto name = "ora_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.h_);
    }
  };

  struct ora_l : meta::describe_instruction<0xb5, 4, 1>
  {
    static constexpr auto name = "ora_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.ora(cpu.l_);
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
      cpu.ora(cpu.a_);
    }
  };

  struct cmp_b : meta::describe_instruction<0xb8, 4, 1>
  {
    static constexpr auto name = "cmp_b";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.b_);
    }
  };

  struct cmp_c : meta::describe_instruction<0xb9, 4, 1>
  {
    static constexpr auto name = "cmp_c";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.c_);
    }
  };

  struct cmp_d : meta::describe_instruction<0xba, 4, 1>
  {
    static constexpr auto name = "cmp_d";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.d_);
    }
  };

  struct cmp_e : meta::describe_instruction<0xbb, 4, 1>
  {
    static constexpr auto name = "cmp_e";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.e_);
    }
  };

  struct cmp_h : meta::describe_instruction<0xbc, 4, 1>
  {
    static constexpr auto name = "cmp_h";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.h_);
    }
  };

  struct cmp_l : meta::describe_instruction<0xbd, 4, 1>
  {
    static constexpr auto name = "cmp_l";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.cmp(cpu.l_);
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
      cpu.cmp(cpu.a_);
    }
  };

  struct rnz : meta::describe_instruction<0xc0, 5, 1, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "rnz";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_ret(not cpu.z_);
    }
  };

  struct pop_b : meta::describe_instruction<0xc1, 10, 1>
  {
    static constexpr auto name = "pop_b";

    void operator()(cpu& cpu) const
    {
      std::tie(cpu.b_, cpu.c_) = cpu.pop();
    }
  };

  struct jnz : meta::describe_instruction<0xc2, 10, 3>
  {
    static constexpr auto name = "jnz";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.conditional_jump(cpu.operands_word(), not cpu.z_);
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
      return cpu.conditional_call(cpu.operands_word(), not cpu.z_);
    }
  };

  struct push_b : meta::describe_instruction<0xc5, 11, 1>
  {
    static constexpr auto name = "push_b";

    void operator()(cpu& cpu) const
    {
      cpu.push(cpu.b_, cpu.c_);
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
      return cpu.conditional_ret(cpu.z_);
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
      cpu.conditional_jump(cpu.operands_word(), cpu.z_);
    }
  };

  struct cz : meta::describe_instruction<0xcc, 11, 3, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "cz";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_call(cpu.operands_word(), cpu.z_);
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
      cpu.adda(cpu.op1(), cpu.cy_);
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
      return cpu.conditional_ret(not cpu.cy_);
    }
  };

  struct pop_d : meta::describe_instruction<0xd1, 10, 1>
  {
    static constexpr auto name = "pop_d";

    void operator()(cpu& cpu) const
    {
      std::tie(cpu.d_, cpu.e_) = cpu.pop();
    }
  };

  struct jnc : meta::describe_instruction<0xd2, 10, 3>
  {
    static constexpr auto name = "jnc";

    void operator()(cpu& cpu) const
    {
      cpu.conditional_jump(cpu.operands_word(), not cpu.cy_);
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
      return cpu.conditional_call(cpu.operands_word(), not cpu.cy_);
    }
  };

  struct push_d : meta::describe_instruction<0xd5, 11, 1>
  {
    static constexpr auto name = "push_d";

    void operator()(cpu& cpu) const
    {
      cpu.push(cpu.d_, cpu.e_);
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
      return cpu.conditional_ret(cpu.cy_);
    }
  };

  struct jc : meta::describe_instruction<0xda, 10, 3>
  {
    static constexpr auto name = "jc";

    void operator()(cpu& cpu) const
    {
      cpu.conditional_jump(cpu.operands_word(), cpu.cy_);
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
      return cpu.conditional_call(cpu.operands_word(), cpu.cy_ != 0);
    }
  };

  struct sbi : meta::describe_instruction<0xde, 7, 2>
  {
    static constexpr auto name = "sbi";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.suba(cpu.op1(), cpu.cy_);
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
      return cpu.conditional_ret(not cpu.p_);
    }
  };

  struct pop_h : meta::describe_instruction<0xe1, 10, 1>
  {
    static constexpr auto name = "pop_h";

    void operator()(cpu& cpu) const
    {
      std::tie(cpu.h_, cpu.l_) = cpu.pop();
    }
  };

  struct jpo : meta::describe_instruction<0xe2, 10, 3>
  {
    static constexpr auto name = "jpo";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.conditional_jump(cpu.operands_word(), not cpu.p_);
    }
  };

  struct xthl : meta::describe_instruction<0xe3, 18, 1>
  {
    static constexpr auto name = "xthl";

    void operator()(cpu& cpu) const
    {
      const auto h = cpu.h_;
      const auto l = cpu.l_;
      cpu.l_= cpu.memory_read_byte(cpu.sp_);
      cpu.h_= cpu.memory_read_byte(cpu.sp_ + 1);
      cpu.memory_write_byte(cpu.sp_, l);
      cpu.memory_write_byte(cpu.sp_ + 1, h);
    }
  };

  struct cpo : meta::describe_instruction<0xe4, 11, 3, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "cpo";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_call(cpu.operands_word(), not cpu.p_);
    }
  };

  struct push_h : meta::describe_instruction<0xe5, 11, 1>
  {
    static constexpr auto name = "push_h";

    void operator()(cpu& cpu) const
    {
      cpu.push(cpu.h_, cpu.l_);
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
      return cpu.conditional_ret(cpu.p_);
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
      cpu.conditional_jump(cpu.operands_word(), cpu.p_);
    }
  };

  struct xchg : meta::describe_instruction<0xeb, 5, 1>
  {
    static constexpr auto name = "xchg";

    void operator()(cpu& cpu) const noexcept
    {
      std::swap(cpu.d_, cpu.h_);
      std::swap(cpu.e_, cpu.l_);
    }
  };

  struct cpe : meta::describe_instruction<0xec, 11, 3, meta::constant_instruction<false, 6>>
  {
    static constexpr auto name = "cpe";

    auto operator()(cpu& cpu) const
    {
      return cpu.conditional_call(cpu.operands_word(), cpu.p_ != 0);
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
      return cpu.conditional_ret(not cpu.s_);
    }
  };

  struct pop_psw : meta::describe_instruction<0xf1, 10, 1>
  {
    static constexpr auto name = "pop_psw";

    void operator()(cpu& cpu) const
    {
      auto flags = std::uint8_t{};
      std::tie(cpu.a_, flags) = cpu.pop();

      cpu.s_  = flags & 0b10000000 ? true : false;
      cpu.z_  = flags & 0b01000000 ? true : false;
      cpu.ac_ = flags & 0b00010000 ? true : false;
      cpu.p_  = flags & 0b00000100 ? true : false;
      cpu.cy_ = flags & 0b00000001 ? true : false;
    }
  };

  struct jp : meta::describe_instruction<0xf2, 10, 3>
  {
    static constexpr auto name = "jp";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.conditional_jump(cpu.operands_word(), not cpu.s_);
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
      return cpu.conditional_call(cpu.operands_word(), not cpu.s_);
    }
  };

  struct push_psw : meta::describe_instruction<0xf5, 11, 1>
  {
    static constexpr auto name = "push_psw";

    void operator()(cpu& cpu) const
    {
      auto flags = std::uint8_t{0};

      if (cpu.s_)  flags |= 0b10000000;
      if (cpu.z_)  flags |= 0b01000000;
      if (cpu.ac_) flags |= 0b00010000;
      if (cpu.p_)  flags |= 0b00000100;
      if (cpu.cy_) flags |= 0b00000001;

      flags |=  0b00000010; // bit 1 is always 1
      flags &= ~0b00001000; // bit 3 is always 0
      flags &= ~0b00100000; // bit 5 is always 0

      cpu.push(cpu.a_, flags);
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
      return cpu.conditional_ret(cpu.s_);
    }
  };

  struct sphl : meta::describe_instruction<0xf9, 5, 1>
  {
    static constexpr auto name = "sphl";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.sp_ = cpu.hl();
    }
  };

  struct jm : meta::describe_instruction<0xfa, 10, 3>
  {
    static constexpr auto name = "jm";

    void operator()(cpu& cpu) const noexcept
    {
      cpu.conditional_jump(cpu.operands_word(), cpu.s_);
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
      return cpu.conditional_call(cpu.operands_word(), cpu.s_ != 0);
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
    unimplemented<0x08>,
    dad_b,
    ldax_b,
    dcx_b,
    inr_c,
    dcr_c,
    mvi_c,
    rrc,
    unimplemented<0x10>,
    lxi_d,
    stax_d,
    inx_d,
    inr_d,
    dcr_d,
    mvi_d,
    ral,
    unimplemented<0x18>,
    dad_d,
    ldax_d,
    dcx_d,
    inr_e,
    dcr_e,
    mvi_e,
    rar,
    unimplemented<0x20>,
    lxi_h,
    shld,
    inx_h,
    inr_h,
    dcr_h,
    mvi_h,
    daa,
    unimplemented<0x28>,
    dad_h,
    lhld,
    dcx_h,
    inr_l,
    dcr_l,
    mvi_l,
    cma,
    unimplemented<0x30>,
    lxi_sp,
    sta,
    inx_sp,
    inr_m,
    dcr_m,
    mvi_m,
    stc,
    unimplemented<0x38>,
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
    unimplemented<0xcb>,
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
    unimplemented<0xd9>,
    jc,
    in,
    cc,
    unimplemented<0xdd>,
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
    unimplemented<0xed>,
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
    unimplemented<0xfd>,
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
    : a_{0}
    , b_{0}
    , c_{0}
    , d_{0}
    , e_{0}
    , h_{0}
    , l_{0}
    , sp_{0}
    , cy_{0}
    , p_{0}
    , ac_{0}
    , z_{0}
    , s_{0}
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
      << (cpu.z_ ? "z" : ".")
      << (cpu.s_ ? "s" : ".")
      << (cpu.p_ ? "p" : ".")
      << (cpu.cy_ ? "c" : ".")
      << (cpu.ac_ ? "a" : ".")
      << std::hex
      << "  A $" << std::setfill('0') << std::setw(2) << +cpu.a__
      << " B $"  << std::setfill('0') << std::setw(2) << +cpu.b__
      << " C $"  << std::setfill('0') << std::setw(2) << +cpu.c_
      << " D $"  << std::setfill('0') << std::setw(2) << +cpu.d_
      << " E $"  << std::setfill('0') << std::setw(2) << +cpu.e_
      << " H $"  << std::setfill('0') << std::setw(2) << +cpu.h_
      << " L $"  << std::setfill('0') << std::setw(2) << +cpu.l_
      << " SP "  << std::setfill('0') << std::setw(4) << +cpu.sp_
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
    return (h_ << 8) | l_;
  }

  [[nodiscard]]
  std::uint16_t
  bc()
  const noexcept
  {
    return (b_ << 8) | c_;
  }

  [[nodiscard]]
  std::uint16_t
  de()
  const noexcept
  {
    return (d_ << 8) | e_;
  }

  void
  push(std::uint8_t high, std::uint8_t low)
  {
    memory_write_byte(sp_ - 1, high);
    memory_write_byte(sp_ - 2, low);
    sp_ -= 2;
  }

  [[nodiscard]]
  std::tuple<std::uint8_t, std::uint8_t>
  pop()
  {
    const auto low = memory_read_byte(sp_);
    const auto high = memory_read_byte(sp_ + 1);
    sp_ += 2;
    return {high, low};
  }

  [[nodiscard]]
  std::uint16_t
  pop_word()
  {
    const auto low = memory_read_byte(sp_);
    const auto high = memory_read_byte(sp_ + 1);
    sp_ += 2;
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
  std::uint8_t
  a()
  const noexcept
  {
    return a_;
  }

  [[nodiscard]]
  std::uint8_t&
  a()
  noexcept
  {
    return a_;
  }

  [[nodiscard]]
  std::uint8_t
  c()
  const noexcept
  {
    return c_;
  }

  [[nodiscard]]
  std::uint8_t
  e()
  const noexcept
  {
    return e_;
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
    ac_ = (res & 0x0f) == 0;
    z_ = res == 0;
    s_ = (res & 0b10000000) != 0;
    p_ = util::parity(res);
    return res;
  }

  [[nodiscard]]
  std::uint8_t
  dcr(std::uint8_t value)
  noexcept
  {
    const std::uint8_t res = value - 1;
    ac_ = not ((res & 0x0f) == 0x0f);
    z_ = res == 0;
    s_ = (res & 0b10000000) != 0;
    p_ = util::parity(res);
    return res;
  }

  void
  adda(std::uint8_t val, bool carry)
  noexcept
  {
    const std::uint16_t res = a_ + val + carry;
    z_  = (res & 0xff) == 0;
    s_  = (res & 0b10000000) != 0;
    cy_ = (res & 0b100000000) != 0;
    ac_ = (a_ ^ res ^ val) & 0x10;
    p_  = util::parity(res & 0xff);
    a_ = res & 0xff;
  }

  void
  suba(std::uint8_t val, bool carry)
  noexcept
  {
    const std::int16_t res = a_ - val - carry;
    z_ = (res & 0xff) == 0;
    s_ = (res & 0b10000000) != 0;
    cy_ = (res & 0b100000000) != 0;
    ac_ = ~(a_ ^ res ^ val) & 0x10;
    p_ = util::parity(res & 0xff);
    a_ = res & 0xff;
  }

  void
  ora(std::uint8_t val)
  noexcept
  {
    a_ |= val;
    cy_ = false;
    ac_ = false;
    z_ = a_ == 0;
    s_ = (a_ & 0x80) != 0;
    p_ = util::parity(a_);
  }

  void
  ana(std::uint8_t val)
  noexcept
  {
    const std::uint8_t res = a_ & val;
    cy_ = false;
    ac_ = ((a_ | val) & 0x08) != 0;
    z_ = res == 0;
    s_ = (res & 0x80) != 0;
    p_ = util::parity(res);
    a_ = res;
  }

  void
  xra(std::uint8_t val)
  noexcept
  {
    a_ ^= val;
    cy_ = false;
    ac_ = false;
    z_ = a_ == 0;
    s_ = (a_ & 0x80) != 0;
    p_ = util::parity(a_);
  }

  void
  cmp(std::uint8_t val)
  noexcept
  {
    const std::int16_t res = a_ - val;
    cy_ = (res & 0b100000000) != 0;
    ac_ = ~(a_ ^ res ^ val) & 0x10;
    z_ = (res & 0xff) == 0;
    s_ = (res & 0x80) != 0;
    p_ = util::parity(res);
  }

  void
  increment_cycles(std::uint64_t nb_cycles)
  noexcept
  {
    cycles_ += nb_cycles;
  }
};

/*------------------------------------------------------------------------------------------------*/

} // namespace cpp8080::specific
