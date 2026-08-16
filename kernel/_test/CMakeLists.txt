cmake_minimum_required(VERSION 3.24)
project(TinyLoaderTest)

set(CMAKE_CXX_STANDARD 23)

include_directories(.)
include_directories(../uCodebase)
include_directories(../common)

add_executable(TinyLoaderTest main.cpp ../common/com_uart.c ../common/tickTimer.c ../common/protocol.c  ../common/device.c)

target_include_directories(TinyLoaderTest PRIVATE ..)