cmake_minimum_required(VERSION 3.0.2)

enable_testing()

project(scheduler CXX)

set(scheduler_source_files  src/main.cpp)

add_executable(scheduler "${scheduler_source_files}")

target_include_directories(scheduler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(scheduler PUBLIC scheduler_lib)

set_property(TARGET scheduler PROPERTY CXX_STANDARD 17)
set_target_properties(scheduler PROPERTIES LINKER_LANGUAGE CXX)

add_test(NAME test_scheduler_sanity COMMAND scheduler ../test/resources/test_ref_graph.txt WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")