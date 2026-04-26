cmake_minimum_required(VERSION 3.16)
project(Memoire)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
add_link_options(-fsanitize=address,undefined)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/view/DrawableGraph.cpp
        src/model/Graph.hpp
        src/model/Graph.cpp
        src/model/randomGenerator.hpp
        src/model/Algorithms.hpp
        src/model/Algorithms.cpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt6::Widgets
)

find_package(nlohmann_json 3.2.0 QUIET)

if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found : use brew install nlohmann-json to download the library")
endif()
add_executable(my_app src/main.cpp)
target_link_libraries(my_app PRIVATE nlohmann_json::nlohmann_json)