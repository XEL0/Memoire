cmake_minimum_required(VERSION 3.16)

#file(GLOB_RECURSE ANALYSIS_SOURCES "src/*.cpp")
#file(GLOB_RECURSE ANALYSIS_HEADERS "src/*.hpp")

#add_executable(Analysis ${ANALYSIS_SOURCES} ${ANALYSIS_HEADERS})

add_executable(Analysis
        src/algorithms.cpp
        src/algorithms.hpp
        src/Bigraph.hpp
        src/ComparabilityBigraph.hpp
        src/RandomGenerator.hpp
        src/experiment.cpp
)

#target_include_directories(analysis PRIVATE src)