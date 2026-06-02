cmake_minimum_required(VERSION 3.28)
project(Chip8_SummerAttempt)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)
add_executable(Chip8_SummerAttempt main.cpp)
target_link_libraries(Chip8_SummerAttempt PRIVATE raylib)