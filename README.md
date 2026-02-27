# ESP32 WoL Controller

Remotely wake and monitor a PC/Server via a web interface hosted on the ESP32.

## Features

- Web interface on port 44 (mobile/desktop)
- PC status via ICMP ping, auto-refresh every 10 seconds
- Wake-on-LAN magic packet
- Auto-boot on ESP32 power-up
- Auto-check every 1 minute — sends WoL if PC is offline
- Auto-restart if WiFi fails

## Setup

### ESP32

Configure `src/main.cpp`:

```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char *MACAddress = "AA:BB:CC:DD:EE:FF"; // PC MAC address
IPAddress pc_ip(0, 0, 0, 0);                  // PC static IP
```

Build and upload via PlatformIO. Check Serial Monitor for the ESP32's IP address.

### PC — Find MAC Address

**Windows:** `ipconfig /all` → "Physical Address"

**Linux:** `ip link show`

### PC — Enable WoL

**Windows:**
1. Device Manager → Network Adapter → Properties
2. Power Management: enable "Allow this device to wake the computer"
3. Advanced: set "Wake on Magic Packet" to Enabled
4. Disable Fast Startup (Control Panel → Power Options)

**Linux:**
```bash
sudo apt install ethtool
sudo ethtool -s enp1s0 wol g
```
For persistence, create a systemd service that runs the above command on boot.

### BIOS/UEFI

Enable: Wake on LAN, Power On by PCI/PCIE, Power On by Onboard LAN
Disable: Deep Sleep (if present)

> PC must remain connected to power (PSU on) for WoL to work.

## Usage

1. Power on ESP32 — it connects to WiFi and sends a WoL packet automatically
2. Open `http://<ESP32_IP>:44` in a browser
3. View PC status (ONLINE/OFFLINE) or click **WAKE UP**

## Troubleshooting

- **WoL not working:** Check BIOS settings, disable Windows Fast Startup, verify MAC address in Serial Monitor
- **WiFi fails:** ESP32 restarts after 30s — check SSID/password, use 2.4 GHz only
- **PC not on same subnet:** Some routers block broadcast packets; assign PC a static IP

## Libraries

- [ESP32Ping](https://github.com/marian-craciunescu/ESP32Ping)
- [WakeOnLan](https://github.com/a7md0/WakeOnLan)

## Security

No authentication. Use on trusted local networks only. Credentials are stored in plaintext.

## License

Open source. Free to modify and distribute.
