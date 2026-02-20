# Homelab Inventory

Last updated: 2026-02-20

## M900 (Local Machine)

- **IP:** On local network (Tailscale: 100.74.125.34)
- **OS:** Ubuntu 24.04
- **User:** brandon

| Service | Port | Description |
|---------|------|-------------|
| Jazz-Stats | 8888 | Utah Jazz NBA dashboard |
| NHL Tracker | 3050 | Multi-team NHL stats + WebSocket |
| ClaudeCAD | 4000 | Sketch-based 3D CAD web app |
| Cockpit | 9090 | Server admin panel |
| Blender MCP | 9091 | AI-powered 3D modeling |
| Cloudflared tunnel | - | Exposes services to taylorshome.cc |

**Public URLs (Cloudflare Tunnel):**
- https://jazzlive.taylorshome.cc → localhost:8888
- https://nhlteams.taylorshome.cc → localhost:3050
- https://claudecad.taylorshome.cc → localhost:4000
- https://m900.taylorshome.cc → localhost:9090

---

## Unraid NAS (10.1.10.193)

- **User:** root
- **Access:** SSH key (`~/.ssh/unraid_key`), SCP only (no interactive SSH)

### Running Containers

| Container | Image | Port | Description |
|-----------|-------|------|-------------|
| plex | linuxserver/plex | - | Media server |
| sonarr | linuxserver/sonarr | 8989 | TV show management |
| radarr | linuxserver/radarr | 7878 | Movie management |
| prowlarr | linuxserver/prowlarr | 9696 | Indexer manager for Sonarr/Radarr |
| qbittorrentvpn | binhex/arch-qbittorrentvpn | 8080 | Torrent client with VPN |
| overseerr | binhex/arch-overseerr | 5055 | Media request portal |
| audiobookshelf | advplyr/audiobookshelf | 13378 | Audiobook/podcast server |
| ownfoil | a1ex4/ownfoil | 8465 | Nintendo Switch homebrew shop |
| utah-mammoth-stats | mammoth-stats-img | 9999 | Utah Mammoth hockey stats |
| nhl-proxy | nhl-proxy-image | 5000 | NHL API proxy |
| OctoEverywhere | octoeverywhere | - | 3D printer remote access |
| Cloudflared tunnel | unraid-cloudflared-tunnel | 46495 | Unraid tunnel (old) |
| DiskSpeed | jbartlett777/diskspeed | 18888 | Disk benchmark tool |

### Stopped / Never Started

| Container | Image | Status | Notes |
|-----------|-------|--------|-------|
| nhl-team-tracker | nhl-team-tracker | Exited | Migrated to M900 |
| utah-jazz-stats | jazz-stats-img | Exited | Migrated to M900 |
| mealie | mealie-recipes/mealie | Exited | Crashed ~4 weeks ago. Recipe manager. |
| Jellyfin | jellyfin/jellyfin | Created | Never started. Alt media server. |
| Nextcloud | nextcloud | Created | Never started. Cloud storage. |
| minecraft-server | itzg/minecraft-server | Created | Never started. |
| bazarr | linuxserver/bazarr | Created | Never started. Subtitle manager for Sonarr/Radarr. |
| dolphin | linuxserver/dolphin | Exited | GameCube/Wii emulator. |
| TayloredFilmsTunnel | unraid-cloudflared-tunnel | Exited | Old tunnel, stopped 6 months ago. |
| cloudflared (old) | cloudflare/cloudflared | Exited | Old cloudflared versions. |

---

## Raspberry Pis (Lab Rax 10" Rack)

4x Raspberry Pi 3B (1GB RAM) in 2U blade rack mount.

| # | Hostname | SD Card | Role | Status |
|---|----------|---------|------|--------|
| 1 | flight-radar | 32GB | ADS-B / FlightRadar24 | Setup script ready |
| 2 | uptime-kuma | 32GB | Service monitoring | Setup script ready |
| 3 | pi-spare | 32GB | TBD | Flashing |
| 4 | pi-spare | 128GB | TBD | Flashing |

- **ESP32** → 1U display panel (6x round 1.28" GC9A01 TFTs)

**Credentials:** brandon / Qu4dF4th3r (SSH key auth primary)

---

## 3D Printer

- **Model:** Bambu Lab
- **Remote Access:** OctoEverywhere (running on Unraid)

---

## Network

- **Router/Gateway:** 10.1.10.1 (assumed)
- **Cloudflare Domain:** taylorshome.cc
- **Cloudflare Tunnel (M900):** m900-ssh (4afbda9b-6542-4b5b-8e49-128ec7c20ed7)
- **Cloudflare Tunnel (Unraid):** Still running (old, consider cleanup)

---

## Cleanup Candidates

- [ ] Remove stopped containers on Unraid that were migrated (jazz-stats, nhl-team-tracker)
- [ ] Remove old Cloudflare tunnels (TayloredFilmsTunnel, old cloudflared)
- [ ] Decide on mealie - revive or remove?
- [ ] Decide on Jellyfin, Nextcloud, minecraft, bazarr, dolphin - use or remove?
- [ ] Migrate mammoth-stats + nhl-proxy to M900?
