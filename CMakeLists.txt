cmake_minimum_required(VERSION 3.16)
project(Memoire)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Active les outils automatiques de Qt (MOC, UIC, RCC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(${PROJECT_NAME}
        main.cpp
        model/Graph.hpp
        view/DrawableGraph.cpp
        model/Graph.cpp
        model/randomGenerator.hpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt6::Widgets
)