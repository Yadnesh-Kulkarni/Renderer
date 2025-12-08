
cmake_minimum_required(VERSION 3.20)
project(GameEngineFoundation VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect sources
file(GLOB_RECURSE ENGINE_SOURCES
    "Engine/Math/Public/*.h"
    "Engine/Math/Private/*.cpp"
    "Engine/Renderer/Public/*.h"
    "Engine/Renderer/Private/*.cpp"
    "Engine/Platform/Public/*.h"
    "Engine/Platform/Private/*.cpp"
    "Engine/Utils/Public/*.h"
    "Engine/Utils/Private/*.cpp"
)

add_library(Engine ${ENGINE_SOURCES})

# Mirror folder structure in Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINE_SOURCES})
