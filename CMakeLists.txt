cmake_minimum_required(VERSION 3.20)

project(NexusCore
        VERSION 1.0.0
        DESCRIPTION "Nexus modular framework core"
        LANGUAGES CXX
)

# ─────────────────────────────────────────────
# C++ standard
# ─────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ─────────────────────────────────────────────
# Output dirs
# ─────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ─────────────────────────────────────────────
# Speedups
# ─────────────────────────────────────────────
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "" FORCE)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM AND NOT NEXUS_ENABLE_COV)
    message(STATUS "ccache found: ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER   "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# ─────────────────────────────────────────────
# MinGW fix
# ─────────────────────────────────────────────
if(MINGW)
    add_compile_options(-Wa,-mbig-obj)
endif()

# ─────────────────────────────────────────────
# Dependencies
# ─────────────────────────────────────────────
include(cmake/Dependencies.cmake)
include(cmake/CompilerOptions.cmake)
include(cmake/GoCLI.cmake)

# ─────────────────────────────────────────────
# CORE LIBRARY
# ─────────────────────────────────────────────
add_library(nexus_core SHARED)

target_sources(nexus_core PRIVATE
        src/event/EventBus.cpp

        src/modules/Module.cpp
        src/modules/ModuleLoader.cpp
        src/modules/ModuleManager.cpp
        src/modules/ModuleStateStore.cpp
        src/modules/operations/ModuleOperationsFactory.cpp

        src/scheduler/Scheduler.cpp

        src/config/ConfigProviderFactory.cpp
        src/config/provider/ConfigProvider.cpp
        src/config/source/JsonConfigSource.cpp
        src/config/NexusConfig.cpp

        src/mqtt/MQTTClient.cpp

        src/services/ModuleService.cpp
        src/result/Result.cpp
        src/result/ModuleResult.cpp

        src/devices/Device.cpp
        src/devices/DeviceRegistry.cpp
        src/services/DeviceService.cpp
        src/rooms/Room.cpp
        src/rooms/RoomRegistry.cpp
        src/services/RoomService.cpp
        src/http/IController.cpp
        src/http/controllers/ModuleController.cpp
        src/http/controllers/DeviceController.cpp
        src/http/controllers/RoomController.cpp
        src/http/controllers/ConfigController.cpp
        src/services/MqttService.cpp
        src/services/HttpService.cpp

        src/pathing/PathResolver.cpp
        src/pathing/resolver/UnixPathResolver.cpp
        src/pathing/resolver/WindowsPathResolver.cpp
        src/pathing/resolver/PathResolverFactory.cpp

        src/mainframe/service/WindowsServiceHost.cpp
        src/mainframe/service/ServiceHostFactory.cpp

        src/server/HttpServer.cpp
        src/server/Endpoint.cpp
        src/server/WsServer.cpp

        src/services/WsService.cpp
        src/services/McpService.cpp

        src/mcp/McpServer.cpp

        src/logging/LogBroadcaster.cpp
        src/logging/WebLogger.cpp
        src/services/LogService.cpp

        NexusMainFrame.cpp
)

# ─────────────────────────────────────────────
# Include dirs
# ─────────────────────────────────────────────
target_include_directories(nexus_core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# ─────────────────────────────────────────────
# Link libs
# ─────────────────────────────────────────────
target_link_libraries(nexus_core
        PUBLIC
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        croncpp::croncpp
        httplib::httplib
        PRIVATE
        CURL::libcurl
)

# Windows system libs
if(WIN32)
    target_link_libraries(nexus_core PRIVATE
            ws2_32
            wsock32
            iphlpapi
    )
else()
    find_package(OpenSSL REQUIRED)
    target_link_libraries(nexus_core PRIVATE
            OpenSSL::SSL
            OpenSSL::Crypto
    )
endif()

# Export symbols
set_target_properties(nexus_core PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
)

# ─────────────────────────────────────────────
# PCH
# ─────────────────────────────────────────────
target_precompile_headers(nexus_core PUBLIC
        <nlohmann/json.hpp>
        <spdlog/spdlog.h>
        <httplib.h>
        <croncpp.h>

        <string>
        <vector>
        <memory>
        <functional>
        <optional>
        <unordered_map>
        <stdexcept>
)

# ─────────────────────────────────────────────
# Compile options
# ─────────────────────────────────────────────
nexus_set_compile_options(nexus_core)

# ─────────────────────────────────────────────
# Runtime fixes (Windows)
# ─────────────────────────────────────────────
if(MINGW)
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    foreach(dll libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
        if(EXISTS "${MINGW_BIN_DIR}/${dll}")
            add_custom_command(TARGET nexus_core POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${MINGW_BIN_DIR}/${dll}"
                    $<TARGET_FILE_DIR:nexus_core>
            )
        endif()
    endforeach()
endif()

# ─────────────────────────────────────────────
# Tests
# ─────────────────────────────────────────────
if(NEXUS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ─────────────────────────────────────────────
# Executable (FIXED)
# ─────────────────────────────────────────────
add_executable(NexusMainFrame
        main.cpp
)

target_link_libraries(NexusMainFrame PRIVATE nexus_core)

if(WIN32)
    target_link_libraries(NexusMainFrame PRIVATE ws2_32 wsock32)

    # 🔥 FORCE CONSOLE SUBSYSTEM (fix WinMain error)
    target_link_options(NexusMainFrame PRIVATE -mconsole)
endif()

# ─────────────────────────────────────────────
# Optional modules
# ─────────────────────────────────────────────
option(NEXUS_BUILD_CHROMECAST_MODULE "Build the Chromecast control module" OFF)
if(NEXUS_BUILD_CHROMECAST_MODULE)
    add_subdirectory(modules/NexusChromecastModule)
endif()