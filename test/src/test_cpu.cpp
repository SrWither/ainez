#include "Core/NesCpu.hpp"
#include <gtest/gtest.h>

class CPUTest : public ::testing::Test {
protected:
    void SetUp() override {
        cpu = NesCpu();
    }

    void TearDown() override {
        // Clean up after test execution
    }

    NesCpu cpu;
};

TEST_F(CPUTest, test_lda_immediate_load_data) {
    cpu.load_and_run({0xa9, 0x05, 0x00});
    EXPECT_EQ(cpu.register_a, 5);
    EXPECT_FALSE(cpu.status == CpuFlags::ZERO);
    EXPECT_FALSE(cpu.status == CpuFlags::NEGATIV);
}

TEST_F(CPUTest, test_tax_move_a_to_x) {
    cpu.register_a = 10;
    cpu.load_and_run({0xaa, 0x00});
    EXPECT_EQ(cpu.register_x, 10);
}

TEST_F(CPUTest, test_5_ops_working_together) {
    cpu.load_and_run({0xa9, 0xc0, 0xaa, 0xe8, 0x00});
    EXPECT_EQ(cpu.register_x, 0xc1);
}

TEST_F(CPUTest, test_inx_overflow) {
    cpu.register_x = 0xff;
    cpu.load_and_run({0xe8, 0xe8, 0x00});
    EXPECT_EQ(cpu.register_x, 1);
}

TEST_F(CPUTest, test_lda_from_memory) {
    cpu.mem_write(0x10, 0x55);
    cpu.load_and_run({0xa5, 0x10, 0x00});
    EXPECT_EQ(cpu.register_a, 0x55);
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
