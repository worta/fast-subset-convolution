cmake_minimum_required(VERSION 3.14)
project(fast_subset_convolution)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

find_package(Boost)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()

add_executable(fast_subset_convolution
        functions.cpp
        functions.h
        main.cpp
        main.h
        mobius.cpp
        mobius.h
        steiner.cpp
        steiner.h
        utility.cpp
        utility.h common.h)
