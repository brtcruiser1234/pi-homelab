#!/bin/bash
# Pi-hole health check
# Usage: ./check-status.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

check_service() {
    local name=$1
    local service=$2
    if systemctl is-active --quiet "$service" 2>/dev/null; then
        echo -e "  ${GREEN}●${NC} $name - running"
    else
        echo -e "  ${RED}●${NC} $name - stopped"
    fi
}

PI_IP=$(hostname -I | awk '{print $1}')

echo ""
echo "=== Pi-hole Status ==="
echo ""

echo "Services:"
check_service "Pi-hole FTL" "pihole-FTL"
check_service "lighttpd" "lighttpd"

echo ""
echo "Blocking:"
pihole status 2>/dev/null | head -5

echo ""
echo "Stats (last 24h):"
# Pull stats from the API
STATS=$(curl -s "http://localhost/admin/api.php?summary" 2>/dev/null)
if [ -n "$STATS" ]; then
    QUERIES=$(echo "$STATS" | jq -r '.dns_queries_today // "N/A"')
    BLOCKED=$(echo "$STATS" | jq -r '.ads_blocked_today // "N/A"')
    PERCENT=$(echo "$STATS" | jq -r '.ads_percentage_today // "N/A"')
    DOMAINS=$(echo "$STATS" | jq -r '.domains_being_blocked // "N/A"')
    echo "  Total queries:    $QUERIES"
    echo "  Ads blocked:      $BLOCKED ($PERCENT%)"
    echo "  Domains on list:  $DOMAINS"
else
    echo "  Could not fetch stats"
fi

echo ""
echo "Web Admin: http://${PI_IP}/admin"
echo ""
