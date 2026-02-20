#!/bin/bash
# Pi-hole Setup Script
# For Raspberry Pi 3B - DNS ad blocker for entire network
#
# Usage: sudo ./setup.sh
# Run AFTER first-boot.sh

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log() { echo -e "${GREEN}[+]${NC} $1"; }
warn() { echo -e "${YELLOW}[!]${NC} $1"; }
err() { echo -e "${RED}[!]${NC} $1"; exit 1; }

if [ "$EUID" -ne 0 ]; then
    err "Please run as root: sudo ./setup.sh"
fi

echo "============================================"
echo "  Pi-hole Setup"
echo "============================================"
echo ""

PI_IP=$(hostname -I | awk '{print $1}')

# ------------------------------------------
# Step 1: Set static IP
# ------------------------------------------
log "Pi-hole works best with a static IP."
echo "  Current IP: ${PI_IP}"
echo ""
read -p "Use ${PI_IP} as the static IP? (y/n): " USE_CURRENT_IP

if [ "$USE_CURRENT_IP" != "y" ] && [ "$USE_CURRENT_IP" != "Y" ]; then
    read -p "Enter desired static IP: " PI_IP
fi

read -p "Enter your router/gateway IP (e.g., 10.1.10.1): " GATEWAY_IP

# Configure static IP via dhcpcd
cat >> /etc/dhcpcd.conf << EOF

# Static IP for Pi-hole
interface eth0
static ip_address=${PI_IP}/24
static routers=${GATEWAY_IP}
static domain_name_servers=127.0.0.1
EOF

log "Static IP configured: ${PI_IP}"

# ------------------------------------------
# Step 2: Install Pi-hole (unattended)
# ------------------------------------------
log "Creating Pi-hole unattended config..."

mkdir -p /etc/pihole

cat > /etc/pihole/setupVars.conf << EOF
PIHOLE_INTERFACE=eth0
PIHOLE_DNS_1=1.1.1.1
PIHOLE_DNS_2=1.0.0.1
QUERY_LOGGING=true
INSTALL_WEB_SERVER=true
INSTALL_WEB_INTERFACE=true
LIGHTTPD_ENABLED=true
CACHE_SIZE=10000
DNS_FQDN_REQUIRED=true
DNS_BOGUS_PRIV=true
DNSMASQ_LISTENING=local
WEBPASSWORD=
BLOCKING_ENABLED=true
DNSSEC=false
REV_SERVER=false
EOF

log "Installing Pi-hole..."
curl -sSL https://install.pi-hole.net | bash /dev/stdin --unattended

# ------------------------------------------
# Step 3: Set web admin password
# ------------------------------------------
log "Setting Pi-hole admin password..."
echo ""
warn "Set the web admin password:"
pihole -a -p

# ------------------------------------------
# Step 4: Add extra blocklists
# ------------------------------------------
log "Adding recommended blocklists..."

# Pi-hole default list is already included
# Adding popular community lists
sqlite3 /etc/pihole/gravity.db "INSERT OR IGNORE INTO adlist (address, enabled, comment) VALUES
('https://raw.githubusercontent.com/StevenBlack/hosts/master/hosts', 1, 'StevenBlack Unified'),
('https://raw.githubusercontent.com/hagezi/dns-blocklists/main/domains/multi.txt', 1, 'HaGeZi Multi Normal'),
('https://big.oisd.nl/domainswild', 1, 'OISD Big');"

# Update gravity (download blocklists)
log "Updating gravity (downloading blocklists)..."
pihole -g

# ------------------------------------------
# Step 5: Add local DNS entries for homelab
# ------------------------------------------
log "Adding local DNS entries for your services..."

cat > /etc/pihole/custom.list << EOF
# Brandon's homelab services
# M900 (main server)
10.1.10.XXX     jazzlive.local
10.1.10.XXX     nhlteams.local
10.1.10.XXX     claudecad.local
10.1.10.XXX     m900.local

# Unraid
10.1.10.193     unraid.local

# Pis (update IPs after all are online)
# ${PI_IP}      pihole.local
# x.x.x.x      flight-radar.local
# x.x.x.x      uptime-kuma.local
# x.x.x.x      pi-spare.local
EOF

warn "Edit /etc/pihole/custom.list to set your M900's actual IP address."
warn "Update Pi IPs once all are on the network."

# ------------------------------------------
# Step 6: Open firewall ports
# ------------------------------------------
log "Opening Pi-hole ports in firewall..."
ufw allow 53/tcp    # DNS
ufw allow 53/udp    # DNS
ufw allow 80/tcp    # Web admin
ufw allow 67/udp    # DHCP (if used)

# ------------------------------------------
# Summary
# ------------------------------------------
echo ""
echo "============================================"
echo "  Pi-hole Setup Complete!"
echo "============================================"
echo ""
echo "  Admin Panel:  http://${PI_IP}/admin"
echo "  DNS Server:   ${PI_IP}"
echo ""
echo "  Blocklists installed:"
echo "    - Pi-hole default"
echo "    - StevenBlack Unified"
echo "    - HaGeZi Multi Normal"
echo "    - OISD Big"
echo ""
echo "  TO DO:"
echo "  1. Set your router's DNS to ${PI_IP}"
echo "     (or configure per-device)"
echo "  2. Edit /etc/pihole/custom.list with"
echo "     your M900's actual IP"
echo "  3. Update Pi IPs once all are online"
echo ""
echo "  Manage: pihole -a -p  (change password)"
echo "          pihole status  (check status)"
echo "          pihole -g      (update blocklists)"
echo ""
