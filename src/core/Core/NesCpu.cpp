#include "Core/NesCpu.hpp"
#include <cstddef>
#include <cstring>
#include <iostream>
#include <stdexcept>

CpuFlags cpuflags_from_bits(uint8_t bits) {
  CpuFlags flags = static_cast<CpuFlags>(0);
  if (bits & CARRY) {
    flags |= CARRY;
  }
  if (bits & ZERO) {
    flags |= ZERO;
  }
  if (bits & INTERRUPT_DISABLE) {
    flags |= INTERRUPT_DISABLE;
  }
  if (bits & DECIMAL_MODE) {
    flags |= DECIMAL_MODE;
  }
  if (bits & BREAK) {
    flags |= BREAK;
  }
  if (bits & BREAK2) {
    flags |= BREAK2;
  }
  if (bits & OVERFLOW) {
    flags |= OVERFLOW;
  }
  if (bits & NEGATIV) {
    flags |= NEGATIV;
  }
  return flags;
}

NesCpu::NesCpu() {
  this->register_a = 0;
  this->status = cpuflags_from_bits(0b100100);
  this->program_counter = 0;
  this->register_x = 0;
  this->register_y = 0;
}

void NesCpu::lda(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t value = this->mem_read(addr);
  this->set_register_a(value);
}

void NesCpu::ldy(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t value = this->mem_read(addr);

  this->register_y = value;
  this->update_zero_and_negative_flags(register_y);
}

void NesCpu::ldx(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t value = this->mem_read(addr);

  this->register_x = value;
  this->update_zero_and_negative_flags(register_x);
}

void NesCpu::tax() {
  this->register_x = this->register_a;
  this->update_zero_and_negative_flags(this->register_x);
}

void NesCpu::inx() {
  this->register_x = static_cast<uint8_t>(this->register_x + 1);
  update_zero_and_negative_flags(this->register_x);
}

void NesCpu::iny() {
  this->register_y = static_cast<uint8_t>(this->register_y + 1);
  update_zero_and_negative_flags(this->register_y);
}

void NesCpu::sta(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  this->mem_write(addr, this->register_a);
}

void NesCpu::set_register_a(uint8_t value) {
  this->register_a = value;
  this->update_zero_and_negative_flags(this->register_a);
}

void NesCpu::andd(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t data = this->mem_read(addr);
  this->set_register_a(data & this->register_a);
}

void NesCpu::eor(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t data = this->mem_read(addr);
  this->set_register_a(data ^ this->register_a);
}

void NesCpu::ora(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t data = this->mem_read(addr);
  this->set_register_a(data | this->register_a);
}

void NesCpu::update_zero_and_negative_flags(uint8_t result) {
  if (result == 0) {
    this->status |= CpuFlags::ZERO;
  } else {
    this->status &= ~CpuFlags::ZERO;
  }

  if ((result >> 7) == 1) {
    this->status |= CpuFlags::NEGATIV;
  } else {
    this->status &= ~CpuFlags::NEGATIV;
  }
}

void NesCpu::update_negative_flags(uint8_t result) {
  if ((result >> 7) == 1) {
    this->status = this->status | CpuFlags::NEGATIV;
  } else {
    this->status = this->status & ~CpuFlags::NEGATIV;
  }
}

// Memory Management

uint8_t NesCpu::mem_read(uint16_t addr) {
  return this->memory[static_cast<std::size_t>(addr)];
}

uint16_t NesCpu::mem_read_u16(uint16_t pos) {
  uint8_t lo = this->mem_read(pos);
  uint8_t hi = this->mem_read(pos + 1);
  return (hi << 8) | lo;
}

void NesCpu::mem_write(uint16_t addr, uint8_t data) {
  this->memory[static_cast<std::size_t>(addr)] = data;
}

void NesCpu::mem_write_u16(uint16_t pos, uint16_t data) {
  uint8_t hi = (data >> 8) & 0xff;
  uint8_t lo = data & 0xff;
  this->mem_write(pos, lo);
  this->mem_write(pos + 1, hi);
}

void NesCpu::load(std::vector<uint8_t> program) {
  std::memcpy(&this->memory[0x0600], program.data(), program.size());
  this->mem_write_u16(0xFFFC, 0x0600);
}

void NesCpu::reset() {
  this->register_a = 0;
  this->register_x = 0;
  this->status = cpuflags_from_bits(0b100100);
  this->program_counter = this->mem_read_u16(0xFFFC);
}

void NesCpu::set_carry_flag() { this->status = this->status | CpuFlags::CARRY; }

void NesCpu::clear_carry_flag() {
  this->status = this->status & ~CpuFlags::CARRY;
}

void NesCpu::add_to_register_a(uint8_t data) {
  uint16_t sum =
      this->register_a + data + (this->status & CpuFlags::CARRY ? 1 : 0);
  bool carry = sum > 0xFF;

  if (carry) {
    this->status |= CpuFlags::CARRY;
  } else {
    this->status &= ~CpuFlags::CARRY;
  }

  uint8_t result = static_cast<uint8_t>(sum & 0xFF);

  if (((data ^ result) & (result ^ this->register_a) & 0x80) != 0) {
    this->status |= CpuFlags::OVERFLOW;
  } else {
    this->status &= ~CpuFlags::OVERFLOW;
  }

  this->register_a = result;
}

