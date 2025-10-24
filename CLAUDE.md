# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is the LilyGo Modem Series repository, containing examples and libraries for various LilyGo cellular modem development boards. The project supports multiple ESP32/ESP32-S3 boards with different cellular modems including A7670, A7608, SIM7000G, SIM7070G, SIM7080G, SIM7600, and SIM7670G.

## Development Commands

### Build and Upload
```bash
# Build project for specific board
pio run -e [BOARD_NAME]

# Upload to device
pio run -t upload -e [BOARD_NAME]

# Monitor serial output
pio device monitor --port [PORT] --baud 115200

# Clean build files
pio run -t clean
```

### Common Board Names (default_envs in platformio.ini)
- `T-Call-A7670X-V1-1` - T-Call A7670X Version 1.1
- `T-A7670X` - T-A7670X ESP32 version
- `T-SIM7670G` - T-SIM7670G ESP32-S3 version
- `T-A7608X` - T-A7608X ESP32 version
- `T-SIM7000G` - T-SIM7000G ESP32 version
- `T-SIM7600X` - T-SIM7600X ESP32 version
- Standard series with `-S3-Standard` suffix

## Architecture Overview

### Board Configuration System
The project uses a sophisticated board configuration system centered around `examples/ATdebug/utilities.h`:

- **Board Selection**: Controlled by `#define` macros (e.g., `LILYGO_T_CALL_A7670_V1_1`)
- **Pin Definitions**: Each board variant has specific GPIO pin assignments for modem communication, power control, LED, etc.
- **Modem Types**: Automatically configures TinyGSM modem type based on board selection
- **Hardware Features**: Defines available features like GPS, battery monitoring, SD card, etc.

### Project Structure
- **`platformio.ini`**: Main configuration file with board definitions and build settings
- **`examples/`**: 60+ example applications demonstrating different modem capabilities
- **`lib/`**: Contains modified TinyGSM library and other dependencies
- **`docs/`**: Board-specific documentation and quick start guides
- **`src/`**: Custom source code (when not using examples)

### Example Selection System
The build system uses `src_dir` in platformio.ini to select which example to compile:
```ini
src_dir = examples/ATdebug  # Select specific example
; src_dir = examples/GPS_BuiltIn  # Comment out others
```

### Power Management
Critical for cellular modems - most boards require external 5V 2A power supply for reliable operation. USB power from computers is typically insufficient for modem startup and operation.

### TinyGSM Integration
Uses a forked version of TinyGSM library specifically modified to support LilyGo's cellular modules. The fork includes support for:
- A7670/A7608 series modems
- SIM7672G/SIM7670G modems
- Additional AT commands and functionality

### Modem Communication
- **Serial Interface**: Uses Hardware Serial (Serial1) for AT command communication
- **Default Baud Rate**: 115200 (configurable per board)
- **Power Sequence**: Specific startup/shutdown sequences required for each modem type
- **Pin Control**: DTR, PWRKEY, RESET pins for modem control

### Board Variants
The project supports multiple hardware generations:
- **ESP32 variants**: Original boards with ESP32-WROVER modules
- **ESP32-S3 variants**: Newer boards with ESP32-S3 modules and enhanced features
- **Standard series**: Latest generation with QWIIC connectors, seamless power switching, improved power efficiency

## Development Notes

### Board Selection Process
1. Identify your exact board model (check silkscreen markings)
2. Uncomment the corresponding `#define` in utilities.h OR set `default_envs` in platformio.ini
3. Select appropriate example with `src_dir` setting

### Common Issues
- **Power Supply**: Ensure adequate power (5V 2A) for modem operation
- **SIM Card**: Disable PIN code protection before use
- **Antenna**: Connect appropriate antenna for cellular/GPS functionality
- **Board Version**: T-Call A7670X has two versions (V1.0 vs V1.1) with different pin assignments

### Testing Workflow
1. Start with `ATdebug` example to verify basic modem communication
2. Use `GPS_BuiltIn` for GPS functionality testing
3. Progress to specific feature examples (HTTP, MQTT, SMS, etc.)

### Pin Configuration
Each board variant has unique pin assignments defined in utilities.h. Key pins include:
- `MODEM_TX_PIN`/`MODEM_RX_PIN` - Serial communication
- `BOARD_PWRKEY_PIN` - Modem power control
- `MODEM_DTR_PIN` - Sleep/wake control
- `BOARD_LED_PIN` - Status indication
- Board-specific pins for reset, power enable, etc.

The utilities.h file contains comprehensive pin definitions for all supported board variants.