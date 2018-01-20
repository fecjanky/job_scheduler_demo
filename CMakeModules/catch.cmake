add_library(Catch_lib INTERFACE)

#file(GLOB_RECURSE catch_source_files  Catch2/single/ *.(h|hpp))

#target_sources(Catch_lib INTERFACE ${catch_source_files})

target_include_directories(Catch_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Catch2/single_include)
