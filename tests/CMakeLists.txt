# ------------------------------------------------------
# Find or fetch Catch2
# ------------------------------------------------------
find_package(Catch2 3 QUIET)

if(NOT Catch2_FOUND)
    message(STATUS "Catch2 not found, fetching from GitHub...")
    include(FetchContent)
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.5.1
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
else()
    message(STATUS "Using system Catch2")
endif()

include(Catch)

# ------------------------------------------------------
# Test executable
# ------------------------------------------------------
add_executable(nexus_tests
        test_main.cpp
        config/source/JsonConfigSourceTests.cpp
)

target_link_libraries(nexus_tests
        PRIVATE
        nexus_core
        Catch2::Catch2WithMain
)

target_include_directories(nexus_tests
        PRIVATE
        ${CMAKE_SOURCE_DIR}
)

# Register tests with CTest
catch_discover_tests(nexus_tests)