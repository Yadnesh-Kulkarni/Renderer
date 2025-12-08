cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

# Add gtest source files directly into your test target
file(GLOB GTEST_SRC
    ${googletest_SOURCE_DIR}/googletest/src/*.cc
    ${googletest_SOURCE_DIR}/googlemock/src/*.cc
)

add_executable(unit_tests src/test_main.cpp ${GTEST_SRC})
target_include_directories(unit_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/include
    ${googletest_SOURCE_DIR}
    ${googletest_SOURCE_DIR}/googlemock/include
    ${googletest_SOURCE_DIR}/googletest/include
)
set_target_properties(unit_tests PROPERTIES FOLDER "TestSuite")

# Enable test discovery
include(GoogleTest)
gtest_discover_tests(unit_tests)