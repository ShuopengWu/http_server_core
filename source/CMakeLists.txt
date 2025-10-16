cmake_minimum_required(VERSION 3.10)

project(http_server_core LANGUAGES CXX)

# Make relative paths based on the current CMakeLists.txt file
cmake_policy(SET CMP0076 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(http_server_core main.cpp)

add_subdirectory("src")