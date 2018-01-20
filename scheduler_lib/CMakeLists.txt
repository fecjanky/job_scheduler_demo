cmake_minimum_required(VERSION 3.0.2)

project(scheduler_lib CXX)

set(scheduler_lib_source_files src/lib.cpp)

add_library(scheduler_lib STATIC ${scheduler_lib_source_files})

target_include_directories(scheduler_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(scheduler_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include_priv)
