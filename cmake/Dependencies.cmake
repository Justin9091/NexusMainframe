include(FetchContent)

# ─────────────────────────────────────────────────────────────────────────────
# Helper: try system package first, fall back to FetchContent.
# Usage:
#   nexus_find_or_fetch(
#       PACKAGE   nlohmann_json
#       VERSION   3.11
#       GIT_REPO  https://github.com/nlohmann/json.git
#       GIT_TAG   v3.11.3
#   )
# ─────────────────────────────────────────────────────────────────────────────
macro(nexus_find_or_fetch)
    cmake_parse_arguments(FOF "" "PACKAGE;VERSION;GIT_REPO;GIT_TAG" "" ${ARGN})
    find_package(${FOF_PACKAGE} ${FOF_VERSION} QUIET)
    if(NOT ${FOF_PACKAGE}_FOUND)
        message(STATUS "${FOF_PACKAGE} not found on system, fetching ${FOF_TAG}...")
        FetchContent_Declare(${FOF_PACKAGE}
                GIT_REPOSITORY ${FOF_GIT_REPO}
                GIT_TAG        ${FOF_GIT_TAG}
                GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(${FOF_PACKAGE})
    else()
        message(STATUS "${FOF_PACKAGE} ${${FOF_PACKAGE}_VERSION} found on system")
    endif()
endmacro()

# ── spdlog ────────────────────────────────────────────────────────────────────
set(SPDLOG_INSTALL       OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL  OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED  OFF CACHE BOOL "" FORCE)
nexus_find_or_fetch(
        PACKAGE  spdlog
        VERSION  1.12
        GIT_REPO https://github.com/gabime/spdlog.git
        GIT_TAG  v1.12.0
)

# ── cpp-httplib ───────────────────────────────────────────────────────────────
# Header-only; no system package lookup needed.
FetchContent_Declare(httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG        v0.18.1
        GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(httplib)

# ── croncpp ───────────────────────────────────────────────────────────────────
FetchContent_Declare(croncpp
        GIT_REPOSITORY https://github.com/mariusbancila/croncpp.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(croncpp)

# Provide an ALIAS target if croncpp doesn't export one
if(NOT TARGET croncpp::croncpp)
    add_library(croncpp::croncpp INTERFACE IMPORTED)
    target_include_directories(croncpp::croncpp INTERFACE
            "${croncpp_SOURCE_DIR}/include")
endif()

# ── nlohmann_json ─────────────────────────────────────────────────────────────
nexus_find_or_fetch(
        PACKAGE  nlohmann_json
        VERSION  3.11
        GIT_REPO https://github.com/nlohmann/json.git
        GIT_TAG  v3.11.3
)

# ── curl ──────────────────────────────────────────────────────────────────────
# Prefer the system curl — building from source takes 1-3 minutes on its own.
# On Ubuntu:  sudo apt install libcurl4-openssl-dev
# On Windows: install via vcpkg or provide a prebuilt binary.
find_package(CURL QUIET)
if(NOT CURL_FOUND)
    message(STATUS "System curl not found — building from source (this will be slow).")
    message(STATUS "  Consider: sudo apt install libcurl4-openssl-dev")

    set(BUILD_CURL_EXE    OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(CURL_STATICLIB    ON  CACHE BOOL "" FORCE)

    set(CURL_DISABLE_HTTP  OFF CACHE BOOL "" FORCE)
    set(CURL_DISABLE_HTTPS OFF CACHE BOOL "" FORCE)

    if(WIN32)
        set(CMAKE_USE_SCHANNEL ON  CACHE BOOL "" FORCE)
        set(CURL_USE_SCHANNEL  ON  CACHE BOOL "" FORCE)
        set(CURL_USE_OPENSSL   OFF CACHE BOOL "" FORCE)
    else()
        set(CMAKE_USE_SCHANNEL OFF CACHE BOOL "" FORCE)
        set(CURL_USE_SCHANNEL  OFF CACHE BOOL "" FORCE)
        set(CURL_USE_OPENSSL   ON  CACHE BOOL "" FORCE)
    endif()

    # Disable unused protocols to speed up the curl build
    foreach(proto LDAP LDAPS TELNET DICT FILE TFTP RTSP POP3 IMAP SMTP GOPHER MQTT)
        set(CURL_DISABLE_${proto} ON CACHE BOOL "" FORCE)
    endforeach()

    set(ENABLE_MANUAL OFF CACHE BOOL "" FORCE)

    # Temporarily disable testing while building curl
    set(_saved_BUILD_TESTING ${BUILD_TESTING})
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(curl
            GIT_REPOSITORY https://github.com/curl/curl.git
            GIT_TAG        curl-8_5_0
            GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(curl)

    set(BUILD_TESTING ${_saved_BUILD_TESTING} CACHE BOOL "" FORCE)
endif()

# ── Catch2 (only when tests are enabled) ─────────────────────────────────────
if(NEXUS_BUILD_TESTS)
    nexus_find_or_fetch(
            PACKAGE  Catch2
            VERSION  3
            GIT_REPO https://github.com/catchorg/Catch2.git
            GIT_TAG  v3.5.1
    )
    if(catch2_SOURCE_DIR)
        list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
    endif()
    include(CTest)
    include(Catch)
endif()