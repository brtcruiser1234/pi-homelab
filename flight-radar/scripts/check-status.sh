#!/bin/bash
# Quick health check for all ADS-B services
# Usage: ./check-status.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

check_service() {
    local name=$1
    local service=$2
    if systemctl is-active --quiet "$service" 2>/dev/null; then
        echo -e "  ${GREEN}●${NC} $name ($service) - running"
    else
        echo -e "  ${RED}●${NC} $name ($service) - stopped"
    fi
}

check_url() {
    local name=$1
    local url=$2
    if curl -s -o /dev/null -w "%{http_code}" "$url" | grep -q "200"; then
        echo -e "  ${GREEN}●${NC} $name - $url"
    else
        echo -e "  ${RED}●${NC} $name - $url (unreachable)"
    fi
}

PI_IP=$(hostname -I | awk '{print $1}')

echo ""
echo "=== ADS-B Receiver Status ==="
echo ""
echo "Services:"
check_service "readsb (decoder)" "readsb"
check_service "tar1090 (map)" "tar1090"
check_service "graphs1090" "collectd"
check_service "fr24feed (FR24)" "fr24feed"
check_service "piaware (FlightAware)" "piaware"
check_service "lighttpd (web)" "lighttpd"

echo ""
echo "Web Interfaces:"
check_url "Aircraft Map" "http://localhost/tar1090/"
check_url "Graphs" "http://localhost/graphs1090/"
check_url "FR24 Status" "http://localhost:8754/"

echo ""
echo "RTL-SDR Dongle:"
if lsusb 2>/dev/null | grep -qi "RTL2838\|RTL-SDR\|Realtek.*2832\|0bda:2838"; then
    echo -e "  ${GREEN}●${NC} RTL-SDR dongle detected"
else
    echo -e "  ${RED}●${NC} RTL-SDR dongle NOT detected (check USB connection)"
fi

echo ""
echo "Aircraft Stats (last 60s):"
if [ -f /run/readsb/stats.json ]; then
    MESSAGES=$(cat /run/readsb/stats.json | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('last1min',{}).get('messages',0))" 2>/dev/null || echo "N/A")
    AIRCRAFT=$(cat /run/readsb/stats.json | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('last1min',{}).get('tracks',{}).get('all',0))" 2>/dev/null || echo "N/A")
    echo "  Messages: $MESSAGES"
    echo "  Aircraft tracked: $AIRCRAFT"
else
    echo "  No stats available (readsb may not be running)"
fi

echo ""
echo "Network: http://${PI_IP}/tar1090"
echo ""
