# Pi-hole - DNS Ad Blocker

Network-wide ad blocking via DNS. Blocks ads, trackers, and malware domains for every device on your network.

## Hardware

- Raspberry Pi 3B (32GB SD card)
- Ethernet connection (recommended for DNS server)

## Quick Start

1. Flash Pi OS Lite, SSH in
2. Run the shared first-boot script: `sudo ./first-boot.sh`
3. Run the Pi-hole setup: `sudo ./setup.sh`
4. **Point your router's DNS to the Pi's IP**

## What Gets Installed

| Component | Purpose |
|-----------|---------|
| Pi-hole | DNS sinkhole + web admin |
| lighttpd | Web server for admin panel |
| SQLite | Query logging database |

## Blocklists

Pre-configured with community-recommended lists:
- **Pi-hole default** - curated by the Pi-hole team
- **StevenBlack Unified** - ads + malware + fakenews
- **HaGeZi Multi Normal** - comprehensive multi-source
- **OISD Big** - curated general-purpose list

## Web Admin

Access at `http://pihole.local/admin`

- View query logs and statistics
- Whitelist/blacklist domains
- See which devices are making queries
- Disable blocking temporarily

## Router Setup

To block ads network-wide, set your router's DNS to the Pi-hole's IP:
1. Log into your router admin
2. Find DHCP/DNS settings
3. Set primary DNS to Pi-hole's IP
4. Optional: set secondary DNS to `1.1.1.1` (fallback if Pi is down)

## Local DNS

Custom local DNS entries are in `/etc/pihole/custom.list`. Use this to access your homelab services by name instead of IP.

## Useful Commands

```bash
pihole status           # Check if blocking is active
pihole -c               # Real-time console stats
pihole -g               # Update blocklists
pihole -a -p            # Change admin password
pihole disable 5m       # Disable blocking for 5 minutes
pihole enable           # Re-enable blocking
pihole -t               # Tail the live query log
```
