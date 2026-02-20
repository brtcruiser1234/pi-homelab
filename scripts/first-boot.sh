#!/bin/bash
# First-boot setup script for all Pi 3Bs
# Run this on every Pi after flashing Pi OS Lite
#
# Usage: sudo ./first-boot.sh
#
# This handles common setup before running the service-specific script.

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log() { echo -e "${GREEN}[+]${NC} $1"; }
warn() { echo -e "${YELLOW}[!]${NC} $1"; }
err() { echo -e "${RED}[!]${NC} $1"; exit 1; }

if [ "$EUID" -ne 0 ]; then
    err "Please run as root: sudo ./first-boot.sh"
fi

HOSTNAME=$(hostname)
echo "============================================"
echo "  First Boot Setup - ${HOSTNAME}"
echo "============================================"
echo ""

# ------------------------------------------
# Step 1: System update
# ------------------------------------------
log "Updating system packages..."
apt update && apt upgrade -y

# ------------------------------------------
# Step 2: Install common tools
# ------------------------------------------
log "Installing common utilities..."
apt install -y \
    vim \
    htop \
    curl \
    wget \
    git \
    net-tools \
    dnsutils \
    jq \
    python3 \
    python3-pip \
    ufw

# ------------------------------------------
# Step 3: Set timezone
# ------------------------------------------
log "Setting timezone to America/Denver (Mountain Time)..."
timedatectl set-timezone America/Denver

# ------------------------------------------
# Step 4: Enable SPI and I2C
# ------------------------------------------
log "Enabling SPI and I2C interfaces..."
raspi-config nonint do_spi 0
raspi-config nonint do_i2c 0

# ------------------------------------------
# Step 5: Basic firewall setup
# ------------------------------------------
log "Configuring firewall (UFW)..."
ufw default deny incoming
ufw default allow outgoing
ufw allow ssh
# Allow all traffic from local network
ufw allow from 10.1.10.0/24
ufw --force enable
log "Firewall enabled. SSH and local network traffic allowed."

# ------------------------------------------
# Step 6: SSH hardening
# ------------------------------------------
log "Hardening SSH config..."
sed -i 's/#PasswordAuthentication yes/PasswordAuthentication no/' /etc/ssh/sshd_config
sed -i 's/PasswordAuthentication yes/PasswordAuthentication no/' /etc/ssh/sshd_config
sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin no/' /etc/ssh/sshd_config
systemctl restart sshd
warn "SSH password auth disabled. Key-only access from now on."

# ------------------------------------------
# Step 7: Reduce swap usage (saves SD card writes)
# ------------------------------------------
log "Reducing swap usage..."
echo "vm.swappiness=10" >> /etc/sysctl.conf
sysctl -p

# ------------------------------------------
# Step 8: Set up log rotation to save SD card
# ------------------------------------------
log "Configuring log rotation..."
cat > /etc/logrotate.d/rsyslog-custom << 'EOF'
/var/log/syslog
/var/log/messages
{
    rotate 3
    daily
    maxsize 10M
    missingok
    notifempty
    compress
    delaycompress
}
EOF

# ------------------------------------------
# Summary
# ------------------------------------------
PI_IP=$(hostname -I | awk '{print $1}')

echo ""
echo "============================================"
echo "  First Boot Complete - ${HOSTNAME}"
echo "============================================"
echo ""
echo "  Hostname:  ${HOSTNAME}"
echo "  IP:        ${PI_IP}"
echo "  Timezone:  $(timedatectl show -p Timezone --value)"
echo "  SSH:       Key-only (password auth disabled)"
echo "  Firewall:  Enabled (SSH + local network)"
echo "  Swap:      Reduced (swappiness=10)"
echo ""
echo "  Next: Run the service-specific setup script."
echo ""
