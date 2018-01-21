cmake_minimum_required(VERSION 3.0.2)

project(scheduler_lib CXX)

add_library(scheduler_lib INTERFACE)

target_include_directories(scheduler_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
