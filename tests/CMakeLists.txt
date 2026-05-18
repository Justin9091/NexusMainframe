# tests/CMakeLists.txt
# Note: Catch2 is already fetched in cmake/Dependencies.cmake.
#       enable_testing() is called from the root when NEXUS_BUILD_TESTS=ON.

add_executable(nexus_tests
        config/source/JsonConfigSourceTests.cpp
        Event/EventBusTests.cpp
)

# ── Build into a predictable location for all generators ─────────────────────
set_target_properties(nexus_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                "${CMAKE_BINARY_DIR}/tests"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_BINARY_DIR}/tests"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_BINARY_DIR}/tests"
)

# ── Link ──────────────────────────────────────────────────────────────────────
target_link_libraries(nexus_tests PRIVATE
        nexus_core
        Catch2::Catch2WithMain
)

# Coverage is opt-in via nexus_set_compile_options — no separate target needed.
if(NEXUS_ENABLE_COV)
    nexus_set_compile_options(nexus_tests)
endif()

# ── Make the nexus_core shared library findable at test-run time ──────────────
if(WIN32 OR APPLE)
    # Copy the .dll / .dylib next to the test binary after every build.
    add_custom_command(TARGET nexus_tests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:nexus_core>
            $<TARGET_FILE_DIR:nexus_tests>
            COMMENT "Copying nexus_core shared library to test output directory"
    )
else()
    # Linux: embed an RPATH so the dynamic linker finds the library without copying.
    set_target_properties(nexus_tests PROPERTIES
            BUILD_RPATH   "${CMAKE_BINARY_DIR}"
            INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif()

# ── Test discovery ────────────────────────────────────────────────────────────
# PRE_TEST: Catch2 enumerates test cases at ctest-run time, not configure time.
# This means adding a new TEST_CASE doesn't require a full re-configure.
catch_discover_tests(nexus_tests
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
        TEST_PREFIX       "nexus::"
        PROPERTIES        LABELS "unit" TIMEOUT 60
        DISCOVERY_MODE    PRE_TEST
)