# WiUS firmware

WiUS: High-Throughput, Low-Power WiFi for Wearable Ultrasound Systems

This repository contains the firmware for the WiUS Wi-Fi PCB, part of the [TinyProbe](https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=9593910) device. The PCB is based on the Silicon Labs SiWG917 which uses the [Gecko](https://docs.silabs.com/gecko-platform/4.4.2/platform-overview/) and [WiseConnect 3](https://docs.silabs.com/wiseconnect/3.2.0/wiseconnect-developing-with-wiseconnect-sdk/) SDKs.

The firmware is split in three parts:
- The **Common** part of the firmware which contains code that is shared between the **WiUS** and **TinyProbe** parts.
- The **WiUS** part of the firmware which provides some easy-to-use modules for the Low-Level API of the SiWG917 SOC.
- The **TinyProbe** part of the firmware which is the actual application that interfaces with the TinyProbe hardware.

The main firmware is located in the [`wius_firmware/`](wius_firmware/) folder.

## Documentation

Documentation can be found in the [`docs/`](docs/) folder. There you can find
- The [Doxygen](docs/doxygen/) part of the documentation, which is able to generate a detailed documentation of the code.
- The [Markdown](docs/markdown/) part of the documentation, which contains a guide on how to get started with development on this platform.

## Building

See the [Building](docs/markdown/siwg917_getting_started/siwg917_getting_started.md) guide for instructions on how to build the firmware.
