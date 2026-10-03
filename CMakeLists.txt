cmake_minimum_required(VERSION 3.10)
project(celia CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenMP REQUIRED)

add_executable(celia
    src/main.cpp
    src/matrix.cpp
)

target_link_libraries(celia PRIVATE OpenMP::OpenMP_CXX)
