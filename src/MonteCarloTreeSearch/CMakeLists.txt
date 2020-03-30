cmake_minimum_required(VERSION 3.8.0)
project(MancalaMcts)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O2 -Wall")
add_subdirectory(MancalaMctsLib)

add_executable(main proto.cpp)
target_link_libraries(main MancalaMctsLib)
target_include_directories(main PRIVATE MancalaMctsLib/include)