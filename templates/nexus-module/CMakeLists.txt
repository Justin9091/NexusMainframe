cmake_minimum_required(VERSION 3.20)

# ─── Rename "MyModule" everywhere to match your module name ───────────────────
project(MyModule VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── Pull NexusMainFrame headers ──────────────────────────────────────────────
include(FetchContent)

FetchContent_Declare(
    nexus_headers
    GIT_REPOSITORY https://github.com/Justin9091/NexusMainFrame.git
    GIT_TAG        main
    SOURCE_SUBDIR  _no_build   # only headers needed, skip CMake build
)
FetchContent_MakeAvailable(nexus_headers)

# spdlog (same version as nexus_core uses)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.12.0
)
FetchContent_MakeAvailable(spdlog)

# ─── Module target ────────────────────────────────────────────────────────────
add_library(MyModule SHARED
    MyModule.cpp
)

target_include_directories(MyModule PRIVATE
    ${nexus_headers_SOURCE_DIR}/includes
    ${nexus_headers_SOURCE_DIR}
)

target_link_libraries(MyModule PRIVATE
    spdlog::spdlog
)

# Modules moeten geen "lib"-prefix krijgen (nexus zoekt naar "MyModule.dll")
set_target_properties(MyModule PROPERTIES PREFIX "")

# ─── Installeer naar de nexus modules-map ─────────────────────────────────────
if(WIN32)
    set(NEXUS_MODULES_DIR "$ENV{USERPROFILE}/.nexus/modules/downloaded")
else()
    set(NEXUS_MODULES_DIR "$ENV{HOME}/.nexus/modules/downloaded")
endif()

install(TARGETS MyModule
    LIBRARY DESTINATION "${NEXUS_MODULES_DIR}"
    RUNTIME DESTINATION "${NEXUS_MODULES_DIR}"
)
