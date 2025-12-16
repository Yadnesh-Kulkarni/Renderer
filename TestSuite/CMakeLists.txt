cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(unit_tests src/test_main.cpp)
target_link_libraries(unit_tests
    PRIVATE
        Engine
        gtest_main
        gmock_main
)
target_include_directories(unit_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/include
)
set_target_properties(unit_tests PROPERTIES FOLDER "TestSuite")

# Enable test discovery
include(GoogleTest)
gtest_discover_tests(unit_tests)