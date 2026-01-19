#!/bin/bash
set -e

# Install systemd service if systemd is available
if command -v systemctl &> /dev/null; then
    # Copy service file to systemd directory
    if [ -f /usr/share/nexusmainframe/systemd/nexus.service ]; then
        cp /usr/share/nexusmainframe/systemd/nexus.service /etc/systemd/system/

        # Reload systemd daemon
        systemctl daemon-reload

        # Enable service (but don't start it automatically)
        systemctl enable nexus.service

        echo "NexusCore service installed and enabled."
        echo "Start the service with: sudo systemctl start nexus.service"
    fi
fi

exit 0