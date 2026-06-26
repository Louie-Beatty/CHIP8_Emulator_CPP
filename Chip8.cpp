#include "Chip8.h"
#include <iostream>
#include <fstream>
#include <filesystem>

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

Chip8::Chip8() {
    m_CHIP8STACK.reserve(16); //never needs more than 16 elements
    LoadFonts();
    LoadFile();

}

void Chip8::LoadFile() {
    constexpr int16_t ADDRESSABLEMEMORY = 3584; // 4096 - 512
    std::string filename = "/mnt/c/Users/Louie/CLionProjects/Chip8_SummerAttempt/ibm_logo.ch8";
    if (!std::filesystem::exists(filename)) {std::cerr << "ERROR: Could not find " << filename << "! Check your folder.\n";return;}
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {std::cerr << "ERROR: File exists but cannot be opened (check permissions).\n";return;}
    const std::streamsize fileSize = file.tellg();
    if (fileSize > ADDRESSABLEMEMORY) {std::cerr << "ERROR: ROM file is too large (" << fileSize << " bytes).\n";return;}
    file.seekg(0, std::ios::beg);
    if (!file.read(reinterpret_cast<char*>(&m_RAM[0x200]), fileSize)) {std::cerr << "ERROR: Failed to read data from the file.\n";return; }
}
// https://www.raylib.com/examples/core/loader.html?name=core_drop_files next thing!



void Chip8::FDELoop(){
    const uint16_t OPCODE = Fetch();
    m_PC += 2;
    DecodeExecute(OPCODE);
}

void Chip8::LoadFonts() {
    for (int i = 0; i < 80; ++i) {
        constexpr int FONT_START_ADDRESS = 0x050; //05-09F
        m_RAM[FONT_START_ADDRESS + i] = FONT[i];
    }
}

void Chip8::Chip8::ClearScreen() {
    for (auto & i : m_frameBuffer) {
        for (unsigned char & j : i) {
            j = 0;
        }
    }
    //IDE cleaned up code,just a nested for loop.
}

//ISA
void Chip8::JumpInstruction(const uint16_t NNN) {
    m_PC = NNN;
}

void Chip8::SetRegister(const uint8_t X, const uint8_t NN) {
    m_V[X] = NN;
}

void Chip8::AddValueToRegister(const uint8_t X, const uint8_t NN) {
    m_V[X] += NN;
}

void Chip8::SetIndexRegister(const uint16_t NNN) {
    m_I = NNN;
}

void Chip8::SetRegisterToRegister(const uint8_t X, const uint8_t Y) {
    m_V[X] = m_V[Y];
}

void Chip8::BitwiseOR(const uint8_t X, const uint8_t Y) {
    m_V[X] |= m_V[Y];
}

void Chip8::BitwiseAND(const uint8_t X, const uint8_t Y) {
    m_V[X] &= m_V[Y];
}

void Chip8::BitwiseXOR(const uint8_t X, const uint8_t Y) {
    m_V[X] ^= m_V[Y];
}

void Chip8::BitwiseADD(const uint8_t X, const uint8_t Y) {
    const uint16_t sum  = m_V[X] += m_V[Y];
    m_V[X] = sum & 0xFF;
    if (sum>255) {m_V[15] =1;}else{m_V[15]=0;}; //V[15] is F the overflow register
}

void Chip8::BitwiseSubtractXY(const uint8_t X, const uint8_t Y) {
    const uint16_t sum  = m_V[X] - m_V[Y];
    m_V[X] = sum & 0xFF;
    if (sum < 256) {m_V[15] = 1;}else{m_V[15]=0;};
}

void Chip8::BitwiseSubtractYX(const uint8_t X, const uint8_t Y) {
    const uint16_t sum  = m_V[Y] - m_V[X];
    m_V[X] = sum & 0xFF;
    if (sum < 256) {m_V[15] = 1;}else{m_V[15]=0;};
}

//in bits
void Chip8::BitwiseShiftOneToTheRight(const uint8_t X, const uint8_t Y) {
    m_V[X] = m_V[Y] >> 1;
}
void Chip8::BitwiseShiftOneToTheLeft(const uint8_t X, const uint8_t Y) {
    m_V[X] = m_V[Y] << 1;
}

