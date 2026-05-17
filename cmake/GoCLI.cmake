find_program(GO_EXECUTABLE go)

if (NOT GO_EXECUTABLE)
    message(WARNING "Go not found — nexus-cli will not be built")
    return()
endif ()

add_custom_target(nexus-cli ALL
    COMMAND ${GO_EXECUTABLE} build -o ${CMAKE_BINARY_DIR}/bin/nexus${CMAKE_EXECUTABLE_SUFFIX} .
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/cli
    COMMENT "Building nexus CLI (Go)"
    VERBATIM
)
