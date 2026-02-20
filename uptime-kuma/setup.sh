#!/bin/bash
# Uptime Kuma Setup Script
# For Raspberry Pi 3B - Service monitoring dashboard
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
echo "  Uptime Kuma Setup"
echo "============================================"
echo ""

PI_IP=$(hostname -I | awk '{print $1}')

# ------------------------------------------
# Step 1: Install Node.js 18 LTS
# ------------------------------------------
log "Installing Node.js 18 LTS..."

curl -fsSL https://deb.nodesource.com/setup_18.x | bash -
apt install -y nodejs

NODE_VER=$(node --version)
log "Node.js ${NODE_VER} installed."

# ------------------------------------------
# Step 2: Install Uptime Kuma
# ------------------------------------------
log "Installing Uptime Kuma..."

# Create app user
useradd -r -s /bin/false uptime-kuma 2>/dev/null || true

# Clone and install
git clone https://github.com/louislam/uptime-kuma.git /opt/uptime-kuma
cd /opt/uptime-kuma
npm run setup

# Set ownership
chown -R uptime-kuma:uptime-kuma /opt/uptime-kuma

log "Uptime Kuma installed to /opt/uptime-kuma"

# ------------------------------------------
# Step 3: Create systemd service
# ------------------------------------------
log "Creating systemd service..."

cat > /etc/systemd/system/uptime-kuma.service << 'EOF'
[Unit]
Description=Uptime Kuma - Service Monitor
After=network.target

[Service]
Type=simple
User=uptime-kuma
WorkingDirectory=/opt/uptime-kuma
ExecStart=/usr/bin/node server/server.js
Restart=always
RestartSec=10
Environment=NODE_ENV=production
Environment=UPTIME_KUMA_PORT=3001

[Install]
WantedBy=multi-user.target
EOF

systemctl daemon-reload
systemctl enable uptime-kuma
systemctl start uptime-kuma

log "Uptime Kuma service started."

# ------------------------------------------
# Step 4: Open firewall port
# ------------------------------------------
log "Opening port 3001 in firewall..."
ufw allow 3001/tcp

# ------------------------------------------
# Step 5: Create monitor seed script
# ------------------------------------------
log "Creating monitor seed script..."

cat > /home/brandon/seed-monitors.sh << 'SEEDEOF'
#!/bin/bash
# Seed script for Uptime Kuma monitors
# Run this AFTER completing the web setup (creating admin account)
#
# This uses the Uptime Kuma API to add monitors for all services.
# You'll need to get your API key from the web UI first.
#
# Usage: ./seed-monitors.sh <api-url> <username> <password>

API_URL="${1:-http://localhost:3001}"
USERNAME="${2}"
PASSWORD="${3}"

if [ -z "$USERNAME" ] || [ -z "$PASSWORD" ]; then
    echo "Usage: ./seed-monitors.sh <api-url> <username> <password>"
    echo ""
    echo "Or just add monitors manually in the web UI at ${API_URL}"
    echo ""
    echo "Recommended monitors to add:"
    echo ""
    echo "  HTTP Monitors:"
    echo "    - Jazz-Stats:        http://10.1.10.XXX:8888"
    echo "    - NHL Tracker:       http://10.1.10.XXX:3050"
    echo "    - ClaudeCAD:         http://10.1.10.XXX:4000"
    echo "    - Pi-hole Admin:     http://pihole.local/admin"
    echo "    - FlightRadar Map:   http://flight-radar.local/tar1090"
    echo "    - FR24 Feeder:       http://flight-radar.local:8754"
    echo ""
    echo "  Ping Monitors:"
    echo "    - M900 Server:       10.1.10.XXX"
    echo "    - Unraid NAS:        10.1.10.193"
    echo "    - Pi-hole:           pihole.local"
    echo "    - Flight Radar Pi:   flight-radar.local"
    echo "    - Pi Spare:          pi-spare.local"
    echo ""
    echo "  DNS Monitors:"
    echo "    - jazzlive.taylorshome.cc"
    echo "    - nhlteams.taylorshome.cc"
    echo "    - claudecad.taylorshome.cc"
    echo ""
    echo "Replace 10.1.10.XXX with your M900's actual IP."
    exit 0
fi

echo "Manual setup recommended - add monitors via ${API_URL}"
echo "See the list above for recommended monitors."
SEEDEOF

chmod +x /home/brandon/seed-monitors.sh
chown brandon:brandon /home/brandon/seed-monitors.sh

# ------------------------------------------
# Summary
# ------------------------------------------
echo ""
echo "============================================"
echo "  Uptime Kuma Setup Complete!"
echo "============================================"
echo ""
echo "  Dashboard:  http://${PI_IP}:3001"
echo ""
echo "  First visit:"
echo "  1. Go to http://${PI_IP}:3001"
echo "  2. Create an admin account"
echo "  3. Add monitors for your services"
echo ""
echo "  Run ./seed-monitors.sh to see recommended"
echo "  monitors to add for your homelab."
echo ""
echo "  Service management:"
echo "    sudo systemctl status uptime-kuma"
echo "    sudo systemctl restart uptime-kuma"
echo "    journalctl -u uptime-kuma -f"
echo ""
