cmake_minimum_required(VERSION 3.0.2)

enable_testing()

project(job_scheduler)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(config)
include(catch)

add_subdirectory(scheduler_lib)
add_subdirectory(test)
add_subdirectory(scheduler)
