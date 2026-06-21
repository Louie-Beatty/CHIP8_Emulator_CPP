#include <iostream>
#include <cstdint>
#include "raylib.h"

int_fast16_t PC = 0x200;
uint8_t RAM[4096];
int_fast16_t STACK[16];
int_fast8_t SP = 0;


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

void LoadFonts() {
    for (int i = 0; i < 80; ++i) {
        constexpr int FONT_START_ADDRESS = 0x050; //05-09F
        RAM[FONT_START_ADDRESS + i] = FONT[i];
    }
}

//ISA
void ClearScreen() {
    for (auto & i : frameBuffer) {
        for (unsigned char & j : i) {
            j = 0;
        }
    }
    //IDE cleaned up code,just a nested for loop.
}

void JumpInstruction(const uint16_t address) {
    PC = address;
}

void SetRegister(const uint8_t X, const uint8_t NN) {
    V[X] = NN;
}

void AddValueToRegister(const uint8_t X, const uint8_t NN) {
    V[X] += NN;
}

void SetIndexRegister(const uint16_t NNN) {
    I = NNN;
}


//GUI
void DrawDisplay(const uint8_t X, const uint8_t Y, const uint8_t N) {
    V[15] = 0;
    const uint16_t xCord = V[X] % 64;
    const uint16_t yCord = V[Y] % 32;

    for (uint16_t row = 0; row < N; ++row) {
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
                frameBuffer[x][y] ^= 1;
            }
        }
    }
}



void UpdateDisplay() {
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

void LoadFile() {
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

uint16_t Fetch() { //ai generated function credit where credits due.
    const uint16_t opcode = (RAM[PC] << 8) | RAM[PC + 1];
    return opcode;
}

void DecodeExecute(const uint16_t OPCODE) {
    const uint8_t X   = (OPCODE & 0x0F00) >> 8;
    const uint8_t Y   = (OPCODE & 0x00F0) >> 4;
    const uint8_t N   = (OPCODE & 0x000F);
    const uint8_t NN  = (OPCODE & 0x00FF);
    const uint16_t NNN = (OPCODE & 0x0FFF);

    switch (OPCODE & 0xF000) {
        case 0x0000:
            ClearScreen();
            break;
        case 0x1000:
            JumpInstruction(NNN);
            break;
        case 0x6000:
            SetRegister(X,NN);
            break;
        case 0x7000:
            AddValueToRegister(X,NN);
            break;
        case 0xA000:
            SetIndexRegister(NNN);
            break;
        case 0xD000:
            DrawDisplay(X,Y,N);
            break;
        default:
            std::cout << "Unknown Opcode:" << OPCODE << std::endl;
            break;
    }
}
void FDELoop() {
    const uint16_t OPCODE = Fetch();
    PC += 2;
    DecodeExecute(OPCODE);
}

void Setup() {
    LoadFonts();
    LoadFile();
    InitWindow(640, 320, "CHIP8 - Louie Beatty");
    SetTargetFPS(60);
}

int main() {
    Setup();
    //not a fan of this but works for now
    while (!WindowShouldClose()) {
        for (int i = 0; i < 10; ++i) {
            FDELoop();
        }
        UpdateDisplay();
    }
    CloseWindow();
    return 0;
}

//REFERENCES
//https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
