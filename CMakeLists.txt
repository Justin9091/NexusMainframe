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
if (NOT nlohmann_json_FOUND)
    FetchContent_Declare(
            nlohmann_json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.11.3
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif ()

# ------------------ CURL (STATIC, HTTPS) ------------------
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CURL_STATICLIB ON CACHE BOOL "" FORCE)

# Enable HTTPS
set(CURL_DISABLE_HTTP OFF CACHE BOOL "" FORCE)
set(CURL_DISABLE_HTTPS OFF CACHE BOOL "" FORCE)

# SSL configuration per platform
if (WIN32)
    # SSL via Windows Schannel
    set(CMAKE_USE_SCHANNEL ON CACHE BOOL "" FORCE)
    set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
    set(CURL_USE_OPENSSL OFF CACHE BOOL "" FORCE)
else ()
    # SSL via OpenSSL on Unix/Linux
    set(CMAKE_USE_SCHANNEL OFF CACHE BOOL "" FORCE)
    set(CURL_USE_SCHANNEL OFF CACHE BOOL "" FORCE)
    set(CURL_USE_OPENSSL ON CACHE BOOL "" FORCE)
endif ()

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
        includes/mainframe/service/IServiceHost.hpp
        includes/mainframe/service/WindowsServiceHost.hpp
        src/mainframe/service/WindowsServiceHost.cpp
        includes/mainframe/service/ServiceHostFactory.hpp
        src/mainframe/service/ServiceHostFactory.cpp
        includes/mainframe/service/ForegroundServiceHost.hpp
)

set_target_properties(nexus_core PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib
        LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib
)

