cmake_minimum_required(VERSION 3.20)

project(NexusCLI
        VERSION 1.0.0
        DESCRIPTION "Nexus command-line interface client"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------ CLI executable ------------------
add_executable(nexus
        ../../includes/cli/NexusCLI.h
        NexusCLI.cpp
        Main.cpp
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(nexus PRIVATE ws2_32)
endif()

# Set output name to 'nexus' on all platforms
set_target_properties(nexus PROPERTIES
        OUTPUT_NAME "nexus"
)

# ------------------ Installation ------------------
install(TARGETS nexus
        RUNTIME DESTINATION bin
)

# ------------------ Optional: Add to PATH instructions ------------------
message(STATUS "")
message(STATUS "=====================================================")
message(STATUS "Nexus CLI will be installed to: ${CMAKE_INSTALL_PREFIX}/bin")
message(STATUS "")
message(STATUS "To use 'nexus' command from anywhere, add to PATH:")
if(WIN32)
    message(STATUS "  setx PATH \"%PATH%;${CMAKE_INSTALL_PREFIX}/bin\"")
else()
    message(STATUS "  export PATH=\"${CMAKE_INSTALL_PREFIX}/bin:$PATH\"")
    message(STATUS "  (Add to ~/.bashrc or ~/.zshrc for persistence)")
endif()
message(STATUS "=====================================================")
message(STATUS "")