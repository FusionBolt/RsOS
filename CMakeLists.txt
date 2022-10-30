cmake_minimum_required(VERSION 3.22)
project(RsOS)

set(CMAKE_CXX_STANDARD 20)

add_executable(RsOS main.cpp kernel/os.cpp)
