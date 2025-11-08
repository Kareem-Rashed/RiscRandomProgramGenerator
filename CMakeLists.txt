cmake_minimum_required(VERSION 3.30)
project(RiscRandomProgramGenerator)

set(CMAKE_CXX_STANDARD 20)

add_executable(RiscRandomProgramGenerator main.cpp
        Generator.cpp
        Generator.h)
