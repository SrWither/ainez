#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <unordered_map>
#include <vector>

enum class AddressingMode {
  Immediate,
  ZeroPage,
  ZeroPage_X,
  ZeroPage_Y,
  Absolute,
  Absolute_X,
  Absolute_Y,
  Indirect_X,
  Indirect_Y,
  NoneAddressing,
};

enum CpuFlags : uint8_t {
  CARRY = 0b00000001,
  ZERO = 0b00000010,
  INTERRUPT_DISABLE = 0b00000100,
  DECIMAL_MODE = 0b00001000,
  BREAK = 0b00010000,
  BREAK2 = 0b00100000,
  OVERFLOW = 0b01000000,
  NEGATIV = 0b10000000
};

CpuFlags cpuflags_from_bits(uint8_t bits);

constexpr CpuFlags operator|(CpuFlags lhs, CpuFlags rhs) {
  return static_cast<CpuFlags>(static_cast<uint8_t>(lhs) |
                               static_cast<uint8_t>(rhs));
}

constexpr CpuFlags &operator|=(CpuFlags &lhs, CpuFlags rhs) {
  lhs = lhs | rhs;
  return lhs;
}

constexpr CpuFlags operator&(CpuFlags a, CpuFlags b) {
  return static_cast<CpuFlags>(static_cast<uint8_t>(a) &
                               static_cast<uint8_t>(b));
}

constexpr CpuFlags operator~(CpuFlags a) {
  return static_cast<CpuFlags>(~static_cast<uint8_t>(a));
}

