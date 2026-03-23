#include <iostream>
#include <fstream>
#include <system_error>
#include <random>
#include <chrono>

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
        void fetch();
        void decode(unsigned short opcode);

        // Chip8() {
        //     unsigned int seed = (std::chrono::system_clock::now().time_since_epoch().count());
        //     std::mt19937 rng(seed);
        //     std::uniform_int_distribution<int> gen(0, 255);
        // }
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

void Chip8::fetch() {
    unsigned short byteOne = memory[PC];
    unsigned short byteTwo = memory[PC+1];
    PC += 2;
    opcode = byteOne << 8 | byteTwo;
}

void Chip8::decode(unsigned short opcode) {
    unsigned char firstDigit = opcode >> 12;
    unsigned char X = (opcode & 0x0F00) >> 8;
    unsigned char Y = (opcode & 0x00F0) >> 4;
    unsigned char N = (opcode & 0x000F);
    unsigned char NN = (opcode & 0x00FF);
    unsigned short NNN = (opcode & 0x0FFF);

    switch (firstDigit) {
        case 0x0:
            switch (NN) {
                case 0xE0:
                    for (int i = 0; i < 2048; i++) {
                        gfx[i] = 0x00000000;
                    }
                    break;
                
                case 0xEE:
                    PC = S[SP];
                    S[SP] = 0x0000;
                    SP -= 1;
                    break;
            }
            break;

        case 0x1:
            PC = NNN;
            break;

        case 0x2:
            SP += 1;
            S[SP] = PC;
            PC = NNN;
            break;

        case 0x3:
            if (V[X] == NN) {
                PC += 2;
            }
            break;
        
        case 0x4:
            if (V[X] != NN) {
                PC += 2;
            }
            break;
        
        case 0x5:
            if (V[X] == V[Y]) {
                PC +=2;
            }
            break;

        case 0x6:
            V[X]= NN;
            break;
        
        case 0x7:
            V[X] += NN;
            break;
        
        case 0x8:
            switch (N) {
                case 0x0:
                    V[X] = V[Y];
                    break;
                
                case 0x1:
                    V[X] = V[X] | V[Y];
                    break;
                
                case 0x2:
                    V[X] = V[X] & V[Y];
                    break;

                case 0x3:
                    V[X] = V[X] ^ V[Y];
                    break;

                case 0x4:
                    if (V[Y] > 255 - V[X]) {
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    V[X] = V[X] + V[Y];
                    break;

                case 0x5:
                    if (V[X] >= V[Y]) {
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    V[X] = V[X] - V[Y];
                    break;
                
                //depends on what model we are using COSMAC VIP or CHIP48 and Super CHIP, allow user to choose
                //currently using Chip48
                case 0x6:
                    V[0xF] = ((V[X] << 7) >> 7);
                    V[X] = V[X] >> 1;
                    break;
                
                case 0x7:
                    if (V[Y] >= V[X]) {
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    V[X] = V[Y] - V[X];
                    break;
                //depends on what model we are using COSMAC VIP or CHIP48 and Super CHIP, allow user to choose
                //currently using Chip48
                case 0xE:
                    V[0xF] = (V[X] >> 7);
                    V[X] = V[X] << 1;
                    break;
            
            break;
            }
        
        case 0x9:
            if (V[X] != V[Y]) {
                PC += 2;
            }
            break;
        
        case 0xA:
            I = NNN;
            break;
        
        case 0xB:
            PC = NNN + V[0x0];
            break;
        
        case 0xC:
            ;
            
    }

}