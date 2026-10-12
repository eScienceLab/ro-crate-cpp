cmake_minimum_required(VERSION 3.24)

project(ro-crate-cpp LANGUAGES CXX)

include(CTest)
include(FetchContent)

add_library(ro-crate-cpp INTERFACE)

target_include_directories(ro-crate-cpp
    INTERFACE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/vendor
)

target_compile_features(ro-crate-cpp INTERFACE cxx_std_20)

add_executable(example
    tests/example.cpp
)

target_link_libraries(example
    PRIVATE
        ro-crate-cpp
)

if(BUILD_TESTING)
    FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.16.0
        FIND_PACKAGE_ARGS 3
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(ro-crate-cpp_tests
        tests/integration.cpp
        tests/unit.cpp
    )

    target_link_libraries(ro-crate-cpp_tests
        PRIVATE
            ro-crate-cpp
            Catch2::Catch2WithMain
    )

    target_compile_definitions(ro-crate-cpp_tests
        PRIVATE
            TEST_SOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"
    )

    include(Catch)
    catch_discover_tests(ro-crate-cpp_tests)

    add_test(
        NAME example
        COMMAND example
    )

    set_tests_properties(example
        PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()
