#include "Chip8.h"
#include "RaylibGraphics.h"

int main() {
    RaylibGraphics graphics;
    Chip8 Chip8;
    while (!WindowShouldClose()) {
        graphics.UpdateKeys(Chip8.m_keypad);
        for (int i = 0; i < 10; ++i) {
            Chip8.FDELoop();
        }
        if (Chip8.m_timerReg > 0) {
            --Chip8.m_timerReg;
        }
        if (Chip8.m_soundReg > 0) {
            --Chip8.m_soundReg;
        }
        graphics.UpdateDisplay(Chip8.m_frameBuffer);
    }
    return 0;
}

//REFERENCES
//https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
