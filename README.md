# Dropslot ESP32 Room Controller Client

ESP32-based IoT device for meeting room status indication and control.

## Features

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

3. Configure settings in `include/config.h`:
   ```cpp
   #define WIFI_SSID "your-wifi-network"
   #define WIFI_PASSWORD "your-wifi-password"
   #define SERVER_URL "http://your-server-ip:8080"
   #define ROOM_ID "201"  // Your room ID
   ```

4. Build and upload:
   ```bash
   pio run --target upload
   ```

5. Monitor serial output:
   ```bash
   pio device monitor
   ```

## Configuration Options

Edit `include/config.h` to customize:

- **WIFI_SSID**: WiFi network name
- **WIFI_PASSWORD**: WiFi password
- **SERVER_URL**: Backend API server URL
- **ROOM_ID**: Unique room identifier
- **POLL_INTERVAL_MS**: Status polling interval (default: 5000ms)
- **AUTH_ENABLED**: Enable/disable SHA256 authentication (default: true)
- **Pin assignments**: LED and button GPIO pins
- **PWM settings**: LED brightness and frequency
- **DEBOUNCE_DELAY_MS**: Button debounce time (default: 50ms)
- **HTTP_TIMEOUT_MS**: API request timeout (default: 10000ms)

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
├── include/
│   ├── api_client.h         # HTTP API client
│   ├── auth_util.h          # SHA256 authentication
│   ├── button_handler.h     # Button debouncing
│   ├── config.h             # Configuration constants
│   └── led_controller.h     # LED pattern control
├── src/
│   ├── api_client.cpp
│   ├── auth_util.cpp
│   ├── button_handler.cpp
│   ├── led_controller.cpp
│   └── main.cpp             # Application entry point
├── platformio.ini           # PlatformIO configuration
└── README.md
```

## Troubleshooting

### WiFi Connection Issues
- Verify SSID and password in `config.h`
- Check that WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
- Monitor serial output for connection status

### API Communication Errors
- Verify `SERVER_URL` is correct and server is running
- Check that ESP32 and server are on same network (or server is publicly accessible)
- Verify `ROOM_ID` exists on server
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

## License

[Add your license here]

## Contributing

[Add contribution guidelines here]
