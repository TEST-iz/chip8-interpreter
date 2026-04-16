#ifdef USING_CATCH
#include <catch.hpp>
#include "../src/Chip8.h" // Adjust path to your header

TEST_CASE("Chip8 Initialization", "[init]") {
    Chip8 myChip8;
    myChip8.initialize();

    // Check that PC starts at the correct memory address
    REQUIRE(myChip8.PC == 0x200);
    
    // Check that registers are cleared
    for(int i = 0; i < 16; i++) {
        REQUIRE(myChip8.V[i] == 0);
    }
}

TEST_CASE("Opcode 0x8xy4: ADD with Overflow", "[opcodes]") {
    Chip8 myChip8;
    myChip8.initialize();

    // Setup: V0 = 0xFF, V1 = 0x01
    myChip8.V[0] = 0xFF;
    myChip8.V[1] = 0x01;

    // Manually trigger the ADD opcode (8014)
    // Assuming your function is something like emulateCycle() or execute(opcode)
    myChip8.decode(0x8014); 

    // Result should be 0x00 (0xFF + 0x01 = 0x100, which truncates to 8-bit 0x00)
    REQUIRE(myChip8.V[0] == 0x00);
    
    // Check the carry flag (Register V[0xF])
    // 0xFF + 0x01 > 255, so VF should be 1
    REQUIRE(myChip8.V[0xF] == 1);
}

TEST_CASE("Opcode Stack Flow", "[opcodes]") {
    Chip8 myChip8;
    myChip8.initialize();
    myChip8.PC = 130;
    oldSP = myChip8.SP;
    myChip8.decode(0x2123);
    REQUIRE(myChip8.SP - 1 == oldSP);
    REQUIRE(myChip8.S[myChip8.SP - 1] == 130);
    REQUIRE(myChip8.PC == 123);
    
    myChip8.decode(0x00EE);
    REQUIRE(myChip8.PC == 130);
    REQUIRE(myChip8.SP == oldSP);
}

#endif