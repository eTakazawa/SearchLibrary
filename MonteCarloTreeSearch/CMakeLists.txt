cmake_minimum_required(VERSION 2.8)
project(mcts)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_FLAGS "-Wall -O3")

file(GLOB files src/*.h src/*.cc)
add_executable(mcts ${files})
