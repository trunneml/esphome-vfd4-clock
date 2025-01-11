![GitHub Actions](https://github.com/trunneml/esphome-vfd4-clock/actions/workflows/pipeline.yaml/badge.svg) ![License: MIT](https://img.shields.io/github/license/trunneml/esphome-vfd4-clock) ![ESPHome Compatible](https://img.shields.io/badge/ESPHome-Compatible-blue?logo=esphome) ![Platform-ESP8266](https://img.shields.io/badge/Platform-ESP8266-brightgreen)

# esphome-vfd4-clock

ESPHome Component and Package for the Douk Audio VFD4 Vintage VFD Clock.

## Introduction

This project centers on reverse-engineering the Douk Audio VFD4 Vintage VFD Clock, which features an ESP8266 microcontroller and a stunning Soviet-era IVL2-7/5 vacuum fluorescent tube (VFD) display. Using [ESPHome](https://esphome.io/), the clock can be integrated effortlessly into a smart home setup via Home Assistant or MQTT.

The clock includes NeoPixel LEDs and a DS1302 real-time clock (RTC). The display is controlled by an HV5812WG-G high-voltage shift register, requiring multiplexing logic for proper operation. After reverse-engineering the display connections and wiring, a custom ESPHome component was created to enable full functionality.

## Features

- **ESP8266-based microcontroller**
- **Two physical buttons** for user interaction
- **IVL2-7/5 VFD tube display** driven by an HV5812WG-G high-voltage shift register
- **NeoPixel LED support** with 68 LEDs:
  - 60 LEDs arranged in an outer ring around the clock PCB
  - 8 LEDs beneath the high-voltage shift register
- **DS1302 Real-Time Clock (RTC)** for accurate timekeeping

### Known Limitation
- **Auto-Brightness Control:** Currently non-functional. If you know how this feature could be implemented, contributions or suggestions are highly encouraged!

## Technical Details

### HV5812WG-G High-Voltage Shift Register
The HV5812WG-G is a 20-channel high-voltage shift register designed for applications that require the control of multiple high-voltage outputs. It operates using a serial data input and can shift data through the register with a clock signal. The key features include:

- **Output Voltage:** Up to 80V per channel, suitable for driving high-voltage VFD tubes.
- **Data Input:** 3.3V or 5V logic-level compatible, allowing direct connection to the ESP8266.
- **Daisy-Chaining Support:** Multiple shift registers can be chained for larger applications.
- **Latch Enable:** Ensures stable output values when data is shifted through the register.

In this clock, the HV5812WG-G is used to drive the IVL2-7/5 tube segments and multiplex its digits. This configuration enables the ESP8266 to control the high-voltage display with minimal GPIO usage.

### IVL2-7/5 VFD Tube Display
The IVL2-7/5 is a Soviet-era vacuum fluorescent display (VFD) tube designed for numeric and symbolic displays. It consists of seven-segment digit characters and several additional symbols. Key characteristics include:

- **Operating Voltage:**
  - Filament (cathode): Typically 1.2–2.0V AC or pulsed DC.
  - Grid and Anode (segment): Requires 20–60V for proper illumination.
- **Multiplexing Capability:** The tube supports multiplexing, reducing the number of driver lines needed.
- **Brightness:** Controlled by the duty cycle of the multiplexing signals.

The IVL2-7/5 tube is connected to the HV5812WG-G shift register, which controls the high-voltage segments and grids. This setup allows the ESP8266 to illuminate specific segments by sending appropriate data through the shift register and synchronizing with the tube’s multiplexing requirements.

## Pin Layout

| ESP GPIO  | Function       | PCB Pin No. |
|-----------|----------------|-------------|
| GPIO-1    | TX / SET       | 3           |
| GPIO-3    | RX / ADD       | 5           |
| GPIO-5    | DS Clock       | -           |
| GPIO-4    | DS IO          | -           |
| GPIO-0    | DS CE          | 4           |
| GPIO-2    | LED Ring       | 6           |
| -         | ESP VCC        | 1           |
| -         | ESP EN         | 2           |
| -         | ESP GND        | 8           |

## Flashing the Firmware

### Wiring

To flash the ESP8266, follow these steps:

1. **Remove the glass cover** and **detach the PCB** from the wooden casing.
2. Connect a serial programmer to the PCB pinouts as follows:
   - Pin 3 (TX) → Programmer RX
   - Pin 5 (RX) → Programmer TX
   - Pin 1 → 3.3V on the programmer
   - Pin 8 (GND) and Pin 4 (GPIO_0) → Programmer GND

### Configuring ESPHome

1. Install ESPHome on your computer.
2. Configure the YAML file for your device. You can refer to the provided `vfdclock_example.yaml` file for guidance.
3. Run ESPHome and flash the firmware using the serial connection.

## Links and Resources

- [ESPHome Documentation](https://esphome.io/)
- [IVL2-7/5 VFD Datasheet](https://loststeak.com/ivl2-75-vfd-datasheets/)
- [HV5812WG-G Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/20005629A.pdf)
- [DS1302 ESPHome Component](https://github.com/trombik/esphome-component-ds1302)
