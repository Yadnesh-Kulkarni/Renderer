
cmake_minimum_required(VERSION 3.20)
project(GameEngineFoundation VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MATH_PRIVATE
    src/math/private/Vec2.cpp
    src/math/private/Vec3.cpp
    src/math/private/Vec4.cpp
    src/math/private/Mat3.cpp
    src/math/private/Mat4.cpp
    src/math/private/Quaternion.cpp
    src/math/private/Geometry.cpp
)

set(RENDERER_PRIVATE
    src/renderer/private/Renderer.cpp
    src/renderer/private/Shader.cpp
    src/renderer/private/Material.cpp
    src/renderer/private/Light.cpp
    src/renderer/private/Camera.cpp
    src/renderer/private/Scene.cpp
)

set(PLATFORM_PRIVATE
    src/platform/private/Window.cpp
    src/platform/private/Input.cpp
    src/platform/private/Timer.cpp
)

set(UTILS_PRIVATE
    src/utils/private/Logger.cpp
    src/utils/private/FileUtils.cpp
    src/utils/private/ImageLoader.cpp
)

add_library(Engine
    ${MATH_PRIVATE}
    ${RENDERER_PRIVATE}
    ${PLATFORM_PRIVATE}
    ${UTILS_PRIVATE}
)

target_include_directories(Engine PUBLIC
    src/math/public
    src/renderer/public
    src/platform/public
    src/utils/public
)

source_group("Math/Public" FILES
    src/math/public/Vec2.h
    src/math/public/Vec3.h
    src/math/public/Vec4.h
    src/math/public/Mat3.h
    src/math/public/Mat4.h
    src/math/public/Quaternion.h
    src/math/public/Geometry.h
)
source_group("Math/Private" FILES ${MATH_PRIVATE})

source_group("Renderer/Public" FILES
    src/renderer/public/Renderer.h
    src/renderer/public/Shader.h
    src/renderer/public/Material.h
    src/renderer/public/Light.h
    src/renderer/public/Camera.h
    src/renderer/public/Scene.h
)
source_group("Renderer/Private" FILES ${RENDERER_PRIVATE})

source_group("Platform/Public" FILES
    src/platform/public/Window.h
    src/platform/public/Input.h
    src/platform/public/Timer.h
)
source_group("Platform/Private" FILES ${PLATFORM_PRIVATE})

source_group("Utils/Public" FILES
    src/utils/public/Logger.h
    src/utils/public/FileUtils.h
    src/utils/public/ImageLoader.h
)
source_group("Utils/Private" FILES ${UTILS_PRIVATE})
