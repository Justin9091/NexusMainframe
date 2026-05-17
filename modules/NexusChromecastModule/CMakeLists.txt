cmake_minimum_required(VERSION 3.20)
project(NexusChromecastModule
    VERSION 1.0.0
    DESCRIPTION "Chromecast control module for NexusMainFrame"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─────────────────────────────────────────────────────────────────────────────
# When built standalone (not as part of the main NexusCore project),
# fetch the dependencies ourselves.
# ─────────────────────────────────────────────────────────────────────────────
if(NOT TARGET nexus_core)
    message(STATUS "[ChromecastModule] standalone build — fetching deps")

    set(NEXUS_CORE_INCLUDE_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/../../includes"
        CACHE PATH "Path to NexusMainFrame/includes")

    include(FetchContent)

    find_package(spdlog QUIET)
    if(NOT spdlog_FOUND)
        set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG        v1.12.0
            GIT_SHALLOW    TRUE)
        FetchContent_MakeAvailable(spdlog)
    endif()

    find_package(nlohmann_json QUIET)
    if(NOT nlohmann_json_FOUND)
        FetchContent_Declare(nlohmann_json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG        v3.11.3
            GIT_SHALLOW    TRUE)
        FetchContent_MakeAvailable(nlohmann_json)
    endif()
endif()

# ─────────────────────────────────────────────────────────────────────────────
# OpenSSL — required for TLS connections to Chromecast (port 8009)
# On Windows/MinGW:  pacman -S mingw-w64-x86_64-openssl
# On Ubuntu/Debian:  apt install libssl-dev
# ─────────────────────────────────────────────────────────────────────────────
find_package(OpenSSL REQUIRED)

# ─────────────────────────────────────────────────────────────────────────────
# Module target
# ─────────────────────────────────────────────────────────────────────────────
add_library(NexusChromecastModule SHARED
    src/ChromecastModule.cpp
    src/cast/CastProto.cpp
    src/cast/TlsSocket.cpp
    src/cast/CastChannel.cpp
    src/cast/CastDevice.cpp
)

target_include_directories(NexusChromecastModule PRIVATE src)

if(TARGET nexus_core)
    # Built as part of the main project — use nexus_core directly
    target_link_libraries(NexusChromecastModule PRIVATE
        nexus_core
        OpenSSL::SSL
        OpenSSL::Crypto
    )
else()
    # Standalone — include nexus_core headers, resolve symbols at load time
    target_include_directories(NexusChromecastModule PRIVATE
        ${NEXUS_CORE_INCLUDE_DIR}
    )
    target_link_libraries(NexusChromecastModule PRIVATE
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        OpenSSL::SSL
        OpenSSL::Crypto
    )
endif()

if(WIN32)
    target_link_libraries(NexusChromecastModule PRIVATE ws2_32)
    # Only export createModule, not everything
    set_target_properties(NexusChromecastModule PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS OFF
    )
endif()

if(MINGW)
    target_compile_options(NexusChromecastModule PRIVATE -Wa,-mbig-obj)
endif()

# ─────────────────────────────────────────────────────────────────────────────
# Post-build: copy DLL to the nexus modules directory
# ─────────────────────────────────────────────────────────────────────────────
if(WIN32)
    set(_NEXUS_MODULES "$ENV{USERPROFILE}/.nexus/modules/downloaded")
else()
    set(_NEXUS_MODULES "$ENV{HOME}/.nexus/modules/downloaded")
endif()

add_custom_command(TARGET NexusChromecastModule POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${_NEXUS_MODULES}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:NexusChromecastModule>
        "${_NEXUS_MODULES}/"
    COMMENT "Deploying NexusChromecastModule → ${_NEXUS_MODULES}"
)
