#!/bin/bash
# Lightweight JSON stats API for Unraid
# Serves drive temps, array status, and storage info
#
# Usage: Run via Unraid User Scripts plugin or cron
#   This script is called by a simple HTTP server (see unraid-server.py)
#
# Alternative: Run unraid-server.py directly which calls this script

# Drive temperatures (reads from /dev/sd* using smartctl or hdparm)
get_drive_temps() {
    local drives=()
    for dev in /dev/sd?; do
        if [ -b "$dev" ]; then
            local name=$(basename "$dev")
            local temp=$(smartctl -A "$dev" 2>/dev/null | grep -i "temperature" | head -1 | awk '{print $NF}')
            if [ -z "$temp" ]; then
                temp=$(hdparm -H "$dev" 2>/dev/null | grep -i "temperature" | awk '{print $NF}')
            fi
            [ -z "$temp" ] && temp="0"
            local size=$(lsblk -b -d -n -o SIZE "$dev" 2>/dev/null)
            local size_tb=$(echo "scale=2; ${size:-0} / 1099511627776" | bc 2>/dev/null || echo "0")
            local model=$(smartctl -i "$dev" 2>/dev/null | grep "Device Model" | sed 's/.*: *//')
            [ -z "$model" ] && model="unknown"
            drives+=("{\"device\":\"$name\",\"temp_c\":$temp,\"size_tb\":$size_tb,\"model\":\"$model\"}")
        fi
    done

    # Join array with commas
    local result="["
    local first=true
    for d in "${drives[@]}"; do
        if $first; then
            result+="$d"
            first=false
        else
            result+=",$d"
        fi
    done
    result+="]"
    echo "$result"
}

# Array status
get_array_status() {
    local status="unknown"
    if [ -f /var/local/emhttp/var.ini ]; then
        status=$(grep "mdState=" /var/local/emhttp/var.ini 2>/dev/null | cut -d'"' -f2)
    fi
    echo "$status"
}

# Storage usage
get_storage() {
    local total=0
    local used=0

    # Read from disk shares
    while IFS= read -r line; do
        local size=$(echo "$line" | awk '{print $2}')
        local avail=$(echo "$line" | awk '{print $4}')
        total=$((total + size))
        used=$((total - avail))  # Approximation
    done < <(df -BG /mnt/disk* 2>/dev/null | tail -n +2)

    # User share totals
    if [ -d /mnt/user ]; then
        local user_df=$(df -BG /mnt/user 2>/dev/null | tail -1)
        total=$(echo "$user_df" | awk '{gsub("G",""); print $2}')
        used=$(echo "$user_df" | awk '{gsub("G",""); print $3}')
    fi

    echo "{\"total_gb\":${total:-0},\"used_gb\":${used:-0}}"
}

# CPU and memory
get_system() {
    local cpu_percent=$(top -bn1 | grep "Cpu(s)" | awk '{print $2}' 2>/dev/null || echo "0")
    local mem_total=$(free -m | awk '/Mem:/ {print $2}')
    local mem_used=$(free -m | awk '/Mem:/ {print $3}')
    local mem_percent=$(free | awk '/Mem:/ {printf "%.1f", $3/$2 * 100}')
    local uptime_secs=$(awk '{print int($1)}' /proc/uptime)
    local cpu_temp=$(sensors 2>/dev/null | grep -i "core 0" | awk '{print $3}' | tr -d '+°C' || echo "0")

    echo "{\"cpu_percent\":$cpu_percent,\"cpu_temp\":${cpu_temp:-0},\"mem_total_mb\":$mem_total,\"mem_used_mb\":$mem_used,\"mem_percent\":$mem_percent,\"uptime_seconds\":$uptime_secs}"
}

# Docker container count
get_docker() {
    local running=$(docker ps -q 2>/dev/null | wc -l)
    local total=$(docker ps -aq 2>/dev/null | wc -l)
    echo "{\"running\":$running,\"total\":$total}"
}

# Build full JSON response
DRIVES=$(get_drive_temps)
ARRAY=$(get_array_status)
STORAGE=$(get_storage)
SYSTEM=$(get_system)
DOCKER=$(get_docker)

cat << EOF
{
  "hostname": "$(hostname)",
  "array_status": "$ARRAY",
  "drives": $DRIVES,
  "storage": $STORAGE,
  "system": $SYSTEM,
  "docker": $DOCKER,
  "timestamp": $(date +%s)
}
EOF
