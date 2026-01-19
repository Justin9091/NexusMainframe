#!/bin/bash
set -e

# Remove service file after package removal
if [ -f /etc/systemd/system/nexus.service ]; then
    rm /etc/systemd/system/nexus.service
    systemctl daemon-reload
fi

exit 0