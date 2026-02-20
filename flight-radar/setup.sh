#!/bin/bash
# FlightRadar / ADS-B Receiver Setup Script
# For Raspberry Pi 3B with RTL-SDR dongle
#
# Usage: sudo ./setup.sh
# Run on a fresh Raspberry Pi OS Lite installation

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log() { echo -e "${GREEN}[+]${NC} $1"; }
warn() { echo -e "${YELLOW}[!]${NC} $1"; }
err() { echo -e "${RED}[!]${NC} $1"; exit 1; }

# Must run as root
if [ "$EUID" -ne 0 ]; then
    err "Please run as root: sudo ./setup.sh"
fi

echo "============================================"
echo "  ADS-B Receiver Setup for Raspberry Pi 3B"
echo "============================================"
echo ""

# ------------------------------------------
# Step 0: Gather configuration
# ------------------------------------------
log "We need your location for accurate aircraft tracking."
echo ""

read -p "Enter your latitude (e.g., 40.7608): " LATITUDE
read -p "Enter your longitude (e.g., -111.8910): " LONGITUDE
read -p "Enter your altitude in feet (e.g., 4500): " ALTITUDE

if [ -z "$LATITUDE" ] || [ -z "$LONGITUDE" ] || [ -z "$ALTITUDE" ]; then
    err "Latitude, longitude, and altitude are required."
fi

# Convert altitude to meters for readsb
ALTITUDE_M=$(echo "$ALTITUDE * 0.3048" | bc -l | cut -d. -f1)

echo ""
log "Location set: ${LATITUDE}, ${LONGITUDE} at ${ALTITUDE}ft (${ALTITUDE_M}m)"
echo ""

# ------------------------------------------
# Step 1: System update and dependencies
# ------------------------------------------
log "Updating system packages..."
apt update && apt upgrade -y

log "Installing dependencies..."
apt install -y \
    git \
    build-essential \
    debhelper \
    libusb-1.0-0-dev \
    pkg-config \
    libncurses-dev \
    librtlsdr-dev \
    librtlsdr0 \
    rtl-sdr \
    lighttpd \
    bc \
    wget \
    curl

# Blacklist the default DVB-T drivers so RTL-SDR works
log "Blacklisting default DVB-T drivers..."
cat > /etc/modprobe.d/blacklist-rtlsdr.conf << 'EOF'
blacklist dvb_usb_rtl28xxu
blacklist rtl2832
blacklist rtl2830
EOF

# ------------------------------------------
# Step 2: Install readsb (ADS-B decoder)
# ------------------------------------------
log "Installing readsb (ADS-B decoder)..."

# Use the wiedehopf readsb package repo
bash -c "$(wget -O - https://github.com/wiedehopf/adsb-scripts/raw/master/readsb-install.sh)" _ --lat "$LATITUDE" --lon "$LONGITUDE" --alt "$ALTITUDE_M" --modeac --gain -10

log "readsb installed and configured."

# Write readsb config
mkdir -p /etc/default
cat > /etc/default/readsb << EOF
# readsb configuration
READSB_DEVICE_TYPE="rtlsdr"
READSB_GAIN="autogain"
READSB_LAT="$LATITUDE"
READSB_LON="$LONGITUDE"
READSB_ALT="${ALTITUDE_M}m"
READSB_MODEAC="yes"
READSB_RX_LOCATION_ACCURACY="2"
READSB_STATS_RANGE="yes"
READSB_NET="yes"
READSB_NET_CONNECTOR="beast_out,30005"
READSB_NET_SBS_PORT="30003"
READSB_NET_RAW_OUTPUT_PORT="30002"
READSB_NET_RAW_INPUT_PORT="30001"
READSB_NET_BEAST_INPUT_PORT="30004"
READSB_NET_BEAST_OUTPUT_PORT="30005"
EOF

systemctl restart readsb
systemctl enable readsb

# ------------------------------------------
# Step 3: Install tar1090 (local web map)
# ------------------------------------------
log "Installing tar1090 (aircraft map)..."
bash -c "$(wget -O - https://github.com/wiedehopf/tar1090/raw/master/install.sh)"

