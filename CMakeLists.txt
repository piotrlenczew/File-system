cmake_minimum_required(VERSION 3.16)
project(soi_6_system_plikow)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(soi_6_system_plikow
        FileSystem.cpp
        FileSystem.h
        main.cpp)
