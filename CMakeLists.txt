cmake_minimum_required(VERSION 3.20)

project(NexusCore
        VERSION 1.0.0
        DESCRIPTION "Nexus modular framework core"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ------------------ Options ------------------
option(BUILD_TESTING "Build tests" ON)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

# ------------------ Dependencies ------------------
include(FetchContent)

# spdlog
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

# croncpp
FetchContent_Declare(
        croncpp
        GIT_REPOSITORY https://github.com/mariusbancila/croncpp.git
        GIT_TAG master
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(croncpp)

# nlohmann_json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    FetchContent_Declare(
            nlohmann_json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.11.3
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# Catch2 (tests)
if(BUILD_TESTING)
    find_package(Catch2 3 QUIET)
    if(NOT Catch2_FOUND)
        FetchContent_Declare(
                Catch2
                GIT_REPOSITORY https://github.com/catchorg/Catch2.git
                GIT_TAG v3.5.1
                GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(Catch2)
        list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    endif()
    include(CTest)
    include(Catch)
endif()

# ------------------ Core library ------------------
add_library(nexus_core SHARED
        includes/Logger/Logger.hpp
        includes/Event/EventBus.hpp
        src/event/EventBus.cpp
        includes/Modules/IModule.hpp
        src/modules/ModuleLoader.cpp
        includes/Modules/ModuleLoader.hpp
        NexusMainFrame.cpp
        NexusMainFrame.hpp
        includes/Scheduler/Scheduler.hpp
        includes/Task/ITask.hpp
        src/scheduler/Scheduler.cpp
        src/config/ConfigProviderFactory.cpp
        includes/config/ConfigProviderFactory.hpp
        includes/config/Provider/ConfigProvider.hpp
        includes/config/Source/IConfigSource.hpp
        src/config/provider/ConfigProvider.cpp
        includes/config/Source/JsonConfigSource.hpp
        src/config/source/JsonConfigSource.cpp
        includes/Result/Result.hpp
        src/ipc/CommandServer.cpp
        includes/IPC/CommandServer.hpp
        includes/mqtt/MQTTClient.hpp
        src/mqtt/MQTTClient.cpp
        includes/commands/Command.hpp
        includes/commands/ListCommand.hpp
        src/commands/ListCommands.cpp
        includes/commands/CommandRegistry.hpp
        src/commands/Command.cpp
        includes/commands/HelpCommand.hpp
        src/commands/HelpCommand.cpp
        includes/commands/EnableModuleCommand.hpp
        src/commands/EnableModuleCommand.cpp
)

target_include_directories(nexus_core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>  # ← WIJZIGING: voeg /includes toe
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>           # ← Behoud deze voor root files
        $<BUILD_INTERFACE:${spdlog_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${croncpp_SOURCE_DIR}/include>
)

target_link_libraries(nexus_core
        PRIVATE
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

set_target_properties(nexus_core PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        WINDOWS_EXPORT_ALL_SYMBOLS ON
)

if(WIN32)
    target_link_libraries(nexus_core PRIVATE ws2_32)
    target_compile_definitions(nexus_core
            PRIVATE NEXUS_CORE_EXPORTS
            INTERFACE NEXUS_CORE_IMPORTS
    )
endif()

# ------------------ Module interface ------------------
add_library(nexus_module_interface INTERFACE)
target_link_libraries(nexus_module_interface INTERFACE nexus_core)
target_include_directories(nexus_module_interface
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# ------------------ Main executable ------------------
add_executable(NexusMainFrame main.cpp)
target_link_libraries(NexusMainFrame PRIVATE nexus_core)

# ------------------ Export package ------------------
install(TARGETS nexus_core nexus_module_interface
        EXPORT NexusCoreTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

install(EXPORT NexusCoreTargets
        FILE NexusCoreTargets.cmake
        NAMESPACE Nexus::
        DESTINATION lib/cmake/NexusCore
)

install(DIRECTORY includes/ DESTINATION include)