void Chip8::JumpWithOffset(const uint16_t NNN){
    m_PC = NNN + m_V[0];
}

void Chip8::RandomValueCHIP8(const uint8_t X,const uint8_t NN) {
    const uint8_t num = std::rand() % 255; //deprecated in cpp11 but not critical!
    m_V[X] = num&NN;
}

void Chip8::JumpAndPushToStack(const uint16_t NNN) {
    m_CHIP8STACK.push_back(m_PC);
    m_PC = NNN;
}

void Chip8::PopFromStack() {
    if (m_CHIP8STACK.empty()) {std::cerr << "Stack Underflow Error on 0x00EE!\n";}
    m_PC = m_CHIP8STACK.back();
    m_CHIP8STACK.pop_back();
}
void Chip8::SkipIfVXEqualToNN(const uint8_t X,const uint8_t NN) {
    if (m_V[X] == NN) {
        m_PC+=2;
    }
}
void Chip8::SkipIfVXNotEqualToNN(const uint8_t X,const uint8_t NN) {
    if (m_V[X] != NN) {
        m_PC+=2;
    }
}
void Chip8::SkipIfVXEqualToVY(const uint8_t X, const uint8_t Y) {
    if (m_V[X] == m_V[Y]) {
        m_PC+=2;
    }
}

void Chip8::SkipIfVXNotEqualToVY(const uint8_t X, const uint8_t Y) {
    if (m_V[X] != m_V[Y]) {
        m_PC+=2;
    }
}

void Chip8::SkipIfKeyIsPressed(const uint8_t X) {
    if (m_keypad[m_V[X]] != 0) { // Check the m_keypad array using V[X] as the index
        m_PC+=2;
    }
}
void Chip8::SkipIfKeyIsNotPressed(const uint8_t X) {
    if (m_keypad[m_V[X]] == 0) {
        m_PC+=2;
    }
}


void Chip8::SetVXToValueOfTimer(const uint8_t X) {
    m_V[X] = m_timerReg;
}

void Chip8::SetTimerToVXValue(const uint8_t X) {
    m_timerReg = m_V[X];
}

void Chip8::SetVXToValueOfSound(const uint8_t X) {
    m_soundReg = m_V[X];
}

void Chip8::AddToIndex(const uint8_t X) {
    m_I += m_V[X];
}

void Chip8::GetKey(const uint8_t X) {
    bool keyDetected = false;
    for (const unsigned char i : m_keypad) {
        if (i) {
            m_V[X] = i;
            keyDetected = true;
            break;
        }
    }
    if (!keyDetected) {
        m_PC -= 2;
    }
}

void Chip8::IndexToFont(const uint8_t X) {
    m_I = 0x050 + (m_V[X] * 5);
}

void Chip8::BinaryDecConversion(const uint8_t X) {
    const uint8_t value = m_V[X];
    m_RAM[m_I] = value / 100;
    m_RAM[m_I + 1] = (value / 10) % 10;
    m_RAM[m_I + 2] = value % 10;
    //tested in python
}

void Chip8::StoreInMemory(const uint8_t X) {
    for (uint8_t i = 0; i <= X; ++i) {
        m_RAM[m_I+i] = m_V[i];
    }
}

void Chip8::LoadMemory(const uint8_t X) {
    for (uint8_t i = 0; i <= X; ++i) {
        m_V[i] = m_RAM[m_I+i];
    }
}


//main switch statement

uint16_t Chip8::Fetch() const { //ai generated function credit where credits due.
    const uint16_t opcode = (m_RAM[m_PC] << 8) | m_RAM[m_PC + 1];
    return opcode;
}


