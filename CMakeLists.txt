cmake_minimum_required(VERSION 3.20)
project(GameEngineFoundation VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# ── GLFW options ──────────────────────────────────────────────────────────────
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC  OFF CACHE BOOL "" FORCE)

set(TF_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(TF_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_TESTS          OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES        OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL              OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS   OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)

# Only enable formats you actually need
set(ASSIMP_BUILD_OBJ_IMPORTER   ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER  ON CACHE BOOL "" FORCE)

# ── Read dependencies.json and declare all deps ───────────────────────────────
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/dependencies.json" JSON_DATA)
string(JSON ARRAY_LENGTH LENGTH "${JSON_DATA}")
math(EXPR LOOP_MAX "${ARRAY_LENGTH} - 1")

foreach(IDX RANGE ${LOOP_MAX})
    string(JSON DEP_NAME GET "${JSON_DATA}" ${IDX} "name")
    string(JSON GIT_URL  GET "${JSON_DATA}" ${IDX} "source" "url")
    string(JSON GIT_REV  GET "${JSON_DATA}" ${IDX} "source" "revision")
    message(STATUS "Declaring dependency: ${DEP_NAME} (${GIT_REV})")
    FetchContent_Declare(
        ${DEP_NAME}
        GIT_REPOSITORY ${GIT_URL}
        GIT_TAG        ${GIT_REV}
    )
endforeach()

set(ASSIMP_BUILD_3DS_EXPORTER   OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS          OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS   OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL              OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw taskflow glm assimp)

# ── Vulkan ────────────────────────────────────────────────────────────────────
find_package(Vulkan REQUIRED)

# Locate shaderc from the Vulkan SDK (not exposed as a CMake target by find_package)
if(DEFINED ENV{VULKAN_SDK})
    set(VULKAN_SDK_PATH "$ENV{VULKAN_SDK}")
else()
    get_filename_component(VULKAN_SDK_PATH "${Vulkan_INCLUDE_DIRS}/.." ABSOLUTE)
endif()

add_library(shaderc_combined STATIC IMPORTED)
set_target_properties(shaderc_combined PROPERTIES
    IMPORTED_LOCATION         "${VULKAN_SDK_PATH}/Lib/shaderc_combined.lib"
    IMPORTED_LOCATION_DEBUG   "${VULKAN_SDK_PATH}/Lib/shaderc_combinedd.lib"
    IMPORTED_LOCATION_RELEASE "${VULKAN_SDK_PATH}/Lib/shaderc_combined.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${VULKAN_SDK_PATH}/Include"
)

option(BUILD_TESTS "Build the test suite" OFF)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)

# ── Engine ────────────────────────────────────────────────────────────────────
file(GLOB_RECURSE ENGINE_SOURCES
    "Engine/include/math/Dev/*.h"
    "Engine/include/renderer/*.h"
    "Engine/include/renderer/Vulkan/core/*.h"
    "Engine/include/renderer/Vulkan/Render/*.h"
    "Engine/include/renderer/Vulkan/Surface/*.h"
    "Engine/include/renderer/Vulkan/Resources/*.h"
    "Engine/include/renderer/Vulkan/common/*.h"
    "Engine/include/renderer/OpenGL/core/*.h"
    "Engine/include/platform/*.h"
    "Engine/include/utils/*.h"
    "Engine/include/core/*.h"
    "Engine/include/common/*.h"
    "Engine/src/math/*.cpp"
    "Engine/src/renderer/Vulkan/*.cpp"
    "Engine/src/renderer/Vulkan/core/*.cpp"
    "Engine/src/renderer/Vulkan/Resource/*.cpp"
    "Engine/src/renderer/Vulkan/Surface/*.cpp"
    "Engine/src/renderer/Vulkan/Render/*.cpp"
    "Engine/include/renderer/OpenGL/core/*.cpp"
    "Engine/src/renderer/*.cpp"
    "Engine/src/platform/*.cpp"
    "Engine/src/utils/*.cpp"
    "Engine/src/core/*.cpp"
)

add_library(Engine ${ENGINE_SOURCES})

target_include_directories(Engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Engine/include
    PRIVATE
        ${taskflow_SOURCE_DIR}
        ${glm_SOURCE_DIR}
)

target_link_libraries(Engine
    PUBLIC
        Vulkan::Vulkan
        shaderc_combined
        glfw
        assimp
)

target_compile_definitions(Engine
    PUBLIC
        ENGINE_SOURCE_DIR="${CMAKE_SOURCE_DIR}/"
    PRIVATE
        $<$<CONFIG:Debug>:ENGINE_DEBUG>
)

if(MSVC)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINE_SOURCES})
endif()

# ── App ───────────────────────────────────────────────────────────────────────
add_executable(TestApp App/main.cpp)

target_link_libraries(TestApp
    PRIVATE
        Engine
)

target_include_directories(TestApp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Engine/include
)

target_link_options(TestApp PRIVATE "/NODEFAULTLIB:MSVCRT")

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TestApp)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(TestSuite)
endif()