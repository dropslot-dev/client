# Dropslot ESP32 Room Controller Client

ESP32-based IoT device for meeting room status indication and control.

## Features

- **HTML Administration Interface**: Web-based configuration through ESP32 access point
- **WiFi Configuration**: Configure WiFi credentials through web interface (no code changes needed)
- **Parameter Management**: Modify room settings, server URL, and other parameters via web UI
- **WiFi Connectivity**: Connects to configured WiFi network
- **Room Status Indication**: LED shows room availability
  - Solid Green: Room is free
  - Pulsing Blue: Meeting upcoming soon
  - Flashing Red: Meeting needs confirmation
  - Slow Pulse Red: Meeting in progress
- **Button Control**: Single button for room actions
  - Free: Quick-book room for 30 minutes
  - Awaiting Confirmation: Confirm meeting
  - In Progress: End meeting early
- **SHA256 Authentication**: Secure API communication with room ID hash
- **Auto-Polling**: Checks room status every 5 seconds

## Hardware Requirements

- ESP32 DevKit V1 (or compatible)
- RGB LED (common cathode)
- Push button
- 3x 220Ω resistors (for LED)
- 10kΩ resistor (for button pull-up, if not using internal)
- Breadboard and jumper wires

## Pin Configuration

Default pin mappings (configurable in `include/config.h`):

- **RED_PIN**: GPIO 25
- **GREEN_PIN**: GPIO 26
- **BLUE_PIN**: GPIO 27
- **BUTTON_PIN**: GPIO 4

## Setup Instructions

### 1. Hardware Setup

Connect components to ESP32:

```
RGB LED:
- Red cathode   → GPIO 25 → 220Ω resistor → GND
- Green cathode → GPIO 26 → 220Ω resistor → GND
- Blue cathode  → GPIO 27 → 220Ω resistor → GND
- Common anode  → 3.3V

Button:
- One side → GPIO 4
- Other side → GND
```

### 2. Software Setup

1. Install [PlatformIO](https://platformio.org/) (VS Code extension recommended)

2. Clone this repository and open the `client/` directory

3. Build and upload the firmware:
   ```bash
   pio run --target upload
   ```

4. **Initial Configuration via Access Point**:
   - After first boot, the ESP32 creates a WiFi access point
   - Connect to the ESP32's access point (network name includes MAC address)
   - Open a web browser and navigate to the configuration page
   - Enter your WiFi credentials, server URL, room ID, and other parameters
   - Save the configuration - the device will reboot and connect to your WiFi

5. **Ongoing Configuration**:
   - Once connected to your WiFi network, access the administration page using the device's IP address
   - All parameters can be modified through the web interface without code changes

6. Monitor serial output for debugging:
   ```bash
   pio device monitor
   ```

## Configuration

### Web-Based Configuration

All configuration is now done through the HTML administration interface:

1. **Initial Setup**: Connect to ESP32's access point and configure basic settings
2. **Ongoing Management**: Access the web interface using the device's IP address
3. **Available Settings**:
   - WiFi SSID and Password
   - Server URL and Room ID
   - Polling interval
   - Authentication settings

### Legacy Configuration (config.h)

Some settings remain in `include/config.h` for advanced users:

- **Pin assignments**: LED and button GPIO pins (hardware-specific, requires recompilation)
- **PWM settings**: LED brightness and frequency
- **DEBOUNCE_DELAY**: Button debounce time
- **HTTP_TIMEOUT**: API request timeout
- **Board identification**: Hardware-specific constants
- **Other operational parameters**: Various timing and behavior constants

## API Integration

### Authentication

When `AUTH_ENABLED` is `true`, all API requests include:
```
Authorization: Bearer <SHA256(ROOM_ID)>
```

### Endpoints Used

- **GET /api/v1/rooms/:roomId/status** - Get current room status
- **POST /api/v1/rooms/:roomId/quick-book** - Quick-book room
  ```json
  {"duration": 30}
  ```
- **POST /api/v1/rooms/:roomId/confirm** - Confirm meeting
- **POST /api/v1/rooms/:roomId/end-meeting** - End meeting early

## Project Structure

```
client/
├── html/
│   ├── index.html           # Main configuration page
│   ├── update.html          # Firmware update page
│   ├── functions.js         # Web interface JavaScript
│   └── style.css            # Web interface styling
├── include/
│   ├── api_client.h         # HTTP API client
│   ├── auth_util.h          # SHA256 authentication
│   ├── button_handler.h     # Button debouncing
│   ├── config.h             # Hardware configuration constants
│   ├── led_controller.h     # LED pattern control
│   └── wifinetwork.h        # WiFi and web server management
├── src/
│   ├── api_client.cpp
│   ├── auth_util.cpp
│   ├── button_handler.cpp
│   ├── led_controller.cpp
│   ├── wifinetwork.cpp      # WiFi and access point management
│   └── main.cpp             # Application entry point
├── platformio.ini           # PlatformIO configuration
└── README.md
```

## Troubleshooting

### Initial Setup Issues
- If you can't find the ESP32's access point, check serial monitor for the network name
- The access point name includes the device's MAC address for uniqueness
- Ensure you're connecting to the correct access point

### WiFi Configuration Issues
- Use the web interface to update WiFi credentials instead of modifying code
- If WiFi connection fails, the device will create an access point for reconfiguration
- Check serial monitor for connection status and error messages
- Verify WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)

### Web Interface Access
- After WiFi configuration, find the device's IP address in your router or serial monitor
- Access the administration page at `http://[device-ip]/`
- If the page doesn't load, check network connectivity and firewall settings

### API Communication Errors
- Use the web interface to verify and update the server URL
- Check that ESP32 and server are on same network (or server is publicly accessible)
- Verify room ID through the web configuration page
- Check server connectivity from the device's network
- Monitor serial output for detailed error messages
- If using authentication, ensure room ID hash matches server expectations

### LED Not Working
- Check pin connections and resistor values
- Verify LED is common cathode (not common anode)
- Test with simple blink sketch to confirm hardware

### Button Not Responding
- Check button wiring and GPIO pin
- Adjust `DEBOUNCE_DELAY_MS` if button is too sensitive/unresponsive
- Monitor serial output to see if button presses are detected

## Development

### Building
```bash
pio run
```

### Uploading
```bash
pio run --target upload
```

### Serial Monitor
```bash
pio device monitor --baud 115200
```

### Clean Build
```bash
pio run --target clean
```
