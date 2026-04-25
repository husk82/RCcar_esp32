# RC Car ESP32 Gateway

ESP32 firmware acting as a communication bridge between the Flutter app and STM32 controller.

## System Overview
Flutter App → ESP32 → STM32 → Motor Driver

## Responsibilities
The ESP32 handles:
- WiFi Access Point
- WebSocket server
- Command forwarding via UART

## Features
- WiFi AP mode (`RC_CAR`)
- WebSocket server for real-time control
- UART communication with STM32

## Tech Stack
- ESP-IDF (C)
- FreeRTOS (tasks)
- WiFi (TCP/WebSocket)