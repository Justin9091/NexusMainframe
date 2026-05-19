include(FetchContent)

# ─────────────────────────────────────────────────────────────
# Global performance settings
# ─────────────────────────────────────────────────────────────
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "" FORCE)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps_cache)

# ─────────────────────────────────────────────────────────────
# Configure timing start
# ─────────────────────────────────────────────────────────────
string(TIMESTAMP NEXUS_CONFIGURE_START_TIME "%s")

message(STATUS "[nexus] configure started")

# ─────────────────────────────────────────────────────────────
# Helper: system package or FetchContent + timing
# ─────────────────────────────────────────────────────────────
macro(nexus_find_or_fetch)
    cmake_parse_arguments(FOF "" "PACKAGE;VERSION;GIT_REPO;GIT_TAG" "" ${ARGN})

    find_package(${FOF_PACKAGE} ${FOF_VERSION} QUIET)

    if(NOT ${FOF_PACKAGE}_FOUND)
        message(STATUS "fetching [deps] fetching ${FOF_PACKAGE} (${FOF_GIT_TAG})")

        string(TIMESTAMP _start "%s")

        FetchContent_Declare(${FOF_PACKAGE}
                GIT_REPOSITORY ${FOF_GIT_REPO}
                GIT_TAG        ${FOF_GIT_TAG}
                GIT_SHALLOW    TRUE
        )

        FetchContent_MakeAvailable(${FOF_PACKAGE})

        string(TIMESTAMP _end "%s")
        math(EXPR _duration "${_end} - ${_start}")

        message(STATUS "[time] [deps] ${FOF_PACKAGE} took ${_duration}s")

    else()
        message(STATUS "found [deps] ${FOF_PACKAGE} found system")
    endif()
endmacro()

# ─────────────────────────────────────────────────────────────
# spdlog
# ─────────────────────────────────────────────────────────────
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)

nexus_find_or_fetch(
        PACKAGE  spdlog
        VERSION  1.12
        GIT_REPO https://github.com/gabime/spdlog.git
        GIT_TAG  v1.12.0
)

# ─────────────────────────────────────────────────────────────
# httplib (header-only)
# ─────────────────────────────────────────────────────────────
FetchContent_Declare(httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG        v0.18.1
        GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(httplib)

# ─────────────────────────────────────────────────────────────
# croncpp
# ─────────────────────────────────────────────────────────────
FetchContent_Declare(croncpp
        GIT_REPOSITORY https://github.com/mariusbancila/croncpp.git
        GIT_TAG        master
        GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(croncpp)

if(NOT TARGET croncpp::croncpp)
    add_library(croncpp::croncpp INTERFACE IMPORTED)
    target_include_directories(croncpp::croncpp INTERFACE
            "${croncpp_SOURCE_DIR}/include")
endif()

# ─────────────────────────────────────────────────────────────
# nlohmann json
# ─────────────────────────────────────────────────────────────
nexus_find_or_fetch(
        PACKAGE  nlohmann_json
        VERSION  3.11
        GIT_REPO https://github.com/nlohmann/json.git
        GIT_TAG  v3.11.3
)

# ─────────────────────────────────────────────────────────────
# CURL (system preferred, fallback build)
# ─────────────────────────────────────────────────────────────
find_package(CURL QUIET)

if(NOT CURL_FOUND)
    message(STATUS "fetching [deps] curl not found → building from source")

    string(TIMESTAMP _curl_start "%s")

    set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

    if(WIN32)
        set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
        set(CURL_STATICLIB OFF CACHE BOOL "" FORCE)
        set(CMAKE_USE_SCHANNEL ON CACHE BOOL "" FORCE)
        set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
        set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)
    else()
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
        set(CURL_STATICLIB ON CACHE BOOL "" FORCE)
        set(CURL_USE_OPENSSL ON CACHE BOOL "" FORCE)
    endif()

    foreach(proto LDAP LDAPS TELNET DICT FILE TFTP RTSP POP3 IMAP SMTP GOPHER MQTT)
        set(CURL_DISABLE_${proto} ON CACHE BOOL "" FORCE)
    endforeach()

    set(ENABLE_MANUAL OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(curl
            GIT_REPOSITORY https://github.com/curl/curl.git
            GIT_TAG        curl-8_5_0
            GIT_SHALLOW    TRUE
    )

    FetchContent_MakeAvailable(curl)

    string(TIMESTAMP _curl_end "%s")
    math(EXPR _curl_time "${_curl_end} - ${_curl_start}")

    message(STATUS "[time] curl build took ${_curl_time}s")
endif()

# ─────────────────────────────────────────────────────────────
# Catch2 (tests)
# ─────────────────────────────────────────────────────────────
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

# ─────────────────────────────────────────────────────────────
# Configure time END (FIXED - NO CMAKE_TIME BUG)
# ─────────────────────────────────────────────────────────────
string(TIMESTAMP NEXUS_CONFIGURE_END_TIME "%s")

math(EXPR NEXUS_CONFIGURE_DURATION
        "${NEXUS_CONFIGURE_END_TIME} - ${NEXUS_CONFIGURE_START_TIME}"
)

message(STATUS "────────────────────────────")
message(STATUS "[time] Configure time: ${NEXUS_CONFIGURE_DURATION}s")
message(STATUS "────────────────────────────")