void Chip8::DecodeExecute(const uint16_t OPCODE) {
    const uint8_t X   = (OPCODE & 0x0F00) >> 8;
    const uint8_t Y   = (OPCODE & 0x00F0) >> 4;
    const uint8_t N   = (OPCODE & 0x000F);
    const uint8_t NN  = (OPCODE & 0x00FF);
    const uint16_t NNN = (OPCODE & 0x0FFF);

    switch (OPCODE & 0xF000) {
        case 0x0000:
            switch (OPCODE) {
            case 0x00E0:
                    ClearScreen();
                    break;
            case 0x00EE:
                   PopFromStack();
                    break;
            default:
                    std::cout << "0x0000 opcode not recognised:  " << OPCODE << "\n";
                    break;
            }
            break;
        case 0x1000:
            JumpInstruction(NNN);
            break;
        case 0x2000:
            JumpAndPushToStack(NNN);
            break;
        case 0x3000:
            SkipIfVXEqualToNN(X,NN);
            break;
        case 0x4000:
            SkipIfVXNotEqualToNN(X,NN);
            break;
        case 0x5000:
            SkipIfVXEqualToVY(X,Y);
            break;
        case 0x6000:
            SetRegister(X,NN);
            break;
        case 0x7000:
            AddValueToRegister(X,NN);
            break;
        case 0x8000:
            switch (N) {
                case 0x0:
                        SetRegisterToRegister(X, Y);
                        break;
                case 0x1:
                        BitwiseOR(X, Y);
                        break;
                case 0x2:
                        BitwiseAND(X, Y);
                        break;
                case 0x3:
                        BitwiseXOR(X, Y);
                        break;
            case 0x4:
                    BitwiseADD(X,Y);
                    break;
            case 0x5:
                    BitwiseSubtractXY(X,Y);
                    break;
            case 0x6:
                    BitwiseShiftOneToTheRight(X,Y);
                    break;
            case 0xE:
                    BitwiseShiftOneToTheLeft(X,Y);
                    break;
            case 0x7:
                    BitwiseSubtractYX(X,Y);
                default:
                        std::cout << "Unknown 0x8000 series opcode: " << OPCODE << std::endl;
                        break;
                }
                break;
        case 0x9000:
            SkipIfVXNotEqualToVY(X,Y);
            break;
        case 0xA000:
            SetIndexRegister(NNN);
            break;
        case 0xB000:
            JumpWithOffset(NNN);
            break;
        case 0xC000:
            RandomValueCHIP8(X,NN);
            break;
        case 0xD000:
            DrawDisplay(X,Y,N);
            break;
        case 0xE000:
            switch (NN) {
                case 0x9E:
                    SkipIfKeyIsPressed(X);
                    break;
            case 0xA1:
                    SkipIfKeyIsNotPressed(X);
                    break;
            default:
                    break;
            }
            break;
        case 0xF000:
            switch (NN) {
            case 0x07:
                    SetVXToValueOfTimer(X);
                    break;
            case 0x0A:
                    GetKey(X);
                    break;
            case 0x15:
                    SetTimerToVXValue(X);
                    break;
            case 0x18:
                    SetVXToValueOfSound(X);
                    break;
            case 0x1E:
                    AddToIndex(X);
                    break;
            case 0x29:
                    IndexToFont(X);
                    break;
            case 0x33:
                    BinaryDecConversion(X);
                    break;
            case 0x55:
                    StoreInMemory(X);
                    break;
            case 0x65:
                    LoadMemory(X);
                    break;
            default:
                    break;
            }
            break;
        default:
            std::cout << "Unknown Opcode:" << OPCODE << std::endl;
            break;
    }
}

//GUI
void Chip8::DrawDisplay(const uint8_t X, const uint8_t Y, const uint8_t N) {
    m_V[15] = 0;
    const uint16_t xCord = m_V[X] % 64;
    const uint16_t yCord = m_V[Y] % 32;

    for (uint16_t row = 0; row < N; ++row) {
        const uint16_t spriteByte = m_RAM[m_I + row];
        const uint16_t y = yCord + row;
        if (y >= 32) break;

        for (uint8_t col = 0; col < 8; ++col) {
            const uint8_t x = xCord + col;
            if (x >= 64) break;

            if (spriteByte & (0x80 >> col)) {
                if (m_frameBuffer[x][y]) {
                    m_V[15] = 1;
                }
                m_frameBuffer[x][y] ^= 1;
            }
        }
    }
}




