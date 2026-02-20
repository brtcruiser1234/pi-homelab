#!/bin/bash
# Uptime Kuma health check
# Usage: ./check-status.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

PI_IP=$(hostname -I | awk '{print $1}')

echo ""
echo "=== Uptime Kuma Status ==="
echo ""

echo "Service:"
if systemctl is-active --quiet uptime-kuma 2>/dev/null; then
    echo -e "  ${GREEN}●${NC} uptime-kuma - running"
else
    echo -e "  ${RED}●${NC} uptime-kuma - stopped"
fi

echo ""
echo "Web UI:"
HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" "http://localhost:3001" 2>/dev/null)
if [ "$HTTP_CODE" = "200" ] || [ "$HTTP_CODE" = "302" ]; then
    echo -e "  ${GREEN}●${NC} http://${PI_IP}:3001 - accessible"
else
    echo -e "  ${RED}●${NC} http://${PI_IP}:3001 - unreachable (HTTP ${HTTP_CODE})"
fi

echo ""
echo "Resources:"
echo "  Memory: $(free -m | awk '/Mem:/ {printf "%dMB / %dMB (%.0f%%)", $3, $2, $3/$2*100}')"
echo "  Disk:   $(df -h /opt/uptime-kuma 2>/dev/null | awk 'NR==2 {printf "%s / %s (%s)", $3, $2, $5}')"
echo "  DB:     $(du -sh /opt/uptime-kuma/data/kuma.db 2>/dev/null | awk '{print $1}') "

echo ""
echo "Dashboard: http://${PI_IP}:3001"
echo ""
