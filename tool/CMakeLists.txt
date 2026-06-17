cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
#add_link_options(-fsanitize=address,undefined)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

add_executable(Tool
        src/main.cpp
        src/model/Graph.hpp
        src/model/Graph.cpp
        src/model/RandomGenerator.hpp
        src/model/Algorithms.hpp
        src/model/Algorithms.cpp
        src/view/graphicsItems/DrawableGraph.hpp
        src/view/graphicsItems/DrawableGraph.cpp
        src/view/graphicsItems/DrawableGraphNode.hpp
        src/view/graphicsItems/DrawableGraphNode.cpp
        src/view/graphicsItems/Arrow.hpp
        src/view/graphicsItems/Arrow.cpp
        src/view/graphicsItems/TreeLayout.hpp
        src/view/windows/MainWindow.hpp
        src/view/windows/MainWindow.cpp
        src/view/windows/GraphWindow.hpp
        src/view/windows/GraphWindow.cpp
        src/view/windows/StepsWindow.hpp
        src/view/windows/StepsWindow.cpp
        src/model/Bigraph.hpp
)

include(FetchContent)
find_package(nlohmann_json 3.11.3 QUIET)

if(NOT nlohmann_json_FOUND)
    FetchContent_Declare(
            json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.11.3
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(json)

endif()

add_custom_command(TARGET Tool POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:Tool>/assets
        COMMENT "Copying assets to build directory"
)

target_link_libraries(Tool PRIVATE
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        nlohmann_json::nlohmann_json
)