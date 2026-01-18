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

# CURL
# ------------------ CURL (STATIC, HTTPS, SCHANNEL) ------------------

set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CURL_STATICLIB ON CACHE BOOL "" FORCE)

# SSL via Windows Schannel
set(CMAKE_USE_SCHANNEL ON CACHE BOOL "" FORCE)
set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)

# Enable HTTPS
set(CURL_DISABLE_HTTP OFF CACHE BOOL "" FORCE)
set(CURL_DISABLE_HTTPS OFF CACHE BOOL "" FORCE)

# Disable unused protocols
set(CURL_DISABLE_LDAP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_LDAPS ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_TELNET ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_DICT ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_FILE ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_TFTP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_RTSP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_POP3 ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_IMAP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_SMTP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_GOPHER ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_MQTT ON CACHE BOOL "" FORCE)

set(ENABLE_MANUAL OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG curl-8_5_0
)

FetchContent_MakeAvailable(curl)


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
        includes/commands/DisableModuleCommand.hpp
        src/commands/DisableModuleCommand.cpp
        includes/Modules/operations/IModuleOperations.hpp
        includes/Modules/operations/WindowsModuleOperations.hpp
        includes/Modules/operations/ModuleOperationsFactory.hpp
        src/modules/operations/ModuleOperationsFactory.cpp
        includes/Modules/operations/LinuxModuleOperations.hpp
        includes/Modules/ModuleManager.hpp
        includes/Modules/ModuleUnloader.hpp
        includes/Modules/LoadedModule.hpp
        src/modules/ModuleUnloader.cpp
        src/modules/ModuleManager.cpp
        includes/commands/ScanCommand.hpp
        includes/Modules/scanner/IModuleScanner.hpp
        includes/Modules/scanner/WindowsModuleScanner.hpp
        includes/Modules/scanner/LinuxModuleScanner.hpp
        includes/Modules/scanner/ModuleScannerFactory.hpp
        src/commands/ScanCommand.cpp
        includes/config/Adapter/IConfigAdapter.hpp
        includes/config/Adapter/LoadedModuleAdapter.hpp
        includes/config/Adapter/LoadedModuleListAdapter.hpp
        includes/commands/MonitorCommand.hpp
        src/commands/MonitorCommand.cpp
        includes/pathing/resolver/IPathResolver.hpp
        includes/pathing/resolver/WindowsPathResolver.hpp
        includes/pathing/resolver/UnixPathResolver.hpp
        includes/pathing/resolver/PathResolverFactory.hpp
        src/pathing/resolver/UnixPathResolver.cpp
        src/pathing/resolver/WindowsPathResolver.cpp
        src/pathing/resolver/PathResolverFactory.cpp
        includes/exceptions/PathResolutionException.hpp
        includes/pathing/PathValidator.hpp
        includes/pathing/PathManager.hpp
        src/pathing/PathResolver.cpp
        includes/commands/DownloadCommand.hpp
        src/commands/DownloadCommand.cpp
)

target_include_directories(nexus_core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${spdlog_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${croncpp_SOURCE_DIR}/include>
)

target_link_libraries(nexus_core
        PRIVATE
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        CURL::libcurl
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
    set(CMAKE_USE_SCHANNEL ON CACHE INTERNAL "")
else ()
    find_package(OpenSSL REQUIRED)
    target_link_libraries(nexus_core PRIVATE OpenSSL::SSL OpenSSL::Crypto)
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