void NesCpu::sbc(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t data = this->mem_read(addr);

  int8_t data_signed = static_cast<int8_t>(data);
  int8_t carry = this->status & CpuFlags::CARRY ? 1 : 0;
  int16_t result_signed = this->register_a - data_signed - carry;

  uint8_t result = static_cast<uint8_t>(result_signed);
  this->set_register_a(result);

  this->status &= ~CpuFlags::CARRY;

  if (result_signed >= 0 && result_signed < 128) {
    this->status |= CpuFlags::NEGATIV;
  } else {
    this->status &= ~CpuFlags::NEGATIV;
  }

  if (result == 0) {
    this->status |= CpuFlags::ZERO;
  } else {
    this->status &= ~CpuFlags::ZERO;
  }

  if ((this->register_a ^ result) & (this->register_a ^ data) & 0x80) {
    this->status |= CpuFlags::OVERFLOW;
  } else {
    this->status &= ~CpuFlags::OVERFLOW;
  }
}

void NesCpu::adc(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t value = this->mem_read(addr);
  this->add_to_register_a(value);
}

uint8_t NesCpu::stack_pop() {
  this->stack_pointer = uint8_t(stack_pointer + 1);
  return this->mem_read(static_cast<uint16_t>(STACK) +
                        static_cast<uint16_t>(this->stack_pointer));
}

void NesCpu::stack_push(uint8_t data) {
  this->mem_write(static_cast<uint16_t>(STACK) +
                      static_cast<uint16_t>(this->stack_pointer),
                  data);
  this->stack_pointer = uint8_t(stack_pointer - 1);
}

uint16_t NesCpu::stack_pop_u16() {
  uint16_t lo = static_cast<uint16_t>(this->stack_pop());
  uint16_t hi = static_cast<uint16_t>(this->stack_pop());

  return hi << 8 | lo;
}

void NesCpu::stack_push_u16(uint16_t data) {
  uint8_t hi = (data >> 8) & 0xFF;
  uint8_t lo = data & 0xFF;
  this->stack_push(hi);
  this->stack_push(lo);
}

void NesCpu::asl_accumulator() {
  uint8_t data = this->register_a;
  if ((data >> 7) == 1) {
    this->set_carry_flag();
  } else {
    this->clear_carry_flag();
  }

  data = data << 1;
  this->set_register_a(data);
}

uint8_t NesCpu::asl(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t data = this->mem_read(addr);
  if ((data >> 7) == 1) {
    this->set_carry_flag();
  } else {
    this->clear_carry_flag();
  }
  data = data << 1;
  this->mem_write(addr, data);
  this->update_zero_and_negative_flags(data);

  return data;
}

void NesCpu::lsr_accumulator() {
  uint8_t data = this->register_a;
  if ((data & 1) == 1) {
    this->set_carry_flag();
  } else {
    this->clear_carry_flag();
  }

  data = data >> 1;
  this->set_register_a(data);
}

uint8_t NesCpu::lsr(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t data = this->mem_read(addr);
  if ((data & 1) == 1) {
    this->set_carry_flag();
  } else {
    this->clear_carry_flag();
  }
  data = data >> 1;
  this->mem_write(addr, data);
  this->update_zero_and_negative_flags(data);

  return data;
}

void NesCpu::rol_accumulator() {
  uint8_t data = this->register_a;
  bool old_carry =
      static_cast<bool>(status & static_cast<uint8_t>(CpuFlags::CARRY));

  if ((data >> 7) == 1) {
    this->set_carry_flag();
  } else {
    this->clear_carry_flag();
  }

  data = data << 1;
  if (old_carry) {
    data = data | 1;
  }
  this->set_register_a(data);
}

uint8_t NesCpu::rol(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t data = this->mem_read(addr);
  bool old_carry =
      static_cast<bool>(status & static_cast<uint8_t>(CpuFlags::CARRY));

  if ((data >> 7) == 1) {
    this->set_carry_flag();
  } else {
    this->clear_carry_flag();
  }

  data = data << 1;
  if (old_carry) {
    data = data | 1;
  }
  this->mem_write(addr, data);
  this->update_negative_flags(data);

  return data;
}

void NesCpu::ror_accumulator() {
  uint8_t data = this->register_a;
  bool old_carry =
      static_cast<bool>(status & static_cast<uint8_t>(CpuFlags::CARRY));

  if ((data & 7) == 1) {
    this->set_carry_flag();
  } else {
    this->clear_carry_flag();
  }

  data = data >> 1;
  if (old_carry) {
    data = data | 0b10000000;
  }
  this->set_register_a(data);
}

uint8_t NesCpu::ror(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t data = this->mem_read(addr);
  bool old_carry =
      static_cast<bool>(status & static_cast<uint8_t>(CpuFlags::CARRY));

  if ((data & 7) == 1) {
    this->set_carry_flag();
  } else {
    this->clear_carry_flag();
  }

  data = data >> 1;
  if (old_carry) {
    data = data | 0b10000000;
  }
  this->mem_write(addr, data);
  this->update_negative_flags(data);

  return data;
}

