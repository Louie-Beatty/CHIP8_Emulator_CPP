#pragma once
#include <cstdint>
#include <vector>
class Chip8 {
public:
    //decremented at rate of 60Hz
    uint8_t m_timerReg = 0;
    uint8_t m_soundReg = 0;

    uint8_t m_frameBuffer[64][32]{}; //gui draws directly from 2d array
    uint8_t m_keypad[16] = {0};

    Chip8();
    void LoadFonts();
    void LoadFile();
    void FDELoop();


private:
    int_fast16_t m_PC = 0x200;
    int_fast8_t m_SP = 0;
    uint16_t m_I = 0; //store memory address
    uint8_t m_V[16] = {0}; //general purpose registers V0->VF
    uint8_t m_RAM[4096]{};
    std::vector<int_fast16_t> m_CHIP8STACK;

    [[nodiscard]] uint16_t Fetch() const;
    void DecodeExecute(uint16_t OPCODE);

    //ISA
    void ClearScreen();
    void JumpInstruction(uint16_t NNN);
    void SetRegister(uint8_t X, uint8_t NN);
    void AddValueToRegister(uint8_t X, uint8_t NN);
    void SetIndexRegister(uint16_t NNN);
    void SetRegisterToRegister(uint8_t X, uint8_t Y);
    void BitwiseOR(uint8_t X, uint8_t Y);
    void BitwiseAND(uint8_t X, uint8_t Y);
    void BitwiseXOR(uint8_t X, uint8_t Y);
    void BitwiseADD(uint8_t X, uint8_t Y);
    void BitwiseSubtractXY(uint8_t X, uint8_t Y);
    void BitwiseSubtractYX(uint8_t X, uint8_t Y);
    void BitwiseShiftOneToTheRight(uint8_t X, uint8_t Y);
    void BitwiseShiftOneToTheLeft(uint8_t X, uint8_t Y);
    void JumpWithOffset(uint16_t NNN);
    void RandomValueCHIP8(uint8_t X, uint8_t NN);
    void JumpAndPushToStack(uint16_t NNN);
    void PopFromStack();
    void SkipIfVXEqualToNN(uint8_t X, uint8_t NN);
    void SkipIfVXNotEqualToNN(uint8_t X, uint8_t NN);
    void SkipIfVXEqualToVY(uint8_t X, uint8_t Y);
    void SkipIfVXNotEqualToVY(uint8_t X, uint8_t Y);
    void SkipIfKeyIsPressed(uint8_t X);
    void SkipIfKeyIsNotPressed(uint8_t X);
    void SetVXToValueOfTimer(uint8_t X);
    void SetTimerToVXValue(uint8_t X);
    void SetVXToValueOfSound(uint8_t X);
    void AddToIndex(uint8_t X);
    void GetKey(uint8_t X);
    void IndexToFont(uint8_t X);
    void BinaryDecConversion(uint8_t X);
    void StoreInMemory(uint8_t X);
    void LoadMemory(uint8_t X);
    void DrawDisplay(uint8_t X, uint8_t Y, uint8_t N);
};