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
endif()

include(Catch)

add_executable(nexus_tests
        config/source/JsonConfigSourceTests.cpp
        Event/EventBusTests.cpp
        command/CommandRegistryTests.cpp
)

# Link libraries inclusief coverage
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

catch_discover_tests(nexus_tests)