constexpr void operator&=(CpuFlags &a, CpuFlags b) {
  a = static_cast<CpuFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

const uint16_t STACK = 0x0100;
const uint8_t STACK_RESET = 0xfd;

struct OpCode {
  uint8_t code;
  const char *mnemonic;
  uint8_t len;
  uint8_t cycles;
  AddressingMode mode;
};

OpCode createOpCode(uint8_t code, const char *mnemonic, uint8_t len,
                    uint8_t cycles, AddressingMode mode);

const std::vector<OpCode> CPU_OPS_CODES = {
    createOpCode(0x00, "BRK", 1, 7, AddressingMode::NoneAddressing),
    createOpCode(0xea, "NOP", 1, 2, AddressingMode::NoneAddressing),

    /* Arithmetic */
    createOpCode(0x69, "ADC", 2, 2, AddressingMode::Immediate),
    createOpCode(0x65, "ADC", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0x75, "ADC", 2, 4, AddressingMode::ZeroPage_X),
    createOpCode(0x6d, "ADC", 3, 4, AddressingMode::Absolute),
    createOpCode(0x7d, "ADC", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_X),
    createOpCode(0x79, "ADC", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_Y),
    createOpCode(0x61, "ADC", 2, 6, AddressingMode::Indirect_X),
    createOpCode(0x71, "ADC", 2, 5 /*+1 if page crossed*/,
                 AddressingMode::Indirect_Y),

    createOpCode(0xe9, "SBC", 2, 2, AddressingMode::Immediate),
    createOpCode(0xe5, "SBC", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0xf5, "SBC", 2, 4, AddressingMode::ZeroPage_X),
    createOpCode(0xed, "SBC", 3, 4, AddressingMode::Absolute),
    createOpCode(0xfd, "SBC", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_X),
    createOpCode(0xf9, "SBC", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_Y),
    createOpCode(0xe1, "SBC", 2, 6, AddressingMode::Indirect_X),
    createOpCode(0xf1, "SBC", 2, 5 /*+1 if page crossed*/,
                 AddressingMode::Indirect_Y),

    createOpCode(0x29, "AND", 2, 2, AddressingMode::Immediate),
    createOpCode(0x25, "AND", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0x35, "AND", 2, 4, AddressingMode::ZeroPage_X),
    createOpCode(0x2d, "AND", 3, 4, AddressingMode::Absolute),
    createOpCode(0x3d, "AND", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_X),
    createOpCode(0x39, "AND", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_Y),
    createOpCode(0x21, "AND", 2, 6, AddressingMode::Indirect_X),
    createOpCode(0x31, "AND", 2, 5 /*+1 if page crossed*/,
                 AddressingMode::Indirect_Y),

    createOpCode(0x49, "EOR", 2, 2, AddressingMode::Immediate),
    createOpCode(0x45, "EOR", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0x55, "EOR", 2, 4, AddressingMode::ZeroPage_X),
    createOpCode(0x4d, "EOR", 3, 4, AddressingMode::Absolute),
    createOpCode(0x5d, "EOR", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_X),
    createOpCode(0x59, "EOR", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_Y),
    createOpCode(0x41, "EOR", 2, 6, AddressingMode::Indirect_X),
    createOpCode(0x51, "EOR", 2, 5 /*+1 if page crossed*/,
                 AddressingMode::Indirect_Y),

    createOpCode(0x09, "ORA", 2, 2, AddressingMode::Immediate),
    createOpCode(0x05, "ORA", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0x15, "ORA", 2, 4, AddressingMode::ZeroPage_X),
    createOpCode(0x0d, "ORA", 3, 4, AddressingMode::Absolute),
    createOpCode(0x1d, "ORA", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_X),
    createOpCode(0x19, "ORA", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_Y),
    createOpCode(0x01, "ORA", 2, 6, AddressingMode::Indirect_X),
    createOpCode(0x11, "ORA", 2, 5 /*+1 if page crossed*/,
                 AddressingMode::Indirect_Y),

    /* Shifts */
    createOpCode(0x0a, "ASL", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0x06, "ASL", 2, 5, AddressingMode::ZeroPage),
    createOpCode(0x16, "ASL", 2, 6, AddressingMode::ZeroPage_X),
    createOpCode(0x0e, "ASL", 3, 6, AddressingMode::Absolute),
    createOpCode(0x1e, "ASL", 3, 7, AddressingMode::Absolute_X),

    createOpCode(0x4a, "LSR", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0x46, "LSR", 2, 5, AddressingMode::ZeroPage),
    createOpCode(0x56, "LSR", 2, 6, AddressingMode::ZeroPage_X),
    createOpCode(0x4e, "LSR", 3, 6, AddressingMode::Absolute),
    createOpCode(0x5e, "LSR", 3, 7, AddressingMode::Absolute_X),

    createOpCode(0x2a, "ROL", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0x26, "ROL", 2, 5, AddressingMode::ZeroPage),
    createOpCode(0x36, "ROL", 2, 6, AddressingMode::ZeroPage_X),
    createOpCode(0x2e, "ROL", 3, 6, AddressingMode::Absolute),
    createOpCode(0x3e, "ROL", 3, 7, AddressingMode::Absolute_X),

    createOpCode(0x6a, "ROR", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0x66, "ROR", 2, 5, AddressingMode::ZeroPage),
    createOpCode(0x76, "ROR", 2, 6, AddressingMode::ZeroPage_X),
    createOpCode(0x6e, "ROR", 3, 6, AddressingMode::Absolute),
    createOpCode(0x7e, "ROR", 3, 7, AddressingMode::Absolute_X),

    createOpCode(0xe6, "INC", 2, 5, AddressingMode::ZeroPage),
    createOpCode(0xf6, "INC", 2, 6, AddressingMode::ZeroPage_X),
    createOpCode(0xee, "INC", 3, 6, AddressingMode::Absolute),
    createOpCode(0xfe, "INC", 3, 7, AddressingMode::Absolute_X),

    createOpCode(0xe8, "INX", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0xc8, "INY", 1, 2, AddressingMode::NoneAddressing),

    createOpCode(0xc6, "DEC", 2, 5, AddressingMode::ZeroPage),
    createOpCode(0xd6, "DEC", 2, 6, AddressingMode::ZeroPage_X),
    createOpCode(0xce, "DEC", 3, 6, AddressingMode::Absolute),
    createOpCode(0xde, "DEC", 3, 7, AddressingMode::Absolute_X),

    createOpCode(0xca, "DEX", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0x88, "DEY", 1, 2, AddressingMode::NoneAddressing),

    createOpCode(0xc9, "CMP", 2, 2, AddressingMode::Immediate),
    createOpCode(0xc5, "CMP", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0xd5, "CMP", 2, 4, AddressingMode::ZeroPage_X),
    createOpCode(0xcd, "CMP", 3, 4, AddressingMode::Absolute),
    createOpCode(0xdd, "CMP", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_X),
    createOpCode(0xd9, "CMP", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_Y),
    createOpCode(0xc1, "CMP", 2, 6, AddressingMode::Indirect_X),
    createOpCode(0xd1, "CMP", 2, 5 /*+1 if page crossed*/,
                 AddressingMode::Indirect_Y),

    createOpCode(0xc0, "CPY", 2, 2, AddressingMode::Immediate),
    createOpCode(0xc4, "CPY", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0xcc, "CPY", 3, 4, AddressingMode::Absolute),

    createOpCode(0xe0, "CPX", 2, 2, AddressingMode::Immediate),
    createOpCode(0xe4, "CPX", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0xec, "CPX", 3, 4, AddressingMode::Absolute),

    /* Branching */

    createOpCode(0x4c, "JMP", 3, 3,
                 AddressingMode::NoneAddressing), // AddressingMode that acts as
                                                  // Immidiate
    createOpCode(0x6c, "JMP", 3, 5,
                 AddressingMode::NoneAddressing), // AddressingMode:Indirect
                                                  // with 6502 bug

    createOpCode(0x20, "JSR", 3, 6, AddressingMode::NoneAddressing),
    createOpCode(0x60, "RTS", 1, 6, AddressingMode::NoneAddressing),

    createOpCode(0x40, "RTI", 1, 6, AddressingMode::NoneAddressing),

    createOpCode(0xd0, "BNE", 2,
                 2 /*(+1 if branch succeeds +2 if to a new page)*/,
                 AddressingMode::NoneAddressing),
    createOpCode(0x70, "BVS", 2,
                 2 /*(+1 if branch succeeds +2 if to a new page)*/,
                 AddressingMode::NoneAddressing),
    createOpCode(0x50, "BVC", 2,
                 2 /*(+1 if branch succeeds +2 if to a new page)*/,
                 AddressingMode::NoneAddressing),
    createOpCode(0x30, "BMI", 2,
                 2 /*(+1 if branch succeeds +2 if to a new page)*/,
                 AddressingMode::NoneAddressing),
    createOpCode(0xf0, "BEQ", 2,
                 2 /*(+1 if branch succeeds +2 if to a new page)*/,
                 AddressingMode::NoneAddressing),
    createOpCode(0xb0, "BCS", 2,
                 2 /*(+1 if branch succeeds +2 if to a new page)*/,
                 AddressingMode::NoneAddressing),
    createOpCode(0x90, "BCC", 2,
                 2 /*(+1 if branch succeeds +2 if to a new page)*/,
                 AddressingMode::NoneAddressing),
    createOpCode(0x10, "BPL", 2,
                 2 /*(+1 if branch succeeds +2 if to a new page)*/,
                 AddressingMode::NoneAddressing),

    createOpCode(0x24, "BIT", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0x2c, "BIT", 3, 4, AddressingMode::Absolute),

    /* Stores, Loads */
    createOpCode(0xa9, "LDA", 2, 2, AddressingMode::Immediate),
    createOpCode(0xa5, "LDA", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0xb5, "LDA", 2, 4, AddressingMode::ZeroPage_X),
    createOpCode(0xad, "LDA", 3, 4, AddressingMode::Absolute),
    createOpCode(0xbd, "LDA", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_X),
    createOpCode(0xb9, "LDA", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_Y),
    createOpCode(0xa1, "LDA", 2, 6, AddressingMode::Indirect_X),
    createOpCode(0xb1, "LDA", 2, 5 /*+1 if page crossed*/,
                 AddressingMode::Indirect_Y),

    createOpCode(0xa2, "LDX", 2, 2, AddressingMode::Immediate),
    createOpCode(0xa6, "LDX", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0xb6, "LDX", 2, 4, AddressingMode::ZeroPage_Y),
    createOpCode(0xae, "LDX", 3, 4, AddressingMode::Absolute),
    createOpCode(0xbe, "LDX", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_Y),

    createOpCode(0xa0, "LDY", 2, 2, AddressingMode::Immediate),
    createOpCode(0xa4, "LDY", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0xb4, "LDY", 2, 4, AddressingMode::ZeroPage_X),
    createOpCode(0xac, "LDY", 3, 4, AddressingMode::Absolute),
    createOpCode(0xbc, "LDY", 3, 4 /*+1 if page crossed*/,
                 AddressingMode::Absolute_X),

    createOpCode(0x85, "STA", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0x95, "STA", 2, 4, AddressingMode::ZeroPage_X),
    createOpCode(0x8d, "STA", 3, 4, AddressingMode::Absolute),
    createOpCode(0x9d, "STA", 3, 5, AddressingMode::Absolute_X),
    createOpCode(0x99, "STA", 3, 5, AddressingMode::Absolute_Y),
    createOpCode(0x81, "STA", 2, 6, AddressingMode::Indirect_X),
    createOpCode(0x91, "STA", 2, 6, AddressingMode::Indirect_Y),

    createOpCode(0x86, "STX", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0x96, "STX", 2, 4, AddressingMode::ZeroPage_Y),
    createOpCode(0x8e, "STX", 3, 4, AddressingMode::Absolute),

    createOpCode(0x84, "STY", 2, 3, AddressingMode::ZeroPage),
    createOpCode(0x94, "STY", 2, 4, AddressingMode::ZeroPage_X),
    createOpCode(0x8c, "STY", 3, 4, AddressingMode::Absolute),

    /* Flags clear */

    createOpCode(0xD8, "CLD", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0x58, "CLI", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0xb8, "CLV", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0x18, "CLC", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0x38, "SEC", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0x78, "SEI", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0xf8, "SED", 1, 2, AddressingMode::NoneAddressing),

    createOpCode(0xaa, "TAX", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0xa8, "TAY", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0xba, "TSX", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0x8a, "TXA", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0x9a, "TXS", 1, 2, AddressingMode::NoneAddressing),
    createOpCode(0x98, "TYA", 1, 2, AddressingMode::NoneAddressing),

    /* Stack */
    createOpCode(0x48, "PHA", 1, 3, AddressingMode::NoneAddressing),
    createOpCode(0x68, "PLA", 1, 4, AddressingMode::NoneAddressing),
    createOpCode(0x08, "PHP", 1, 3, AddressingMode::NoneAddressing),
    createOpCode(0x28, "PLP", 1, 4, AddressingMode::NoneAddressing),

};

const std::unordered_map<uint8_t, const OpCode *> OPCODES_MAP = []() {
  std::unordered_map<uint8_t, const OpCode *> map;
  for (const OpCode &cpuop : CPU_OPS_CODES) {
    map[cpuop.code] = &cpuop;
  }
  return map;
}();

class NesCpu {
public:
  uint8_t register_a;
  uint8_t register_x;
  uint8_t register_y;
  CpuFlags status;
  uint16_t program_counter;
  uint8_t stack_pointer;
  std::array<uint8_t, 0xFFFF> memory;

  NesCpu();

  void ldy(AddressingMode mode);
  void ldx(AddressingMode mode);
  void lda(AddressingMode mode);
  void set_register_a(uint8_t value);
  void andd(AddressingMode mode);
  void eor(AddressingMode mode);
  void ora(AddressingMode mode);
  void tax();
  void inx();
  void iny();
  void sta(AddressingMode mode);
  void update_zero_and_negative_flags(uint8_t result);
  void update_negative_flags(uint8_t result);

  // Memory
  uint8_t mem_read(uint16_t addr);
  uint16_t mem_read_u16(uint16_t pos);

  void mem_write(uint16_t addr, uint8_t data);
  void mem_write_u16(uint16_t pos, uint16_t data);

  void load(std::vector<uint8_t> program);
  void reset();
  void run();
  void load_and_run(std::vector<uint8_t> program);
  void set_carry_flag();
  void clear_carry_flag();
  void add_to_register_a(uint8_t data);
  void sbc(AddressingMode mode);
  void adc(AddressingMode mode);

  uint8_t stack_pop();
  void stack_push(uint8_t data);
  uint16_t stack_pop_u16();
  void stack_push_u16(uint16_t data);

  void asl_accumulator();
  uint8_t asl(AddressingMode mode);

  void lsr_accumulator();
  uint8_t lsr(AddressingMode mode);

  void rol_accumulator();
  uint8_t rol(AddressingMode mode);

  void ror_accumulator();
  uint8_t ror(AddressingMode mode);

  uint8_t inc(AddressingMode mode);

  void dey();
  void dex();
  uint8_t dec(AddressingMode mode);

  void pla();
  void plp();
  void php();

  void bit(AddressingMode mode);

  void compare(AddressingMode mode, uint8_t compare_with);
  void branch(bool condition);

  uint16_t get_operand_address(AddressingMode mode);

  /* template <typename T> void run_with_callback(T callback); */
  template <typename T> void run_with_callback(T &&callback) {
    const std::unordered_map<uint8_t, const OpCode *> &opcodes = OPCODES_MAP;
    while (true) {

      /* std::cout << "before program_counter: " << std::hex */
      /*           << this->program_counter << "\n" */
      /*           << std::endl; */

      uint8_t code = this->mem_read(this->program_counter);
      this->program_counter += 1;
      uint16_t program_counter_state = this->program_counter;
      /* std::cout << "CODE ERROR: " << std::bitset<8>(code) << "\n"; */
      auto opcode = opcodes.at(code);

      std::cout << "=================\n";
      std::cout << "code: " << std::bitset<8>(code) << "\n"
                << "program_counter: " << std::hex << this->program_counter
                << "\n"
                << "program_counter_state: " << std::hex
                << program_counter_state << "\n"
                << "opcode: " << opcode->mnemonic
                << "\n=================" << std::endl;

      switch (code) {
      case 0xa9:
      case 0xa5:
      case 0xb5:
      case 0xad:
      case 0xbd:
      case 0xb9:
      case 0xa1:
      case 0xb1: {
        this->lda(opcode->mode);
        break;
      }

      case 0xaa: {
        this->tax();
        break;
      }

      case 0xe8: {
        this->inx();
        break;
      }

      case 0x00: {
        return;
      }

      case 0xd8: {
        this->status &= ~CpuFlags::DECIMAL_MODE;
        break;
      }
      case 0x58: {
        this->status &= ~CpuFlags::INTERRUPT_DISABLE;
        break;
      }
      case 0xb8: {
        this->status &= ~CpuFlags::OVERFLOW;
        break;
      }
      case 0x18: {
        this->clear_carry_flag();
        break;
      }
      case 0x38: {
        this->set_carry_flag();
        break;
      }
      case 0x78: {
        this->status |= CpuFlags::INTERRUPT_DISABLE;
        break;
      }
      case 0xf8: {
        this->status |= CpuFlags::DECIMAL_MODE;
        break;
      }
      case 0x48: {
        this->stack_push(this->register_a);
        break;
      }

      case 0x68: {
        this->pla();
        break;
      }

      case 0x08: {
        this->php();
        break;
      }

      case 0x28: {
        this->plp();
        break;
      }

      case 0x69:
      case 0x65:
      case 0x75:
      case 0x6d:
      case 0x7d:
      case 0x79:
      case 0x61:
      case 0x71: {
        this->adc(opcode->mode);
        break;
      }

      case 0xe9:
      case 0xe5:
      case 0xf5:
      case 0xed:
      case 0xfd:
      case 0xf9:
      case 0xe1:
      case 0xf1: {
        this->sbc(opcode->mode);
        break;
      }

      case 0x29:
      case 0x25:
      case 0x35:
      case 0x2d:
      case 0x3d:
      case 0x39:
      case 0x21:
      case 0x31: {
        this->andd(opcode->mode);
        break;
      }

      case 0x49:
      case 0x45:
      case 0x55:
      case 0x4d:
      case 0x5d:
      case 0x59:
      case 0x41:
      case 0x51: {
        this->eor(opcode->mode);
        break;
      }

      case 0x09:
      case 0x05:
      case 0x15:
      case 0x0d:
      case 0x1d:
      case 0x19:
      case 0x01:
      case 0x11: {
        this->ora(opcode->mode);
        break;
      }

      case 0x46:
      case 0x56:
      case 0x4e:
      case 0x5e: {
        this->lsr(opcode->mode);
        break;
      }

      case 0x4a: {
        this->lsr_accumulator();
        break;
      }

      case 0x06:
      case 0x16:
      case 0x0e:
      case 0x1e: {
        this->asl(opcode->mode);
        break;
      }

      case 0x0a: {
        this->asl_accumulator();
      }

      case 0x2a: {
        this->rol_accumulator();
        break;
      }

      case 0x26:
      case 0x36:
      case 0x2e:
      case 0x3e: {
        this->rol(opcode->mode);
        break;
      }

      case 0x6a: {
        this->ror_accumulator();
        break;
      }

      case 0x66:
      case 0x76:
      case 0x6e:
      case 0x7e: {
        this->ror(opcode->mode);
        break;
      }

      case 0xe6:
      case 0xf6:
      case 0xee:
      case 0xfe: {
        this->inc(opcode->mode);
        break;
      }

      case 0xc8: {
        this->iny();
      }

      case 0xc6:
      case 0xd6:
      case 0xce:
      case 0xde: {
        this->dec(opcode->mode);
        break;
      }

      case 0xca: {
        this->dex();
        break;
      }

      case 0x88: {
        this->dey();
        break;
      }

      case 0xc9:
      case 0xc5:
      case 0xd5:
      case 0xcd:
      case 0xdd:
      case 0xd9:
      case 0xc1:
      case 0xd1: {
        this->compare(opcode->mode, this->register_a);
        break;
      }

      case 0xc0:
      case 0xc4:
      case 0xcc: {
        this->compare(opcode->mode, this->register_y);
        break;
      }

      case 0xe0:
      case 0xe4:
      case 0xec: {
        this->compare(opcode->mode, this->register_x);
        break;
      }

      case 0x4c: {
        uint16_t mem_address = this->mem_read_u16(this->program_counter);
        this->program_counter = mem_address;
        break;
      }

      case 0x6c: {
        uint16_t mem_address = this->mem_read_u16(this->program_counter);
        uint16_t indirect_ref;
        if ((mem_address & 0x00FF) == 0x00FF) {
          uint8_t lo = this->mem_read(mem_address);
          uint8_t hi = this->mem_read(mem_address & 0xFF00);
          indirect_ref = (static_cast<uint16_t>(hi) << 8) | lo;
        } else {
          indirect_ref = this->mem_read_u16(mem_address);
        }
        this->program_counter = indirect_ref;
        break;
      }

      case 0x20: {
        this->stack_push_u16(this->program_counter + 2 - 1);
        uint16_t target_address = this->mem_read_u16(this->program_counter);
        this->program_counter = target_address;
        break;
      }

      case 0x60: {
        this->program_counter = this->stack_pop_u16() + 1;
        break;
      }

      case 0x40: {
        this->status = static_cast<CpuFlags>(this->stack_pop());
        this->status &= ~CpuFlags::BREAK;
        this->status |= CpuFlags::BREAK2;

        this->program_counter = stack_pop_u16();
        break;
      }

      case 0xd0: {
        this->branch(
            !static_cast<bool>(status & static_cast<uint8_t>(CpuFlags::ZERO)));
        break;
      }

      case 0x70: {
        this->branch(static_cast<bool>(
            status & static_cast<uint8_t>(CpuFlags::OVERFLOW)));
        break;
      }

      case 0x50: {
        this->branch(!static_cast<bool>(
            status & static_cast<uint8_t>(CpuFlags::OVERFLOW)));
        break;
      }

      case 0x10: {
        this->branch(!static_cast<bool>(
            status & static_cast<uint8_t>(CpuFlags::NEGATIV)));
        break;
      }

      case 0x30: {
        this->branch(static_cast<bool>(
            status & static_cast<uint8_t>(CpuFlags::NEGATIV)));
        break;
      }

      case 0xf0: {
        this->branch(
            static_cast<bool>(status & static_cast<uint8_t>(CpuFlags::ZERO)));
        break;
      }

      case 0xb0: {
        this->branch(
            static_cast<bool>(status & static_cast<uint8_t>(CpuFlags::CARRY)));
        break;
      }

      case 0x90: {
        this->branch(
            !static_cast<bool>(status & static_cast<uint8_t>(CpuFlags::CARRY)));
        break;
      }

      case 0x24:
      case 0x2c: {
        this->bit(opcode->mode);
        break;
      }

      case 0x85:
      case 0x95:
      case 0x8d:
      case 0x9d:
      case 0x99:
      case 0x81:
      case 0x91: {
        this->sta(opcode->mode);
        break;
      }

      case 0x86:
      case 0x96:
      case 0x8e: {
        uint16_t addr = this->get_operand_address(opcode->mode);
        this->mem_write(addr, this->register_x);
        break;
      }

      case 0x84:
      case 0x94:
      case 0x8c: {
        uint16_t addr = this->get_operand_address(opcode->mode);
        this->mem_write(addr, this->register_y);
        break;
      }

      case 0xa2:
      case 0xa6:
      case 0xb6:
      case 0xae:
      case 0xbe: {
        this->ldx(opcode->mode);
        break;
      }

      case 0xa0:
      case 0xa4:
      case 0xb4:
      case 0xac:
      case 0xbc: {
        this->ldy(opcode->mode);
        break;
      }

      case 0xea: {

        break;
      }

      case 0xa8: {
        this->register_y = this->register_a;
        this->update_zero_and_negative_flags(this->register_y);
        break;
      }

      case 0xba: {
        this->register_x = this->stack_pointer;
        this->update_zero_and_negative_flags(this->register_x);
        break;
      }

      case 0x8a: {
        this->register_a = this->register_x;
        this->update_zero_and_negative_flags(this->register_a);
        break;
      }

      case 0x9a: {
        this->stack_pointer = this->register_x;
        break;
      }

      case 0x98: {
        this->register_a = this->register_y;
        this->update_zero_and_negative_flags(this->register_a);
        break;
      }

      default: {
        break;
      }
      }

      if (program_counter_state == this->program_counter) {
        this->program_counter += static_cast<uint16_t>(opcode->len - 1);
      }

      callback(*this);
    };
  };
};
