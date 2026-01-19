message(STATUS "==================== TESTS SUBDIRECTORY LOADED ====================")

include(FetchContent)

find_package(Catch2 3 QUIET)
if (NOT Catch2_FOUND)
    FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.5.1
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(Catch2)

    # Include Catch2's CMake module
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
endif()

include(Catch)

add_executable(nexus_tests
        config/source/JsonConfigSourceTests.cpp
        Event/EventBusTests.cpp
        command/CommandRegistryTests.cpp
)

target_link_libraries(nexus_tests PRIVATE
        coverage_config
        nexus_core
        Catch2::Catch2WithMain
)

# Kopieer nexus_core.dll naar test directory (Windows)
if(WIN32)
    add_custom_command(TARGET nexus_tests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:nexus_core>
            $<TARGET_FILE_DIR:nexus_tests>
            COMMENT "Copying nexus_core.dll to test directory"
    )
endif()

# Discover tests with proper configuration
catch_discover_tests(nexus_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        PROPERTIES
        LABELS "unit"
)

# Add a manual test target as fallback
add_test(
        NAME nexus_tests_manual
        COMMAND nexus_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

message(STATUS "Test executable will be: $<TARGET_FILE:nexus_tests>")