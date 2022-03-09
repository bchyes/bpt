cmake_minimum_required(VERSION 3.16)
project(bpt)

set(CMAKE_CXX_STANDARD 14)

add_executable(code main_morekey.cpp exceptions.hpp utility.hpp string.hpp)
