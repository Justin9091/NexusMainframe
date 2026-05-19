# tests/CMakeLists.txt
# Note: Catch2 is already fetched in cmake/Dependencies.cmake.
#       enable_testing() is called from the root when NEXUS_BUILD_TESTS=ON.

add_executable(nexus_tests
        event/EventBusTests.cpp
        result/ResultTests.cpp
        result/ModuleResultTests.cpp
        devices/DeviceTests.cpp
        devices/DeviceRegistryTests.cpp
        rooms/RoomTests.cpp
        rooms/RoomRegistryTests.cpp
        config/NexusConfigTests.cpp
        config/JsonConfigSourceTests.cpp
        scheduler/SchedulerTests.cpp
        task/EventTaskTriggerTests.cpp
)

# ── Build next to nexus_core so all DLLs are in the same directory ────────────
# On Windows, nexus_tests.exe needs nexus_core.dll and libcurl.dll at runtime.
# Putting everything in bin/ avoids having to copy individual DLLs.
set_target_properties(nexus_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                "${CMAKE_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_BINARY_DIR}/bin"
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

# ── Runtime library path ──────────────────────────────────────────────────────
if(NOT WIN32 AND NOT APPLE)
    # Linux: embed an RPATH so the dynamic linker finds nexus_core.so.
    set_target_properties(nexus_tests PROPERTIES
            BUILD_RPATH   "${CMAKE_BINARY_DIR}/bin"
            INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif()

# ── Test discovery ────────────────────────────────────────────────────────────
catch_discover_tests(nexus_tests
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        TEST_PREFIX       "nexus::"
        PROPERTIES        LABELS "unit" TIMEOUT 60
        DISCOVERY_MODE    PRE_TEST
)
