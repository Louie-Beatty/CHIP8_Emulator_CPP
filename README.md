# Louie Beatty CHIP8  
Emulator written for the CHIP-8 virtual console
Prerequesites: git, cmake and a C++ 20 compiler (gcc, clang, msvc etc)

## ROMS
Grab some roms from here: 
https://johnearnest.github.io/chip8Archive/

## Controls

The original CHIP-8 used a 16-key hexadecimal keypad. This emulator maps those keys directly to the left side of your keyboard for easy access.
```text
 Modern Keyboard             Original CHIP-8
┌───┬───┬───┬───┐           ┌───┬───┬───┬───┐
│ 1 │ 2 │ 3 │ 4 │           │ 1 │ 2 │ 3 │ C │
├───┼───┼───┼───┤           ├───┼───┼───┼───┤
│ Q │ W │ E │ R │    =>     │ 4 │ 5 │ 6 │ D │
├───┼───┼───┼───┤           ├───┼───┼───┼───┤
│ A │ S │ D │ F │           │ 7 │ 8 │ 9 │ E │
├───┼───┼───┼───┤           ├───┼───┼───┼───┤
│ Z │ X │ C │ V │           │ A │ 0 │ B │ F │
└───┴───┴───┴───┘           └───┴───┴───┴───┘

## Linux/MacOS Build Process:
git clone https://github.com/Louie-Beatty/CHIP8_Emulator_CPP
<br>
cd CHIP8_Emulator_CPP
<br>
mkdir build
<br>
cd build
<br>
cmake ..
<br>
cmake --build .
<br>
./Chip8_Emulator
<br>
Or just double click in your file manager

## Windows Build Process:
git clone https://github.com/Louie-Beatty/CHIP8_Emulator_CPP
<br>
cd CHIP8_Emulator_CPP
<br>
mkdir build
<br>
cd build
<br>
cmake ..
<br>
cmake --build .
<br>
cd Debug
<br>
.\Chip8_Emulator.exe
<br>
Or just double-click the .exe in your File Explorer

## Credit:
https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
Excellent resource this project wouldn't be possible without it.

