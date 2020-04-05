cmake_minimum_required(VERSION 3.8.0)
project(MancalaMctsLib CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O2 -Wall")

file(GLOB SRC "src/*")
add_library(MancalaMctsLib SHARED ${SRC})

target_include_directories(MancalaMctsLib PUBLIC ./include)