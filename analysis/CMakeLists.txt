cmake_minimum_required(VERSION 3.16)

file(GLOB_RECURSE ANALYSIS_SOURCES "src/*.cpp")
file(GLOB_RECURSE ANALYSIS_HEADERS "src/*.hpp")

add_executable(Analysis ${ANALYSIS_SOURCES} ${ANALYSIS_HEADERS})

#target_include_directories(analysis PRIVATE src)