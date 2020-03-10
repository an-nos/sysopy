cmake_minimum_required(VERSION 3.15)
project(sysopy C)

set(CMAKE_C_STANDARD 11)

add_executable(sysopy main.c zestaw1/zad3b/main.c zestaw1/zad3b/main_dyn.c)