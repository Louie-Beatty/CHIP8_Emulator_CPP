cmake_minimum_required(VERSION 3.28)
project(Chip8_Emulator)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
include(FetchContent)
FetchContent_Declare(
raylib
GIT_REPOSITORY https://github.com/raysan5/raylib.git
GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)
add_executable(Chip8_Emulator
main.cpp
Chip8.cpp
Chip8.h
RaylibGraphics.cpp
RaylibGraphics.h)
target_include_directories(Chip8_Emulator PRIVATE ${raylib_SOURCE_DIR}/src)
target_link_libraries(Chip8_Emulator PRIVATE raylib)
if (WIN32)
target_link_options(Chip8_Emulator PRIVATE -static -static-libgcc -static-libstdc++)
elseif (UNIX AND NOT APPLE)
target_link_libraries(Chip8_Emulator PRIVATE m pthread)
endif() 
