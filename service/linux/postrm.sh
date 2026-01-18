#!/bin/bash
set -e

# Reload systemd to forget about the service
systemctl daemon-reload

echo "Nexus Main Frame service has been removed"
echo ""
echo "Note: User data in /usr/local/var/nexus has been preserved"
echo "To completely remove all data, run:"
echo "  sudo rm -rf /usr/local/var/nexus"
echo "  sudo userdel nexus"
echo "  sudo groupdel nexus"

exit 0