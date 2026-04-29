cmake_minimum_required(VERSION 3.16)
project(Memoire)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
add_link_options(-fsanitize=address,undefined)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/model/Graph.hpp
        src/model/Graph.cpp
        src/model/randomGenerator.hpp
        src/model/Algorithms.hpp
        src/model/Algorithms.cpp
        src/view/widgets/DrawableGraph.hpp
        src/view/widgets/DrawableGraph.cpp
        src/view/graphs/ComparabilityBigraphWindow.hpp
        src/view/graphs/ComparabilityBigraphWindow.cpp
        src/view/MainWindow.hpp
        src/view/MainWindow.cpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui
)

find_package(nlohmann_json 3.2.0 QUIET)

if(nlohmann_json_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
else()
    message(STATUS "nlohmann_json not found : use 'brew install nlohmann-json' to download the library")
endif()