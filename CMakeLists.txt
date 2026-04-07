# University of La Laguna
# Higher School of Engineering and Technology
# Project: Memory Consistency Examples
# @file CMakeLists.txt
# @author Domenico Goya (wh0crypt@computer.org)
# @brief Directives to compile the examples.
# @version 0.1
# @date 2026-04-07
# @copyright Copyright (c) 2026 Domenico Goya (wh0crypt)

cmake_minimum_required(VERSION 3.16)

project(MemoryConsistencyExamples 
        VERSION 1.0 
        DESCRIPTION "Examples of Memory Fences and Peterson's Algorithm"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(COMMON_FLAGS /W4 /permissive- /O2)
else()
    set(COMMON_FLAGS 
        -O2 -Wall -Wextra -Wpedantic -Wshadow 
        -Wconversion -Wsign-conversion -Wnull-dereference
        -fsanitize=address,undefined)
endif()

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

foreach(SOURCE_FILE ${SOURCE_FILES})
    get_filename_component(EXE_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${EXE_NAME} ${SOURCE_FILE})
    target_compile_options(${EXE_NAME} PRIVATE ${COMMON_FLAGS})
    
    if(NOT MSVC)
        target_link_options(${EXE_NAME} PRIVATE -fsanitize=address,undefined)
    endif()
    
    message(STATUS "Created executable: ${EXE_NAME}")
endforeach()