target_include_directories(nexus_core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${spdlog_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${croncpp_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
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

if (WIN32)
    target_link_libraries(nexus_core PRIVATE ws2_32)
    target_compile_definitions(nexus_core
            PRIVATE NEXUS_CORE_EXPORTS
            INTERFACE NEXUS_CORE_IMPORTS
    )
    set(CMAKE_USE_SCHANNEL ON CACHE INTERNAL "")
else ()
    find_package(OpenSSL REQUIRED)
    target_link_libraries(nexus_core PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif ()

# ------------------ Module interface ------------------
add_library(nexus_module_interface INTERFACE)
target_link_libraries(nexus_module_interface INTERFACE nexus_core)
target_include_directories(nexus_module_interface
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# ------------------ Main executable ------------------
add_executable(NexusMainFrame main.cpp)
target_link_libraries(NexusMainFrame PRIVATE nexus_core)

if(MINGW)
    # Vind de MinGW bin folder van de compiler
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    set(MINGW_RUNTIME_DLLS
            libstdc++-6.dll
            libgcc_s_seh-1.dll
            libwinpthread-1.dll
    )

    foreach(dll ${MINGW_RUNTIME_DLLS})
        add_custom_command(TARGET NexusMainFrame POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MINGW_BIN_DIR}/${dll}"
                $<TARGET_FILE_DIR:NexusMainFrame>)
    endforeach()
endif()

# ------------------ Installation ------------------
# Install main executable
install(TARGETS NexusMainFrame
        RUNTIME DESTINATION bin
        COMPONENT Application
)

# Install core library with export (CRITICAL: must include EXPORT)
install(TARGETS nexus_core
        EXPORT NexusCoreTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        COMPONENT Libraries
)

# Install module interface with export
install(TARGETS nexus_module_interface
        EXPORT NexusCoreTargets
        COMPONENT Development
)

# Install MinGW runtime DLLs on Windows if needed
if(MINGW)
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    install(FILES
            "${MINGW_BIN_DIR}/libstdc++-6.dll"
            "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
            "${MINGW_BIN_DIR}/libwinpthread-1.dll"
            DESTINATION bin
            COMPONENT Runtime
            OPTIONAL
    )
endif()

# Export targets
install(EXPORT NexusCoreTargets
        FILE NexusCoreTargets.cmake
        NAMESPACE Nexus::
        DESTINATION lib/cmake/NexusCore
        COMPONENT Development
)

# Install headers for development
install(DIRECTORY includes/
        DESTINATION include
        COMPONENT Development
)

# ------------------ Platform-specific service files ------------------
if(WIN32)
    # Install PowerShell service scripts
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/service/windows/install-service.ps1")
        install(FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/service/windows/install-service.ps1
                ${CMAKE_CURRENT_SOURCE_DIR}/service/windows/uninstall-service.ps1
                DESTINATION bin
                COMPONENT Application
        )
    endif()

elseif(APPLE)
    # Install launchd plist
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/service/macos/com.yourcompany.nexus.plist")
        install(FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/service/macos/com.yourcompany.nexus.plist
                DESTINATION /Library/LaunchDaemons
                PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
                COMPONENT Application
        )

        # Install helper scripts
        install(PROGRAMS
                ${CMAKE_CURRENT_SOURCE_DIR}/service/macos/postinstall.sh
                DESTINATION bin
                COMPONENT Application
        )
    endif()
elseif(UNIX)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/nexus.service")
        install(FILES
                ${CMAKE_CURRENT_SOURCE_DIR}/service/linux/nexus.service
                DESTINATION share/nexusmainframe/systemd
                PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
                COMPONENT Application
        )

        # Install helper scripts
        install(PROGRAMS
                ${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postinst.sh
                ${CMAKE_CURRENT_SOURCE_DIR}/service/linux/prerm.sh
                ${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postrm.sh
                DESTINATION bin
                COMPONENT Application
        )
    endif()
endif()

# ------------------ CPack Configuration ------------------
include(CPack)

# Basic package information
set(CPACK_PACKAGE_NAME "NexusMainFrame")
set(CPACK_PACKAGE_VENDOR "Nexus")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Nexus Main Frame - Modular framework service")
set(CPACK_PACKAGE_DESCRIPTION "Nexus Main Frame is a modular framework for building extensible services")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "NexusMainFrame")

# Resource files (optional - create these if you want)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
endif()

# Components
set(CPACK_COMPONENTS_ALL Application Libraries Runtime Development)
set(CPACK_COMPONENT_APPLICATION_DISPLAY_NAME "Nexus Main Frame Application")
set(CPACK_COMPONENT_APPLICATION_DESCRIPTION "Main application and service files")
set(CPACK_COMPONENT_APPLICATION_REQUIRED TRUE)

set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Core Libraries")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "Nexus core libraries")
set(CPACK_COMPONENT_LIBRARIES_REQUIRED TRUE)

set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Runtime Dependencies")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION "Required runtime libraries")
set(CPACK_COMPONENT_RUNTIME_REQUIRED TRUE)

set(CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME "Development Files")
set(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION "Headers and CMake files for module development")
set(CPACK_COMPONENT_DEVELOPMENT_REQUIRED FALSE)

# Platform-specific package configuration
if(WIN32)
    # ==================== WINDOWS ====================
    set(CPACK_GENERATOR "NSIS;ZIP")

    # NSIS (installer) configuration
    set(CPACK_NSIS_DISPLAY_NAME "Nexus Main Frame")
    set(CPACK_NSIS_PACKAGE_NAME "Nexus Main Frame")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH ON)

    # Add icon if available
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.ico")
        set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.ico")
        set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.ico")
    endif()

    # Install/Uninstall service automatically (only if scripts exist)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/service/windows/install-service.ps1")
        set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
            ExecWait 'powershell.exe -ExecutionPolicy Bypass -File \\\"$INSTDIR\\\\bin\\\\install-service.ps1\\\"'
        ")

        set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
            ExecWait 'powershell.exe -ExecutionPolicy Bypass -File \\\"$INSTDIR\\\\bin\\\\uninstall-service.ps1\\\"'
        ")
    endif()

    # Create start menu shortcuts
    set(CPACK_NSIS_MENU_LINKS
            "bin/NexusMainFrame.exe" "Nexus Main Frame"
    )

elseif(APPLE)
    # ==================== macOS ====================
    set(CPACK_GENERATOR "DragNDrop;TGZ")

    # DMG settings
    set(CPACK_DMG_VOLUME_NAME "Nexus Main Frame")
    set(CPACK_DMG_FORMAT "UDZO")

    # macOS Bundle settings (optional)
    set(CPACK_BUNDLE_NAME "NexusMainFrame")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/service/macos/Info.plist")
        set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/service/macos/Info.plist")
    endif()

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.icns")
        set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon.icns")
    endif()

elseif(UNIX)
    # ==================== LINUX ====================
    set(CPACK_GENERATOR "DEB;RPM;TGZ")

    # ---------- DEB (Debian/Ubuntu) ----------
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "jusraaijmakers@gmail.com")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/justin9091/nexusmainframe")

    # Dependencies
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.27)")
    if(NOT CMAKE_USE_SCHANNEL)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libssl3 | libssl1.1")
    endif()

    # Control scripts (only if they exist)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postinst.sh")
        set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
                "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postinst.sh;postinst"
                "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/prerm.sh;prerm"
                "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postrm.sh;postrm"
        )
    endif()

    # Automatic dependency detection
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

    # ---------- RPM (RedHat/CentOS/Fedora) ----------
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/System")
    set(CPACK_RPM_PACKAGE_URL "https://github.com/yourusername/nexusmainframe")
    set(CPACK_RPM_PACKAGE_DESCRIPTION "Nexus Main Frame modular framework service for Linux systems")

    # Dependencies
    set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.27")
    if(NOT CMAKE_USE_SCHANNEL)
        set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES}, openssl-libs >= 1.1.1")
    endif()

    # Scripts (only if they exist)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postinst.sh")
        set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postinst.sh")
        set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/prerm.sh")
        set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postrm.sh")
    endif()

    # Automatic dependency detection
    set(CPACK_RPM_PACKAGE_AUTOREQPROV ON)

    # Exclude system directories from package
    set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
            /etc
            /etc/systemd
            /etc/systemd/system
            /usr/local
            /usr/local/bin
    )
endif()

# Archive settings (for TGZ/ZIP)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

# Source package configuration
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
        /.git
        /.github
        /.idea
        /build
        /cmake-build-*
        /.vs
        /.vscode
        /out
        *.user
        *.swp
        *.swo
        *~
)

# ------------------ Coverage Configuration ------------------
if (ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_library(coverage_config INTERFACE)

        target_compile_options(coverage_config INTERFACE
                --coverage
                -O0
                -g
        )

        target_link_options(coverage_config INTERFACE
                --coverage
        )

        # Link coverage direct aan nexus_core
        target_link_libraries(nexus_core PRIVATE coverage_config)

        message(STATUS "Code coverage enabled")
    else()
        message(WARNING "Code coverage requested but compiler is not GCC or Clang")
        add_library(coverage_config INTERFACE)
    endif()
else()
    # Maak een dummy interface als coverage uit staat
    add_library(coverage_config INTERFACE)
endif()

add_subdirectory(tests)