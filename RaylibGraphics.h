#ifndef CHIP8_SUMMERATTEMPT_RAYLIBGRAPHICS_H
#define CHIP8_SUMMERATTEMPT_RAYLIBGRAPHICS_H
#include <cstdint>

class RaylibGraphics {
    public:
    RaylibGraphics();
    ~RaylibGraphics();
    void UpdateKeys(uint8_t* keypad);
    void UpdateDisplay(const uint8_t frameBuffer[64][32]);
};
#endif