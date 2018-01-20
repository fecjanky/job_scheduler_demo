cmake_minimum_required(VERSION 3.0.2)

project(scheduler_lib CXX)

set(scheduler_lib_source_files src/lib.cpp include/job_graph.h )

add_library(scheduler_lib STATIC "${scheduler_lib_source_files}")

target_include_directories(scheduler_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(scheduler_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include_priv)

set_property(TARGET scheduler_lib PROPERTY CXX_STANDARD 17)
set_target_properties(scheduler_lib PROPERTIES LINKER_LANGUAGE CXX)