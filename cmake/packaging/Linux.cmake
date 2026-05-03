# Linux.cmake - Linux-specific packaging configuration

set(CPACK_GENERATOR "DEB;RPM;TGZ")

# ------------------ Debian package configuration ------------------
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "jusraaijmakers@gmail.com")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/justin9091/nexusmainframe")

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.27)")
if(NOT CMAKE_USE_SCHANNEL)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libssl3 | libssl1.1")
endif()

# Debian control scripts
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postinst.sh")
    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
            "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postinst.sh;postinst"
            "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/prerm.sh;prerm"
            "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postrm.sh;postrm"
    )
endif()

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

# ------------------ RPM package configuration ------------------
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Applications/System")
set(CPACK_RPM_PACKAGE_URL "https://github.com/yourusername/nexusmainframe")
set(CPACK_RPM_PACKAGE_DESCRIPTION "Nexus Main Frame modular framework service for Linux systems")

set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.27")
if(NOT CMAKE_USE_SCHANNEL)
    set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES}, openssl-libs >= 1.1.1")
endif()

# RPM scripts
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postinst.sh")
    set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postinst.sh")
    set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/prerm.sh")
    set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/service/linux/postrm.sh")
endif()

set(CPACK_RPM_PACKAGE_AUTOREQPROV ON)

# Exclude system directories from RPM
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION
        /etc
        /etc/systemd
        /etc/systemd/system
        /usr/local
        /usr/local/bin
)

message(STATUS "Linux packaging configured")