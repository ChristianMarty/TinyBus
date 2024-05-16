cmake_minimum_required(VERSION 3.24)
project(TestTinyloader)

set(CMAKE_CXX_STANDARD 23)

include_directories(.)

add_executable(TestTinyloader main.cpp  ../common/tickTimer.c )

target_include_directories(TestTinyloader PRIVATE ..)