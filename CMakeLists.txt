cmake_minimum_required(VERSION 3.15)
project(subnet_calc C)

set(CMAKE_C_STANDARD 11)

add_executable(subnet_calc main.c subnet.h subnet.c)