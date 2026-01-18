#!/bin/bash
set -e

# Create nexus user and group if they don't exist
if ! getent group nexus >/dev/null; then
    groupadd --system nexus
fi

if ! getent passwd nexus >/dev/null; then
    useradd --system --gid nexus --home-dir /usr/local/var/nexus \
            --shell /usr/sbin/nologin --comment "Nexus Service User" nexus
fi

# Create necessary directories
mkdir -p /usr/local/var/nexus
mkdir -p /usr/local/var/nexus/logs
mkdir -p /usr/local/var/nexus/data
mkdir -p /usr/local/var/nexus/modules

# Set ownership
chown -R nexus:nexus /usr/local/var/nexus

# Set permissions
chmod 755 /usr/local/var/nexus
chmod 755 /usr/local/var/nexus/logs
chmod 755 /usr/local/var/nexus/data
chmod 755 /usr/local/var/nexus/modules

# Reload systemd
systemctl daemon-reload

# Enable and start service
systemctl enable nexus.service

# Start the service
if systemctl is-active --quiet nexus.service; then
    echo "Restarting Nexus service..."
    systemctl restart nexus.service
else
    echo "Starting Nexus service..."
    systemctl start nexus.service
fi

echo "Nexus Main Frame service has been installed and started"
echo "Check status with: systemctl status nexus.service"
echo "View logs with: journalctl -u nexus.service -f"

exit 0