#!/bin/bash
set -e

# Stop and disable service before removal
if command -v systemctl &> /dev/null; then
    if systemctl is-active --quiet nexus.service; then
        systemctl stop nexus.service
    fi
    if systemctl is-enabled --quiet nexus.service; then
        systemctl disable nexus.service
    fi
fi

exit 0