log "tar1090 installed. Access at http://$(hostname -I | awk '{print $1}')/tar1090"

# ------------------------------------------
# Step 4: Install graphs1090 (performance graphs)
# ------------------------------------------
log "Installing graphs1090 (performance monitoring)..."
bash -c "$(wget -O - https://github.com/wiedehopf/graphs1090/raw/master/install.sh)"

log "graphs1090 installed. Access at http://$(hostname -I | awk '{print $1}')/graphs1090"

# ------------------------------------------
# Step 5: Install FlightRadar24 feeder
# ------------------------------------------
echo ""
log "Installing FlightRadar24 feeder (fr24feed)..."
warn "You'll be prompted to sign up or enter an existing sharing key."
warn "If you don't have a key yet, select 'yes' to sign up for a new one."
echo ""
read -p "Install FlightRadar24 feeder? (y/n): " INSTALL_FR24

if [ "$INSTALL_FR24" = "y" ] || [ "$INSTALL_FR24" = "Y" ]; then
    bash -c "$(wget -O - https://repo-feed.flightradar24.com/install_fr24_rpi.sh)"
    systemctl enable fr24feed
    log "fr24feed installed."
    log "FR24 status page: http://$(hostname -I | awk '{print $1}'):8754"
else
    warn "Skipping FlightRadar24 feeder. You can install it later."
fi

# ------------------------------------------
# Step 6: Install FlightAware (piaware)
# ------------------------------------------
echo ""
log "Installing FlightAware feeder (piaware)..."
read -p "Install FlightAware feeder? (y/n): " INSTALL_FA

if [ "$INSTALL_FA" = "y" ] || [ "$INSTALL_FA" = "Y" ]; then
    wget https://www.flightaware.com/adsb/piaware/files/packages/pool/piaware/f/flightaware-apt-repository/flightaware-apt-repository_1.2_all.deb
    dpkg -i flightaware-apt-repository_1.2_all.deb
    apt update
    apt install -y piaware
    piaware-config allow-auto-update yes
    piaware-config allow-manual-update yes
    systemctl enable piaware
    systemctl start piaware
    rm -f flightaware-apt-repository_1.2_all.deb
    log "piaware installed."
    warn "Claim your receiver at: https://flightaware.com/adsb/piaware/claim"
else
    warn "Skipping FlightAware feeder. You can install it later."
fi

# ------------------------------------------
# Step 7: Enable SPI/I2C (for future use)
# ------------------------------------------
log "Enabling SPI and I2C interfaces..."
raspi-config nonint do_spi 0
raspi-config nonint do_i2c 0

# ------------------------------------------
# Step 8: Set hostname
# ------------------------------------------
log "Setting hostname to 'flight-radar'..."
hostnamectl set-hostname flight-radar
echo "127.0.1.1       flight-radar" >> /etc/hosts

# ------------------------------------------
# Summary
# ------------------------------------------
PI_IP=$(hostname -I | awk '{print $1}')

echo ""
echo "============================================"
echo "  Setup Complete!"
echo "============================================"
echo ""
echo "  Aircraft Map:    http://${PI_IP}/tar1090"
echo "  Perf Graphs:     http://${PI_IP}/graphs1090"
if [ "$INSTALL_FR24" = "y" ] || [ "$INSTALL_FR24" = "Y" ]; then
echo "  FR24 Status:     http://${PI_IP}:8754"
fi
echo ""
echo "  Location: ${LATITUDE}, ${LONGITUDE} @ ${ALTITUDE}ft"
echo ""
if [ "$INSTALL_FA" = "y" ] || [ "$INSTALL_FA" = "Y" ]; then
echo "  IMPORTANT: Claim your FlightAware receiver at:"
echo "  https://flightaware.com/adsb/piaware/claim"
echo ""
fi
echo "  A reboot is recommended to load driver changes."
echo ""
read -p "Reboot now? (y/n): " DO_REBOOT

if [ "$DO_REBOOT" = "y" ] || [ "$DO_REBOOT" = "Y" ]; then
    log "Rebooting..."
    reboot
else
    warn "Remember to reboot before plugging in the RTL-SDR dongle."
fi
