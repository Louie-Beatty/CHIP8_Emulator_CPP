#include <iostream>
#include <cstdint>
#include "raylib.h"

int_fast16_t PC = 0x200;
uint8_t RAM[4096];
int_fast16_t STACK[16];

int_fast8_t timerReg; //decremented at rate of 60Hz
int_fast8_t soundReg;

uint8_t frameBuffer[64][32];

uint8_t V[16] = {0}; // V0->VF general purpose registers
uint16_t I = 0;

uint8_t FONT[80] = {
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
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

void loadFonts() {
    for (int i = 0; i < 80; ++i) {
        constexpr int FONT_START_ADDRESS = 0x050; //05-09F
        RAM[FONT_START_ADDRESS + i] = FONT[i];
    }
}

//ISA
void clearScreen() {
    for (auto & i : frameBuffer) {
        for (unsigned char & j : i) {
            j = 0;
        }
    }
    //IDE cleaned up code,just a nested for loop.
}

void jumpInstruction(const uint16_t instructionJump) {
    PC = instructionJump & 0x0FFF;
}

void setRegister(const uint16_t OPCODE) {
    const uint16_t regToSet = (OPCODE & 0x0F00)>>8;
    const uint16_t value = OPCODE & 0x00FF;
    V[regToSet] = value;
}

void addValueToRegister(const uint16_t OPCODE) {
    // 7XNN
    const uint16_t regToSet = (OPCODE & 0x0F00)>>8;
    const uint16_t valueToAdd = OPCODE & 0x00FF;
    V[regToSet] += valueToAdd;
}

void setIndexRegister(const uint16_t OPCODE) {
    I = OPCODE & 0x0FFF;
}


//GUI
void drawDisplay(const uint16_t OPCODE) {
    V[15] = 0;
    const uint16_t pixelHeight = (OPCODE & 0X000F);
    const uint16_t xCord =  V[(OPCODE & 0x0F00) >> 8] % 64;
    const uint16_t yCord =  V[(OPCODE & 0x00F0) >> 4] % 32;


    for (uint16_t row = 0; row < pixelHeight; ++row) {
        const uint16_t spriteByte = RAM[I + row];
        const uint16_t y = yCord + row;
        if (y >= 32) break;
        for (uint8_t col = 0; col < 8; ++col) {
            const uint8_t x = xCord + col;
            if (x >= 64) break;
            if (spriteByte & (0x80 >> col)) {
                if (frameBuffer[x][y]) {
                    V[15] = 1;
                }
                frameBuffer[x][y]^= 1;
            }
        }
    }
}


void updateDisplay() {
    BeginDrawing();
    ClearBackground(BLACK);
    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            if (frameBuffer[x][y]) {
                constexpr int scale = 10;
                DrawRectangle(x * scale, y * scale, scale, scale, BLUE);
            }
        }
    }
    EndDrawing();
}

//load rom into memory

void loadFile() {
    //later on will have preloaded cartridges memory carts also file handling lmao
    //add gui cool little cartridge screen

    //check if file exists
    //check have file perms
    //check if already open
    //check can be read from
    //close file
    constexpr int16_t ADDRESSABLEMEMORY = 3584; //4096-512

    FILE* file = fopen("/mnt/c/Users/Louie/CLionProjects/Chip8_SummerAttempt/ibm_logo.ch8", "rb"); //sucks
    if (file == nullptr) {
        printf("ERROR: Could not find ibm_logo.ch8! Check your folder.\n");
        return;
    }
    fread(&RAM[0x200], 1, ADDRESSABLEMEMORY, file);
    printf("\nloaded to memory");
    fclose(file);

}

uint16_t fetch() { //ai generated function credit where credits due.
    const uint16_t opcode = (RAM[PC] << 8) | RAM[PC + 1];
    return opcode;
}

void decodeExecute(const uint16_t OPCODE) {
    switch (OPCODE & 0xF000) {
        case 0x00E0:
            clearScreen();
            break;
        case 0x1000:
            jumpInstruction(OPCODE);
            break;
        case 0x6000:
            setRegister(OPCODE);
            break;
        case 0x7000:
            addValueToRegister(OPCODE);
            break;

        case 0xA000:
            setIndexRegister(OPCODE);
            break;
        case 0xD000:
            drawDisplay(OPCODE);
            break;
        default:
            std::cout << "Unknown Opcode:" << OPCODE << std::endl;
            break;
    }
}
void FDELoop() {
    const uint16_t OPCODE = fetch();
    PC += 2;
    decodeExecute(OPCODE);
}

void setup() {
    loadFonts();
    loadFile();
    InitWindow(640, 320, "CHIP8 - Louie Beatty");
    SetTargetFPS(60);
}

int main() {
    setup();
    //not a fan of this but works for now
    while (!WindowShouldClose()) {
        for (int i = 0; i < 10; ++i) {
            FDELoop();
        }
        updateDisplay();
    }
    CloseWindow();
    return 0;
}

//REFERENCES
//https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
