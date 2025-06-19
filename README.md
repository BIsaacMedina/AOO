# ESP32 PC Auto-On Scheduler

This project uses an ESP32 board to automatically power on and power off a desktop PC at a scheduled time each day, using GPIO pin control and NTP-synced time over Wi-Fi. It includes a simple HTTP API for remote schedule configuration and system state monitoring, and is designed for future integration with a Pixoo64 display and Homepage dashboard.

---

## Preview

> Insert images of your setup here!
>
> &#x20;&#x20;

---

## Features

- NTP Time Sync: Automatically syncs time over Wi-Fi using NTP (Pacific Time).
- Power On\Off PC: Controls a pin connected to the PC's motherboard.
- State Detection: Detects PC power state via GPIO input.
- HTTP API:
  - `GET /api/status` returns `{ "state": "ON" }` or `"OFF"`
  - `GET /api/set_shutdown?hour=HH&minute=MM` updates the shutdown schedule
- Extensible: Scaffolding in place for:
  - Pixoo64 display output
  - Homepage dashboard integration
  - Log storage via LittleFS or streaming via WebSocket

---

## Hardware Used

| Component        | Purpose                                               |
| ---------------- | ----------------------------------------------------- |
| ESP32 DevKit V1  | Main controller                                       |
| Transistor/Relay | Triggers PC power switch                              |
| GPIO Input Wire  | Reads PC power state (e.g., from power LED or USB 5V) |
| LED              | Heartbeat / status indicator                          |

---

## Wiring Diagram

> Insert your wiring diagram here (Fritzing recommended)
>
>

---

## Setup Instructions

### 1. Flash Firmware

1. Clone the repo and open it in VS Code with PlatformIO.

2. Connect your ESP32 via USB.

3. Edit the Wi-Fi credentials in the `setup()` function:

   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```

4. Upload the code:

   ```bash
   pio run --target upload
   ```

5. Open the serial monitor (optional) to watch boot output:

   ```bash
   pio device monitor
   ```

---

### 2. Verify Operation

Once booted:

- The onboard LED blinks every second.
- The ESP32 syncs time via NTP.
- At 6:00:00 AM Pacific Time, it will pulse the PC power pin to switch the machine on (if off).
- At 10:00:00 PM Pacific Time, it will pulse the PC power pin to switch the machine off (if on).
- The ESP32 also detects and records the PC's current state using the `inPin` which takes input from the motherboard pin that powers the light on the power button.

---

### 3. Access the API

#### Get PC State

```bash
curl http://<esp32-ip>/api/status
```

Returns:

```json
{"state": "ON"}
```

#### Set Shutdown Time

```bash
curl "http://<esp32-ip>/api/set_shutdown?hour=22&minute=0"
```

Sets internal shutdown schedule for future use.

---

## API Reference

| Endpoint                              | Method | Description                    |
| ------------------------------------- | ------ | ------------------------------ |
| `/api/status`                         | GET    | Returns current PC state       |
| `/api/set_shutdown?hour=HH&minute=MM` | GET    | Updates shutdown time schedule |

---

## Roadmap

-

---

## Why I Built This

I wanted a fully custom and flexible way to automatically power my PC each day without relying on BIOS wake timers or Wake-on-LAN — something smarter, more reliable, more power efficeint, and extensible for my smart office setup.

---

## License

MIT License — use it freely, modify it boldly.

---

Made with care by [Benjamin Isaac Medina](benstechsolutions.net)

