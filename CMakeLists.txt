cmake_minimum_required(VERSION 3.20)

project(NexusCore
        VERSION 1.0.0
        DESCRIPTION "Nexus modular framework core"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Position Independent Code for all targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ------------------------------------------------------
# Dependencies via FetchContent
# ------------------------------------------------------
include(FetchContent)

# ---------- spdlog ----------
set(spdlog_FOUND FALSE)
message(STATUS "Fetching spdlog from GitHub...")

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
        GIT_SHALLOW TRUE
)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

# ---------- croncpp ----------
message(STATUS "Fetching croncpp from GitHub...")

FetchContent_Declare(
        croncpp
        GIT_REPOSITORY https://github.com/mariusbancila/croncpp.git
        GIT_TAG master
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(croncpp)

# ---------- nlohmann_json ----------
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann/json not found, fetching from GitHub...")
    FetchContent_Declare(
            nlohmann_json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.11.3
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(nlohmann_json)
    message(STATUS "nlohmann/json fetched successfully")
else()
    message(STATUS "Using system nlohmann/json")
endif()

# ------------------------------------------------------
# Core library
# ------------------------------------------------------
add_library(nexus_core SHARED
        Logger/Logger.hpp
        Event/EventBus.hpp
        Event/EventBus.cpp
        Modules/IModule.hpp
        Modules/ModuleLoader.cpp
        Modules/ModuleLoader.hpp
        NexusMainFrame.cpp
        NexusMainFrame.hpp
        Scheduler/Scheduler.hpp
        Task/ITask.hpp
        Scheduler/Scheduler.cpp
        Config/ConfigProviderFactory.cpp
        Config/ConfigProviderFactory.hpp
        Config/Provider/ConfigProvider.hpp
        Config/Source/IConfigSource.hpp
        Config/Provider/ConfigProvider.cpp
        Config/Source/JsonConfigSource.hpp
        Config/Source/JsonConfigSource.cpp
)

# Include directories
target_include_directories(nexus_core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${spdlog_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${croncpp_SOURCE_DIR}/include>
)

# Link libraries
target_link_libraries(nexus_core
        PRIVATE
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

# Ensure PIC is enabled for nexus_core
set_target_properties(nexus_core PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

# Platform-specific settings
if(WIN32)
    # Windows export macros
    target_compile_definitions(nexus_core
            PRIVATE NEXUS_CORE_EXPORTS
            INTERFACE NEXUS_CORE_IMPORTS
    )

    # Set Windows-specific properties
    set_target_properties(nexus_core PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>"
    )
endif()

# ------------------------------------------------------
# Module interface
# ------------------------------------------------------
add_library(nexus_module_interface INTERFACE)
target_link_libraries(nexus_module_interface INTERFACE nexus_core)
target_include_directories(nexus_module_interface
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# ------------------------------------------------------
# Main executable
# ------------------------------------------------------
add_executable(NexusMainFrame main.cpp)
target_link_libraries(NexusMainFrame PRIVATE nexus_core)

# ------------------------------------------------------
# Export targets for external modules
# ------------------------------------------------------
export(
        TARGETS
        nexus_core
        nexus_module_interface
        NAMESPACE Nexus::
        FILE "${CMAKE_CURRENT_BINARY_DIR}/NexusCoreTargets.cmake"
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/NexusCoreConfig.cmake"
        "include(\"\${CMAKE_CURRENT_LIST_DIR}/NexusCoreTargets.cmake\")"
)