#ifdef USING_CATCH
#include <catch.hpp>
#include "../src/Chip8.h"

TEST_CASE("Chip8 Initialization", "[init]") {
    Chip8 myChip8;
    myChip8.initialize();

    REQUIRE(myChip8.PC == 0x200);
    
    for(int i = 0; i < 16; i++) {
        REQUIRE(myChip8.V[i] == 0);
    }
}

TEST_CASE("Opcode 0x8xy4: ADD with Overflow", "[opcodes]") {
    Chip8 myChip8;
    myChip8.initialize();

    myChip8.V[0] = 0xFF;
    myChip8.V[1] = 0x01;

    myChip8.decode(0x8014); 

    REQUIRE(myChip8.V[0] == 0x00);
    REQUIRE(myChip8.V[0xF] == 1);
}

TEST_CASE("Opcode Stack Flow", "[opcodes]") {
    Chip8 myChip8;
    myChip8.initialize();

    myChip8.PC = 130;
    unsigned char oldSP = myChip8.SP;

    myChip8.PC += 2; 
    myChip8.decode(0x2123);

    REQUIRE(myChip8.SP == oldSP + 1);

    REQUIRE(myChip8.S[myChip8.SP - 1] == 132);

    REQUIRE(myChip8.PC == 0x123);

    myChip8.decode(0x00EE);
    REQUIRE(myChip8.PC == 132);
    REQUIRE(myChip8.SP == oldSP);
}

TEST_CASE("Opcode DXYN: Drawing and Collision", "[graphics]") {
    Chip8 myChip8;
    myChip8.initialize();

    myChip8.memory[0x300] = 0xF0; 
    myChip8.I = 0x300;

    myChip8.V[0] = 5;
    myChip8.V[1] = 5;

    myChip8.decode(0xD011);

    REQUIRE(myChip8.gfx[(5 * 64) + 5] == 1);
    REQUIRE(myChip8.gfx[(5 * 64) + 6] == 1);
    REQUIRE(myChip8.gfx[(5 * 64) + 7] == 1);
    REQUIRE(myChip8.gfx[(5 * 64) + 8] == 1);
    REQUIRE(myChip8.gfx[(5 * 64) + 9] == 0);

    REQUIRE(myChip8.V[0xF] == 0);

    SECTION("XOR Collision") {
        myChip8.decode(0xD011);

        REQUIRE(myChip8.gfx[(5 * 64) + 5] == 0);
        
        REQUIRE(myChip8.V[0xF] == 1);
    }

    SECTION("Screen Edge Clipping") {
        myChip8.initialize();

        myChip8.memory[0x300] = 0xFF; 
        myChip8.I = 0x300;

        myChip8.V[0] = 60; 
        myChip8.V[1] = 10;

        myChip8.decode(0xD011);

        REQUIRE(myChip8.gfx[(10 * 64) + 60] == 1);
        REQUIRE(myChip8.gfx[(10 * 64) + 61] == 1);
        REQUIRE(myChip8.gfx[(10 * 64) + 62] == 1);
        REQUIRE(myChip8.gfx[(10 * 64) + 63] == 1);

        REQUIRE(myChip8.gfx[(10 * 64) + 0] == 0);
        REQUIRE(myChip8.gfx[(10 * 64) + 1] == 0);
    }
}

TEST_CASE("IBM Opcode Testing", "[opcodes]") {
    Chip8 myChip8;
    myChip8.initialize();

    SECTION("Opcode 1NNN") {
        myChip8.decode(0x1420);
        REQUIRE(myChip8.PC == 0x420);
    }
    SECTION("Opcode 6XNN") {
        myChip8.decode(0x6367);
        REQUIRE(myChip8.V[0x3] == 0x67);
    }

    SECTION("Opcode 7XNN") {
        myChip8.decode(0x7420);
        REQUIRE(myChip8.V[0x4] == 0x20);
    }

    SECTION("Opcode ANNN") {
        myChip8.decode(0xA420);
        REQUIRE(myChip8.I == 0x420);
    }

    SECTION("Opcode 00E0") {
        myChip8.memory[0x300] = 0xF0; 
        myChip8.I = 0x300;

        myChip8.V[0] = 5;
        myChip8.V[1] = 5;

        myChip8.decode(0xD011);
        myChip8.decode(00E0);
        unsigned int running = 0;
        for (int i = 0; i < 2048; i++) {
            running = running | gfx[i];
        }
        REQUIRE(running == 0);
    }
}

#endif