cmake_minimum_required(VERSION 3.20)

project(ro-crate-cpp LANGUAGES CXX)

include(CTest)

add_library(ro-crate-cpp INTERFACE)

target_include_directories(ro-crate-cpp
    INTERFACE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/vendor
)

target_compile_features(ro-crate-cpp INTERFACE cxx_std_20)

if(BUILD_TESTING)
    find_package(Catch2 3 REQUIRED)

    add_executable(ro-crate-cpp_tests
        tests/test.cpp
    )

    target_link_libraries(ro-crate-cpp_tests
        PRIVATE
            ro-crate-cpp
            Catch2::Catch2WithMain
    )

    include(Catch)
    catch_discover_tests(ro-crate-cpp_tests)
endif()
