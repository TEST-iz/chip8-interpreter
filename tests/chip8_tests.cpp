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
    unsigned char oldSP = myChip8.SP;
    myChip8.decode(0x2123);
    REQUIRE(myChip8.SP - 1 == oldSP);
    REQUIRE(myChip8.S[myChip8.SP - 1] == 130);
    REQUIRE(myChip8.PC == 0x123);
    
    myChip8.decode(0x00EE);
    REQUIRE(myChip8.PC == 130);
    REQUIRE(myChip8.SP == oldSP);
}

TEST_CASE("Opcode DXYN: Drawing and Collision", "[graphics]") {
    Chip8 myChip8;
    myChip8.initialize();

    // 1. Setup a simple sprite in memory (a single horizontal line: 11110000)
    // We'll put it at address 0x300
    myChip8.memory[0x300] = 0xF0; 
    myChip8.I = 0x300;

    // 2. Setup registers for the draw command
    myChip8.V[0] = 5; // X-coordinate
    myChip8.V[1] = 5; // Y-coordinate

    // Execute: Draw 1 byte (0xF0) at (5, 5)
    // Opcode format: D (Draw) | X (Reg 0) | Y (Reg 1) | N (1 byte tall)
    myChip8.decode(0xD011);

    // Verify: The first 4 pixels starting at (5, 5) should be ON (1)
    // Index calculation: (y * 64) + x
    REQUIRE(myChip8.gfx[(5 * 64) + 5] == 1);
    REQUIRE(myChip8.gfx[(5 * 64) + 6] == 1);
    REQUIRE(myChip8.gfx[(5 * 64) + 7] == 1);
    REQUIRE(myChip8.gfx[(5 * 64) + 8] == 1);
    REQUIRE(myChip8.gfx[(5 * 64) + 9] == 0); // 5th pixel should be OFF

    // Verify: No collision occurred yet
    REQUIRE(myChip8.V[0xF] == 0);

    SECTION("XOR Collision") {
        // Draw the exact same sprite again at the same location
        myChip8.decode(0xD011);

        // Verify: The pixels should now be flipped back to OFF (0)
        REQUIRE(myChip8.gfx[(5 * 64) + 5] == 0);
        
        // Verify: Collision flag (VF) must be set to 1
        REQUIRE(myChip8.V[0xF] == 1);
    }
}

#endif