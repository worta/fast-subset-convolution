cmake_minimum_required(VERSION 3.14)
project(fast_subset_convolution)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

find_package(Boost)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()


#if(NOT CMAKE_BUILD_TYPE)
#    set(CMAKE_BUILD_TYPE Release)
#endif()
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")


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
        utility.h common.h tests.cpp tests.h MinSumRingEmbedd.cpp MinSumRingEmbedd.h stp_parser.cpp stp_parser.h GraphGenerator.cpp GraphGenerator.h benchmark_steiner.cpp benchmark_steiner.h benchmark_writer.cpp benchmark_writer.h benchmark_fsconv.cpp benchmark_fsconv.h)
