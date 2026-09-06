cmake_minimum_required(VERSION 3.10)

project(FileRecovery)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(recovery
    test.cpp
    FAT32_BPB.cpp
    FAT32_FAT.cpp
    FAT32_Dir.cpp
    FAT32_Recovery.cpp
    FAT32_Nav.cpp
)