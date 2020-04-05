cmake_minimum_required(VERSION 3.8.0)
project(MancalaMcts)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O2 -Wall")
add_subdirectory(MancalaMctsLib)

add_executable(test proto.cpp)
target_link_libraries(test MancalaMctsLib)
target_include_directories(test PRIVATE MancalaMctsLib/include)