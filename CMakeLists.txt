cmake_minimum_required(VERSION 3.31)

project(SourceLocation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(SL_INCLUDE_TESTS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if(SL_INCLUDE_TESTS)
    add_subdirectory(tests)
endif()
