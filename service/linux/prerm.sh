#!/bin/bash
set -e

# Stop the service if it's running
if systemctl is-active --quiet nexus.service; then
    echo "Stopping Nexus service..."
    systemctl stop nexus.service
fi

# Disable the service
if systemctl is-enabled --quiet nexus.service 2>/dev/null; then
    echo "Disabling Nexus service..."
    systemctl disable nexus.service
fi

exit 0