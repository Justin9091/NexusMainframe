#!/bin/bash
set -e

# Create nexus user and group if they don't exist
if ! dscl . -read /Groups/_nexus &>/dev/null; then
    # Find next available GID
    MAX_GID=$(dscl . -list /Groups PrimaryGroupID | awk '{print $2}' | sort -n | tail -1)
    NEXT_GID=$((MAX_GID + 1))

    # Create group
    dscl . -create /Groups/_nexus
    dscl . -create /Groups/_nexus PrimaryGroupID $NEXT_GID
    dscl . -create /Groups/_nexus RealName "Nexus Service Group"
fi

if ! dscl . -read /Users/_nexus &>/dev/null; then
    # Find next available UID
    MAX_UID=$(dscl . -list /Users UniqueID | awk '{print $2}' | sort -n | tail -1)
    NEXT_UID=$((MAX_UID + 1))

    # Get GID
    NEXUS_GID=$(dscl . -read /Groups/_nexus PrimaryGroupID | awk '{print $2}')

    # Create user
    dscl . -create /Users/_nexus
    dscl . -create /Users/_nexus UniqueID $NEXT_UID
    dscl . -create /Users/_nexus PrimaryGroupID $NEXUS_GID
    dscl . -create /Users/_nexus UserShell /usr/bin/false
    dscl . -create /Users/_nexus RealName "Nexus Service User"
    dscl . -create /Users/_nexus NFSHomeDirectory /usr/local/var/nexus
fi

# Create necessary directories
mkdir -p /usr/local/var/nexus
mkdir -p /usr/local/var/nexus/logs
mkdir -p /usr/local/var/nexus/data
mkdir -p /usr/local/var/nexus/modules

# Set ownership
chown -R _nexus:_nexus /usr/local/var/nexus

# Set permissions
chmod 755 /usr/local/var/nexus
chmod 755 /usr/local/var/nexus/logs
chmod 755 /usr/local/var/nexus/data
chmod 755 /usr/local/var/nexus/modules

# Load the launchd service
launchctl bootout system/com.yourcompany.nexus 2>/dev/null || true
launchctl bootstrap system /Library/LaunchDaemons/com.yourcompany.nexus.plist

echo "Nexus Main Frame service has been installed and started"
echo "Check status with: launchctl print system/com.yourcompany.nexus"
echo "View logs in: /usr/local/var/nexus/logs/"

exit 0