#include <iostream>
#include <fstream>
#include <system_error>

using namespace std;

class Chip8 {
    public:
        unsigned short opcode;
        unsigned char memory[4096];
        unsigned char V[16];
        unsigned short I;
        unsigned short PC;
        unsigned short S[16];
        unsigned char SP;
        unsigned char font[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };
        unsigned int gfx[64 * 32];
        unsigned char key[16];
        unsigned char delay_timer;
        unsigned char sound_timer;
        void initialize();
        void loadROM(char const* filename);
};

void Chip8::initialize() {
    for (int i = 0; i < 4096; i++) {
        if (i >=0x50 and i < 0xA0) {
            memory[i] = font[i - 0x50];
        }
        else {
            memory[i] = 0x00;
        }
    }
    for (int i = 0; i < 16; i++) {
        S[i] = 0x0000;
        V[i] = 0x00;
    }

    I = 0x0000;
    PC = 0x0200;
    delay_timer = 0;
    sound_timer = 0;
    SP = 0x00;

    for (int i = 0; i < 2048; i++) {
        gfx[i] = 0x00000000;
    }

    //TO-DO
    //initialize sound? and keybinds
}

void Chip8::loadROM(char const* filename) {
    ifstream file(filename, ios::binary | ios::ate);

    if (!file.is_open()) {
        throw runtime_error("File could not be opened");
    }

    else {
        streampos size = file.tellg();
        if (size > 3584) {
            file.close();
            throw runtime_error("File exceeds size limit");
        }

        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (int i = 0x200; i < 0x200 + size; i++) {
            memory[i] = buffer[i - 0x200];
        }

        delete[] buffer;
    }
}