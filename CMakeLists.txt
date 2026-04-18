cmake_minimum_required(VERSION 3.16)
project(Memoire)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(${PROJECT_NAME}
        main.cpp
        view/DrawableGraph.cpp
        model/Graph.hpp
        model/Graph.cpp
        model/randomGenerator.hpp
        model/Algorithms.hpp
        model/Algorithms.cpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt6::Widgets
)