cmake_minimum_required(VERSION 3.0.2)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

enable_testing()

project(scheduler_test CXX)

set(test_source_files  src/main.cpp src/test_graph.cpp)

add_executable(scheduler_test "${test_source_files}")

target_link_libraries(scheduler_test  scheduler_lib  Catch_lib)

set_property(TARGET scheduler_test PROPERTY CXX_STANDARD 17)
set_target_properties(scheduler_test PROPERTIES LINKER_LANGUAGE CXX)

add_test(NAME test_scheduler COMMAND scheduler_test WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
