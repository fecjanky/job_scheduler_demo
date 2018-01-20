cmake_minimum_required(VERSION 3.0.2)


project(scheduler CXX)

set(scheduler_source_files src/main.cpp)

add_executable(scheduler ${scheduler_source_files})

target_include_directories(scheduler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(scheduler PUBLIC scheduler_lib)

set_property(TARGET scheduler PROPERTY CXX_STANDARD 17)