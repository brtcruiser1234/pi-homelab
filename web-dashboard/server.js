const http = require('http');
const fs = require('fs');
const path = require('path');

const PORT = process.env.PORT || 9300;

// ============================================
// Internal service endpoints - UPDATE THESE IPs
// ============================================
const M900_IP = process.env.M900_IP || '127.0.0.1';
const UNRAID_IP = process.env.UNRAID_IP || '10.1.10.193';

const ENDPOINTS = {
    m900: `http://${M900_IP}:9200/stats`,
    unraid: `http://${UNRAID_IP}:9201/stats`,
    pis: {
        'flight-radar': 'http://flight-radar.local:9200/stats',
        'uptime-kuma': 'http://uptime-kuma.local:9200/stats',
        'spare-1': 'http://pi-spare.local:9200/stats',
        'spare-2': 'http://pi-spare.local:9200/stats',
    },
    services: [
        { name: 'Jazz Stats', url: `http://${M900_IP}:8888` },
        { name: 'NHL Tracker', url: `http://${M900_IP}:3050` },
        { name: 'ClaudeCAD', url: `http://${M900_IP}:4000` },
        { name: 'Plex', url: `http://${UNRAID_IP}:32400` },
        { name: 'Sonarr', url: `http://${UNRAID_IP}:8989` },
        { name: 'Radarr', url: `http://${UNRAID_IP}:7878` },
        { name: 'Overseerr', url: `http://${UNRAID_IP}:5055` },
        { name: 'Audiobookshelf', url: `http://${UNRAID_IP}:13378` },
        { name: 'Mammoth Stats', url: `http://${UNRAID_IP}:9999` },
        { name: 'FlightRadar', url: 'http://flight-radar.local:80' },
        { name: 'Uptime Kuma', url: 'http://uptime-kuma.local:3001' },
    ],
};

const MIME = {
    '.html': 'text/html',
    '.css': 'text/css',
    '.js': 'application/javascript',
    '.json': 'application/json',
    '.png': 'image/png',
    '.svg': 'image/svg+xml',
};

// ============================================
// Proxy fetch helper
// ============================================
function proxyFetch(url, timeout = 3000) {
    return new Promise((resolve) => {
        const req = http.get(url, { timeout }, (res) => {
            let data = '';
            res.on('data', chunk => data += chunk);
            res.on('end', () => {
                try { resolve(JSON.parse(data)); }
                catch { resolve(null); }
            });
        });
        req.on('error', () => resolve(null));
        req.on('timeout', () => { req.destroy(); resolve(null); });
    });
}

function httpCheck(url, timeout = 2000) {
    return new Promise((resolve) => {
        const req = http.get(url, { timeout }, (res) => {
            resolve(res.statusCode > 0 && res.statusCode < 500);
            res.resume();
        });
        req.on('error', () => resolve(false));
        req.on('timeout', () => { req.destroy(); resolve(false); });
    });
}

// ============================================
// Server
// ============================================
const server = http.createServer(async (req, res) => {
    const url = req.url;

    // API routes (proxy to internal services)
    if (url === '/api/m900') {
        const data = await proxyFetch(ENDPOINTS.m900);
        res.writeHead(data ? 200 : 502, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify(data || { error: 'unreachable' }));
        return;
    }

    if (url === '/api/unraid') {
        const data = await proxyFetch(ENDPOINTS.unraid);
        res.writeHead(data ? 200 : 502, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify(data || { error: 'unreachable' }));
        return;
    }

    if (url.startsWith('/api/pi/')) {
        const piName = url.replace('/api/pi/', '');
        const endpoint = ENDPOINTS.pis[piName];
        if (!endpoint) {
            res.writeHead(404, { 'Content-Type': 'application/json' });
            res.end(JSON.stringify({ error: 'unknown pi' }));
            return;
        }
        const data = await proxyFetch(endpoint);
        res.writeHead(data ? 200 : 502, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify(data || { error: 'unreachable' }));
        return;
    }

    if (url === '/api/services') {
        const results = await Promise.all(
            ENDPOINTS.services.map(async svc => ({
                name: svc.name,
                up: await httpCheck(svc.url),
            }))
        );
        res.writeHead(200, { 'Content-Type': 'application/json' });
        res.end(JSON.stringify(results));
        return;
    }

    // Static files
    let filePath = url === '/' ? '/index.html' : url;
    filePath = path.join(__dirname, 'public', filePath);

    const ext = path.extname(filePath);
    const contentType = MIME[ext] || 'text/plain';

    fs.readFile(filePath, (err, data) => {
        if (err) {
            res.writeHead(404);
            res.end('Not found');
            return;
        }
        res.writeHead(200, { 'Content-Type': contentType });
        res.end(data);
    });
});

server.listen(PORT, () => {
    console.log(`Rack Status Dashboard running on port ${PORT}`);
    console.log(`  Local:  http://localhost:${PORT}`);
    console.log(`  M900:   ${ENDPOINTS.m900}`);
    console.log(`  Unraid: ${ENDPOINTS.unraid}`);
});
