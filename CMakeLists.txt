
cmake_minimum_required(VERSION 3.20)
project(GameEngineFoundation VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(Vulkan REQUIRED)

# Option to build tests (disabled by default)
option(BUILD_TESTS "Build the test suite" OFF)

# Add libs directory as a library search path
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)

####################ENGINE##############################

# Collect sources
file(GLOB_RECURSE ENGINE_SOURCES
    "Engine/include/math/Dev/*.h"
    "Engine/include/renderer/*.h"
    "Engine/include/platform/*.h"
    "Engine/include/utils/*.h"
    "Engine/src/math/*.cpp"
    "Engine/src/renderer/*.cpp"
    "Engine/src/platform/*.cpp"
    "Engine/src/utils/*.cpp"
)

add_library(Engine ${ENGINE_SOURCES})

target_include_directories(Engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Engine/include
)

# Mirror folder structure in Visual Studio (harmless on other platforms)
if(MSVC)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINE_SOURCES})
endif()

##########################APP############################

# Create executable target
add_executable(TestApp App/main.cpp)

# Link the Engine library
target_link_libraries(TestApp 
    PRIVATE
        Engine
)

target_include_directories(TestApp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Engine/include
)


include_directories(${Vulkan_INCLUDE_DIRS})
target_link_libraries(TestApp PRIVATE Vulkan::Vulkan)
target_link_options(TestApp PRIVATE "/NODEFAULTLIB:MSVCRT")

# Make App the startup project in Visual Studio (harmless on other platforms)
if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TestApp)
endif()

# Build tests only if BUILD_TESTS option is enabled
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(TestSuite)
endif()
