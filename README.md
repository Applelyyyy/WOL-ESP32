# ESP32 Power Controller (WoL)

A DIY IoT solution to remotely wake and monitor your PC/Server using an ESP32. This project allows you to **Wake** the PC using Wake-on-LAN (WoL) through a responsive web interface hosted on the ESP32.

## 🚀 Features

* **Responsive Web Interface:** Hosted directly on the ESP32 (Port 44) - works on desktop and mobile
* **Real-time Status Monitoring:** Auto-refreshes every 10 seconds to check if the PC is Online/Offline using ICMP Ping
* **Wake-on-LAN (WoL):** Turns on the PC from a shutdown state
* **Auto-Boot:** Automatically sends WoL packet when ESP32 powers on or connects to WiFi
* **Visual Countdown:** 60-second countdown timer with progress bar after sending wake signal
* **WiFi Auto-Reconnect:** Automatically restarts and reconnects if WiFi connection fails

## 🛠️ Prerequisites

### Hardware
* ESP32 Development Board (e.g., ESP32 DevKit V1)
* PC/Server (tested on various hardware)
* Ethernet connection for the PC (Recommended for reliable WoL)

### Software
* **VS Code** with **PlatformIO** extension (for ESP32 development)

---

## 📦 1. ESP32 Setup

1.  Open the project in **VS Code with PlatformIO**.
2.  The required libraries are already configured in `platformio.ini`:
    ```ini
    lib_deps =
        marian-craciunescu/ESP32Ping @ ^1.7
        a7md0/WakeOnLan @ ^1.1.7
    ```
3.  Configure your network credentials in `src/main.cpp`:
    ```cpp
    const char* ssid     = "YOUR_WIFI_SSID";      // Change this
    const char* password = "YOUR_WIFI_PASSWORD";  // Change this
    const char *MACAddress = "AA:BB:CC:DD:EE:FF"; // PC's MAC Address
    IPAddress pc_ip(0, 0, 0, 0);             // PC's Static IP
    const int serverPort = 44;                    // ESP32 Web Server Port
    ```
4.  Build and upload the code to your ESP32 using PlatformIO.
5.  Open Serial Monitor to see:
    - WiFi connection status
    - ESP32 IP address
    - Web server URL
    - WoL packet transmission logs

---

## 🖥️ 2. PC Setup (Windows/Linux)

### Find Your PC's MAC Address

**Windows:**
```cmd
ipconfig /all
```
Look for "Physical Address" of your Ethernet adapter.

**Linux:**
```bash
ip link show
# or
ifconfig
```
Look for the MAC address (HWaddr) of your network interface (e.g., eth0, enp1s0).

### Enable Wake-on-LAN

#### Windows
1. Open **Device Manager**
2. Find your **Network Adapter** → Right-click → **Properties**
3. Go to **Power Management** tab:
   - ✅ Enable "Allow this device to wake the computer"
   - ✅ Enable "Only allow a magic packet to wake the computer"
4. Go to **Advanced** tab:
   - Find "Wake on Magic Packet" → Set to **Enabled**
5. Disable **Fast Startup**:
   - Control Panel → Power Options → Choose what the power buttons do
   - Change settings that are currently unavailable
   - ❌ Uncheck "Turn on fast startup"

#### Linux (Debian/Ubuntu)
1.  Install ethtool:
    ```bash
    sudo apt-get install ethtool
    ```
2.  Check current WoL status (replace `enp1s0` with your interface):
    ```bash
    sudo ethtool enp1s0 | grep Wake-on
    ```
3.  Enable WoL temporarily:
    ```bash
    sudo ethtool -s enp1s0 wol g
    ```
4.  Make it permanent with a systemd service:
    ```bash
    sudo nano /etc/systemd/system/wol.service
    ```
    Add:
    ```ini
    [Unit]
    Description=Enable Wake-on-LAN
    After=network.target

    [Service]
    Type=oneshot
    ExecStart=/sbin/ethtool -s enp1s0 wol g

    [Install]
    WantedBy=multi-user.target
    ```
    Enable it:
    ```bash
    sudo systemctl enable wol.service
    sudo systemctl start wol.service
    ```

---

## ⚙️ BIOS/UEFI Settings (Critical!)

For WoL to work, configure your PC's BIOS/UEFI:

1. Enter BIOS during boot
2. Find these settings (names vary by manufacturer):
   - **Wake on LAN:** `Enabled`
   - **Power On by PCI/PCIE:** `Enabled`
   - **Power On by Onboard LAN:** `Enabled`
   - **Deep Sleep Control:** `Disabled` (if present)
3. Save and exit

**Note:** The PC must be connected to power (PSU switched on) for WoL to work!

---

## 📱 Usage

1.  **Power up the ESP32** - it will automatically connect to WiFi and send a WoL packet
2.  **Find the ESP32's IP address:**
    - Check your router's DHCP list, or
    - View the Serial Monitor output
3.  **Access the web interface:**
    - Open browser: `http://<ESP32_IP>:44`
    - Example: `http://192.168.1.100:44`
4.  **Monitor Status:**
    - Shows **ONLINE** (Green) or **OFFLINE** (Red)
    - Auto-refreshes every 10 seconds
5.  **Wake the PC:**
    - Click **"WAKE UP"** button
    - View 60-second countdown timer
    - Automatically returns to main page

---

## 🔍 Troubleshooting

### WoL Not Working?

1. **Check Serial Monitor output:**
   - Verify MAC address is correct
   - Check broadcast IP is calculated correctly
   - Ensure WoL packets are being sent

2. **Network Issues:**
   - PC must be on the same network as ESP32
   - Some routers block broadcast packets - try assigning PC a static IP
   - Firewall might be blocking WoL packets

3. **PC Configuration:**
   - Double-check BIOS settings
   - Verify network adapter WoL settings
   - Test WoL from another device first (using WoL tools)
   - PC must be connected to power

4. **Windows Fast Startup:**
   - This must be disabled for WoL to work properly

### WiFi Connection Issues:

- ESP32 auto-restarts after 30 seconds if WiFi fails
- Check SSID and password in code
- Ensure 2.4GHz WiFi (ESP32 doesn't support 5GHz)

---


## 📝 Technical Details

**Libraries Used:**
- `ESP32Ping` - ICMP ping for status checking
- `WakeOnLan` - Magic packet generation and transmission
- `WiFi` & `WebServer` - Built-in ESP32 libraries

**Network Protocol:**
- Wake-on-LAN uses UDP port 9
- Magic packet contains FF FF FF FF FF FF followed by target MAC repeated 16 times
- ESP32 sends packet 3 times with 100ms delay for reliability

---

## 🛡️ Security Notes

- This web interface has no authentication
- Only use on trusted local networks
- Consider adding password protection for production use
- WiFi credentials are stored in plaintext in the code

---

## 📄 License

This project is open source. Feel free to modify and distribute.

---

## 🤝 Contributing

Improvements and suggestions are welcome!

---

## 📧 Support

If you encounter issues:
1. Check the Serial Monitor for debug output
2. Verify all BIOS and OS settings
3. Test WoL with another tool first
4. Ensure PC and ESP32 are on the same network

---

**Made with ❤️ using ESP32**
