# ─────────────────────────────────────────────────────────────────────────────
# nexus_set_compile_options(<target>)
#
# Apply project-wide compiler warnings and flags to <target>.
# Keep this separate from Dependencies.cmake so it's easy to audit.
# ─────────────────────────────────────────────────────────────────────────────
function(nexus_set_compile_options target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
                /W4
                /w14640   # thread-unsafe static member init
                /permissive-
        )
    else()
        target_compile_options(${target} PRIVATE
                -Wall
                -Wextra
                -Wpedantic
                -Wshadow
                -Wnon-virtual-dtor
                -Woverloaded-virtual
                -Wno-unused-parameter  # too noisy in interface code
        )
    endif()

    # Coverage (GCC / Clang only)
    if(NEXUS_ENABLE_COV AND NOT MSVC)
        target_compile_options(${target} PRIVATE --coverage -O0 -g)
        target_link_options(${target}    PRIVATE --coverage)
    endif()
endfunction()