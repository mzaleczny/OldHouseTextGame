cmake_minimum_required(VERSION 3.15)

project(OldHouse VERSION 1.0)

add_library(OldHouse_compiler_flags INTERFACE)
target_compile_features(OldHouse_compiler_flags INTERFACE cxx_std_11)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

file(GLOB_RECURSE OldHouse_SOURCES "src/*.cpp")
file(GLOB_RECURSE OldHouse_HEADERS "src/*.h")

set (OldHouse_INCLUDE_DIRS "")
foreach (_headerFile ${OldHouse_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list(APPEND OldHouse_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES OldHouse_INCLUDE_DIRS)

add_executable (OldHouse ${OldHouse_SOURCES})

target_include_directories(OldHouse PUBLIC
                           ${OldHouse_INCLUDE_DIRS}
                           )
