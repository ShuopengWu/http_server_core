cmake_minimum_required(VERSION 3.10)

project(http_server_core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(http_server_core main.cpp)