uint8_t NesCpu::inc(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t data = this->mem_read(addr);

  data = uint8_t(data + 1);

  this->mem_write(addr, data);
  this->update_zero_and_negative_flags(data);

  return data;
}

void NesCpu::dey() {
  this->register_y = uint8_t(this->register_y - 1);
  this->update_zero_and_negative_flags(this->register_y);
}

void NesCpu::dex() {
  this->register_x = uint8_t(this->register_x + 1);
  this->update_zero_and_negative_flags(this->register_x);
}

uint8_t NesCpu::dec(AddressingMode mode) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t data = this->mem_read(addr);

  data = uint8_t(data - 1);

  this->mem_write(addr, data);
  this->update_zero_and_negative_flags(data);

  return data;
}

void NesCpu::pla() {
  uint8_t data = this->stack_pop();
  this->set_register_a(data);
}

void NesCpu::plp() {
  this->status = static_cast<CpuFlags>(this->stack_pop());
  this->status &= ~CpuFlags::BREAK;
  this->status |= CpuFlags::BREAK2;
}

void NesCpu::php() {
  CpuFlags flags = this->status;
  flags |= CpuFlags::BREAK;
  flags |= CpuFlags::BREAK2;
  this->stack_push(flags);
}

// Culpable de los errores
void NesCpu::bit(AddressingMode mode) {
  uint16_t addr = get_operand_address(mode);
  uint8_t data = mem_read(addr);
  uint8_t andd = register_a & data;

  if (andd == 0) {
    this->status |= CpuFlags::ZERO;
  } else {
    this->status &= ~CpuFlags::ZERO;
  }

  if ((data & 0b10000000) > 0) {
    this->status |= CpuFlags::NEGATIV;
  } else {
    this->status &= ~CpuFlags::NEGATIV;
  }

  if ((data & 0b01000000) > 0) {
    this->status |= CpuFlags::OVERFLOW;
  } else {
    this->status &= ~CpuFlags::OVERFLOW;
  }
}

void NesCpu::compare(AddressingMode mode, uint8_t compare_with) {
  uint16_t addr = this->get_operand_address(mode);
  uint8_t data = this->mem_read(addr);
  if (data <= compare_with) {
    this->status |= CpuFlags::CARRY;
  } else {
    this->status &= ~CpuFlags::CARRY;
  }
  this->update_zero_and_negative_flags(compare_with - data);
}

void NesCpu::branch(bool condition) {
  if (condition) {
    int8_t jump = static_cast<int8_t>(this->mem_read(this->program_counter));
    uint16_t jump_addr = uint16_t(this->program_counter + 1 + jump);
    this->program_counter = jump_addr;
  }
}

void NesCpu::run() {
  this->run_with_callback([](auto) {});
}

void NesCpu::load_and_run(std::vector<uint8_t> program) {
  this->load(program);
  /* this->reset(); */
  this->program_counter = this->mem_read_u16(0xFFFC);
  this->run();
}

uint16_t NesCpu::get_operand_address(AddressingMode mode) {
  switch (mode) {
  case AddressingMode::Immediate:
    return program_counter;

  case AddressingMode::ZeroPage: {
    uint8_t pos = mem_read(program_counter);
    return static_cast<uint16_t>(pos);
  }

  case AddressingMode::Absolute:
    return mem_read_u16(program_counter);

  case AddressingMode::ZeroPage_X: {
    uint8_t pos = mem_read(program_counter);
    uint16_t addr = static_cast<uint16_t>(pos) + register_x;
    return addr;
  }

  case AddressingMode::ZeroPage_Y: {
    uint8_t pos = mem_read(program_counter);
    uint16_t addr = static_cast<uint16_t>(pos) + register_y;
    return addr;
  }

  case AddressingMode::Absolute_X: {
    uint16_t base = mem_read_u16(program_counter);
    uint16_t addr = base + register_x;
    return addr;
  }

  case AddressingMode::Absolute_Y: {
    uint16_t base = mem_read_u16(program_counter);
    uint16_t addr = base + register_y;
    return addr;
  }

  case AddressingMode::Indirect_X: {
    uint8_t base = mem_read(program_counter);
    uint8_t ptr =
        static_cast<uint8_t>(static_cast<uint16_t>(base) + register_x);
    uint8_t lo = mem_read(ptr);
    uint8_t hi = mem_read(ptr + 1);
    uint16_t deref_base =
        (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
    return deref_base;
  }

  case AddressingMode::Indirect_Y: {
    uint8_t base = mem_read(program_counter);
    uint8_t lo = mem_read(static_cast<uint16_t>(base));
    uint8_t hi = mem_read(static_cast<uint16_t>(base) + 1);
    uint16_t deref_base =
        (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
    uint16_t deref = deref_base + register_y;
    return deref;
  }

  case AddressingMode::NoneAddressing:
    throw std::runtime_error("modo no soportado, dirección nula");

  default:
    throw std::runtime_error("direccion de memoria no encontrada");
  }
}
