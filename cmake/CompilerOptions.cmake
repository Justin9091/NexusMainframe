# ─────────────────────────────────────────────────────────────
# Nexus Compiler Options
# Stable + fast setup for MinGW / GCC / MSVC
# ASan DISABLED for MinGW stability
# ─────────────────────────────────────────────────────────────

function(nexus_set_compile_options target)

    # ─────────────────────────────────────────────────────────
    # Warnings
    # ─────────────────────────────────────────────────────────
    if(MSVC)
        target_compile_options(${target} PRIVATE
                /W4
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
                -Wno-unused-parameter
                -fdiagnostics-color=always
        )
    endif()

    # ─────────────────────────────────────────────────────────
    # Build type flags (optimized for dev speed)
    # ─────────────────────────────────────────────────────────
    if(MSVC)
        target_compile_options(${target} PRIVATE
                $<$<CONFIG:Debug>:/Od /Zi>
                $<$<CONFIG:Release>:/O2 /DNDEBUG>
                $<$<CONFIG:RelWithDebInfo>:/O2 /Zi>
                $<$<CONFIG:MinSizeRel>:/O1 /DNDEBUG>
        )
    else()
        target_compile_options(${target} PRIVATE
                $<$<CONFIG:Debug>:-Og -g3>
                $<$<CONFIG:Release>:-O3 -DNDEBUG>
                $<$<CONFIG:RelWithDebInfo>:-O2 -g>
                $<$<CONFIG:MinSizeRel>:-Os -DNDEBUG>
        )
    endif()

    # ─────────────────────────────────────────────────────────
    # MinGW performance tweaks
    # ─────────────────────────────────────────────────────────
    if(MINGW)
        target_compile_options(${target} PRIVATE
                -pipe
                -Wa,-mbig-obj
                -fno-keep-inline-dllexport
        )
    endif()

    # ─────────────────────────────────────────────────────────
    # UNITY BUILD (FAST COMPILATION)
    # ─────────────────────────────────────────────────────────
    # Batch size 4: een gewijzigd bestand hercompileert maximaal 4 TUs.
    # Hoger = snellere clean build, langzamere incremental.
    option(NEXUS_FAST_BUILD "Enable unity builds for faster compile" ON)

    if(NEXUS_FAST_BUILD)
        set_target_properties(${target} PROPERTIES
                UNITY_BUILD ON
                UNITY_BUILD_BATCH_SIZE 4
        )
    endif()

    # ─────────────────────────────────────────────────────────
    # AddressSanitizer (DISABLED ON PURPOSE)
    # ─────────────────────────────────────────────────────────
    # Reason:
    # - MinGW GCC 13 + CLion toolchain has broken ASan runtime
    # - Unity builds + static libs cause linker failures (__asan_* undefined)
    #
    # If you need ASan → use WSL2 or Linux toolchain
    # ─────────────────────────────────────────────────────────
    option(NEXUS_ENABLE_ASAN "Enable AddressSanitizer" OFF)

    if(NEXUS_ENABLE_ASAN)
        message(WARNING "ASan is disabled in this build system (MinGW incompatible).")
    endif()

    # ─────────────────────────────────────────────────────────
    # Coverage (optional)
    # ─────────────────────────────────────────────────────────
    option(NEXUS_ENABLE_COV "Enable coverage" OFF)

    if(NEXUS_ENABLE_COV AND NOT MSVC)
        target_compile_options(${target} PRIVATE
                --coverage
                -O0
                -g
        )
        target_link_options(${target} PRIVATE --coverage)
    endif()

endfunction()