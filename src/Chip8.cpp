#include "Chip8.h"
#include <iostream>
#include <fstream>
#include <system_error>
#include <random>
#include <chrono>

using namespace std;

void Chip8::initialize() {
    for (int i = 0; i < 4096; i++) {
        if (i >=0x50 && i < 0xA0) {
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

    for (int i = 0; i < 16; i++) {
        key[i] = 0;
    }
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
                    PC = S[SP - 1];
                    S[SP] = 0x0000;
                    SP -= 1;
                    break;
            }
            break;

        case 0x1:
            PC = NNN;
            break;

        case 0x2:
            S[SP] = PC;
            SP += 1;
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
            }
            break;
        
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
        
        //come back to later
        case 0xC:
            {}; //code actual functionality later
            break;
        
        case 0xD: {
            unsigned int x_coord = V[X] & 63;
            unsigned int y_coord = V[Y] & 31;
            V[0xF] = 0;
            for (int height = 0; height < N; height++) {
                if (y_coord + height == 32) {
                        break;
                    }
                unsigned char pixel_row = memory[I + height];
                for (int pixel = 0; pixel < 8; pixel++) {
                    if (x_coord + pixel == 64) {
                        break;
                    }

                    if (gfx[x_coord + pixel + (y_coord + height) * 64] == 1 && (pixel_row >> (7 - pixel) & 1) == 1) {
                        V[0xF] = 1;
                        gfx[x_coord + pixel + (y_coord + height) * 64] = 0;
                    }
                    else if ((pixel_row >> (7 - pixel) & 1) == 1 && gfx[x_coord + pixel + (y_coord + height) * 64] == 0) {
                        gfx[x_coord + pixel + (y_coord + height) * 64] = 1;
                    }
                    
                }
            }
            drawFlag = 1;
            break;
        }

        case 0xE:
            switch (NN) {
                case 0x9E:
                    if (key[V[X]] == 1) {
                        PC += 2;
                    }
                    break;
                
                case 0xA1:
                    if (key[V[X]] == 0) {
                        PC += 2;
                    }
                    break;
            }
            break;
        
        case 0xF:
            switch (NN) {
                case 0x07:
                    V[X] = delay_timer;
                    break;
                
                case 0x15:
                    delay_timer = V[X];
                    break;
                
                case 0x18:
                    sound_timer = V[X];
                    break;
                
                case 0x1E:
                    if (I + V[X] > 0x0FFF) {
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    I += V[X];
                    break;
                
                case 0x0A:
                    if (key[0]) {
                        V[X] = 0;
                    }
                    else if (key[1]) {
                        V[X] = 1;
                    }
                    else if (key[2]) {
                        V[X] = 2;
                    }
                    else if (key[3]) {
                        V[X] = 3;
                    }
                    else if (key[4]) {
                        V[X] = 4;
                    }
                    else if (key[5]) {
                        V[X] = 5;
                    }
                    else if (key[6]) {
                        V[X] = 6;
                    }
                    else if (key[7]) {
                        V[X] = 7;
                    }
                    else if (key[8]) {
                        V[X] = 8;
                    }
                    else if (key[9]) {
                        V[X] = 9;
                    }
                    else if (key[10]) {
                        V[X] = 10;
                    }
                    else if (key[11]) {
                        V[X] = 11;
                    }
                    else if (key[12]) {
                        V[X] = 12;
                    }
                    else if (key[13]) {
                        V[X] = 13;
                    }
                    else if (key[14]) {
                        V[X] = 14;
                    }
                    else if (key[15]) {
                        V[X] = 15;
                    }
                    else {
                        PC -= 2;
                    }
                    break;

                case 0x29:
                    I = 0x50 + (5 * V[X]);
                    break;
                
                case 0x33:
                    memory[I] = V[X] / 100;
                    memory[I + 1] = (V[X]  / 10) % 10;
                    memory[I + 2] = V[X] % 10;
                    break;
                
                case 0x55:
                    for (int i = 0; i <= X; i++) {
                        memory[I + i] = V[i];
                    }
                    break;
                
                case 0x65:
                    for (int i = 0; i <= X; i++) {
                        V[i] = memory[I + i] ;
                    }
                    break;
            }
        break;
    }

}

void Chip8::updateTimers() {
    if (delay_timer > 0) {
        delay_timer -= 1;
    }

    if (sound_timer > 0) {
        delay_timer -= 1;
    }
}

void Chip8::cycle() {
    fetch();
    decode(opcode);

    if (delay_timer > 0) {
        delay_timer -= 1;
    }

    if (sound_timer > 0) {
        sound_timer -= 1;
    }
}
