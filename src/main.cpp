#include "chip8.h"
#include "Display.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

Display display;
Chip8 chip8;

int main(int argc, char* argv[]) {
    bool quit = false;
    char* romPath = argv[1];
    display.initialize();
    chip8.initialize();
    chip8.loadROM(romPath);
    const std::chrono::microseconds timerInterval = std::chrono::microseconds(16666);
    const std::chrono::microseconds cpuInterval = std::chrono::microseconds(1428);
    auto lastCPUTime = std::chrono::steady_clock::now();
    auto lastTimerTime = std::chrono::steady_clock::now();
    while (!quit) {
        auto currentTime = std::chrono::steady_clock::now();
        quit = display.processInput(chip8.key);
        while (currentTime - lastCPUTime >= cpuInterval) {
            chip8.cycle();
            lastCPUTime += cpuInterval;
        }
        while (currentTime - lastTimerTime >= timerInterval) {
            chip8.updateTimers();
            lastTimerTime += timerInterval;
        }
        if (chip8.drawFlag) {
            display.draw(chip8.gfx);
            chip8.drawFlag = false;
        }
    }
    return 0;
}  