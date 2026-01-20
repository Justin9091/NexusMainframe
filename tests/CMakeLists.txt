message(STATUS "==================== TESTS SUBDIRECTORY LOADED ====================")

include(FetchContent)

# Fetch Catch2
find_package(Catch2 3 QUIET)
if (NOT Catch2_FOUND)
    message(STATUS "Catch2 not found, fetching from GitHub...")
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.5.1
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(Catch2)

    # Add Catch2 CMake modules to path
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    message(STATUS "Catch2 fetched successfully")
endif()

# Include Catch2 integration
include(Catch)

# Enable testing
enable_testing()

# Create test executable
add_executable(nexus_tests
        config/source/JsonConfigSourceTests.cpp
        Event/EventBusTests.cpp
        command/CommandRegistryTests.cpp
)

# Force consistent output directory across all generators
set_target_properties(nexus_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/tests
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/tests
)

# Link libraries
target_link_libraries(nexus_tests PRIVATE
        coverage_config
        nexus_core
        Catch2::Catch2WithMain
)

# Copy nexus_core shared library to test directory
if(WIN32)
    # Copy to consistent location for all configurations
    add_custom_command(TARGET nexus_tests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:nexus_core>
            ${CMAKE_BINARY_DIR}/tests
            COMMENT "Copying nexus_core.dll to test directory"
    )
elseif(APPLE)
    add_custom_command(TARGET nexus_tests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:nexus_core>
            ${CMAKE_BINARY_DIR}/tests
            COMMENT "Copying nexus_core.dylib to test directory"
    )
else()
    # Linux - set RPATH to find shared library
    set_target_properties(nexus_tests PROPERTIES
            BUILD_RPATH ${CMAKE_BINARY_DIR}/lib:${CMAKE_BINARY_DIR}
            INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
    )
endif()

# Discover tests - this runs AFTER the executable is built
catch_discover_tests(nexus_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        PROPERTIES
        LABELS "unit"
        DISCOVERY_MODE PRE_TEST  # Critical: discover tests at test-time, not configure-time
)

# Also add a manual test as fallback
add_test(
        NAME AllTests
        COMMAND nexus_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set_tests_properties(AllTests PROPERTIES
        TIMEOUT 300
        LABELS "unit;catch2;fallback"
)

message(STATUS "Test executable target: nexus_tests")
message(STATUS "Test executable location: $<TARGET_FILE:nexus_tests>")