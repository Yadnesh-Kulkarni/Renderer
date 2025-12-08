
cmake_minimum_required(VERSION 3.20)
project(GameEngineFoundation VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

####################ENGINE##############################

# Collect sources
file(GLOB_RECURSE ENGINE_SOURCES
    "Engine/include/Math/*.h"
    "Engine/include/Renderer/*.h"
    "Engine/include/Platform/*.h"
    "Engine/include/Utils/*.h"
    "Engine/src/Math/*.cpp"
    "Engine/src/Renderer/*.cpp"
    "Engine/src/Platform/*.cpp"
    "Engine/src/Utils/*.cpp"
)

add_library(Engine ${ENGINE_SOURCES})

# Mirror folder structure in Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINE_SOURCES})

##########################APP############################

# Create executable target
add_executable(TestApp App/main.cpp)

# Link the Engine library
target_link_libraries(TestApp Engine)

target_include_directories(TestApp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Engine/include
)

# Make App the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TestApp)

add_subdirectory(TestSuite)
