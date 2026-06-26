#include "RaylibGraphics.h"
#include "raylib.h"

RaylibGraphics::RaylibGraphics() {
    InitWindow(640, 320, "CHIP8 - Louie Beatty");
    SetTargetFPS(60);
}
RaylibGraphics::~RaylibGraphics() {
    CloseWindow();
}

void RaylibGraphics::UpdateDisplay(const uint8_t frameBuffer[64][32]) {
    BeginDrawing();
    ClearBackground(BLACK);
    for (uint8_t y = 0; y < 32; ++y) {
        for (uint8_t x = 0; x < 64; ++x) {
            if (frameBuffer[x][y]) {
                constexpr int scale = 10;
                DrawRectangle(x * scale, y * scale, scale, scale, BLUE);
            }
        }
    }
    EndDrawing();
}

void RaylibGraphics::UpdateKeys(uint8_t* keypad) { //same mapping as old
    keypad[0x1] = IsKeyDown(KEY_ONE);
    keypad[0x2] = IsKeyDown(KEY_TWO);
    keypad[0x3] = IsKeyDown(KEY_THREE);
    keypad[0xC] = IsKeyDown(KEY_FOUR);
    keypad[0x4] = IsKeyDown(KEY_Q);
    keypad[0x5] = IsKeyDown(KEY_W);
    keypad[0x6] = IsKeyDown(KEY_E);
    keypad[0xD] = IsKeyDown(KEY_R);
    keypad[0x7] = IsKeyDown(KEY_A);
    keypad[0x8] = IsKeyDown(KEY_S);
    keypad[0x9] = IsKeyDown(KEY_D);
    keypad[0xE] = IsKeyDown(KEY_F);
    keypad[0xA] = IsKeyDown(KEY_Z);
    keypad[0x0] = IsKeyDown(KEY_X);
    keypad[0xB] = IsKeyDown(KEY_C);
    keypad[0xF] = IsKeyDown(KEY_V);
}
