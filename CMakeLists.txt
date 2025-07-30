cmake_minimum_required(VERSION 3.31)
project(decs)

set(CMAKE_CXX_STANDARD 17)

add_library(decs STATIC src/decs.cpp
                        src/ComponentArray.cpp)
target_include_directories(decs PUBLIC include)
