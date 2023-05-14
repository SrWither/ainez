#include "Core/NesCpu.hpp"

OpCode createOpCode(uint8_t code, const char *mnemonic, uint8_t len,
                    uint8_t cycles, AddressingMode mode) {
  return {code, mnemonic, len, cycles, mode};
}
