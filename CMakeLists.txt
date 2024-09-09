cmake_minimum_required(VERSION 3.16)
project(cs_examples)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_executable(cs_examples main.c
        t1.c
        t1.h
        t2.c
        t2.h
        t3.c
        t3.h
)
