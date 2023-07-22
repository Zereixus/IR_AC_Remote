# ESP32 Wi-Fi Master-Slave Communication with Web Control

## Overview

Welcome to the ESP32 Wi-Fi Master-Slave Communication project! This repository showcases a robust and flexible communication system utilizing ESP32 microcontrollers to establish seamless communication between a Master and multiple Slave devices over a Wi-Fi network. The Master device acts as a web server, allowing users to control GPIO pins on the Slave devices through an intuitive web interface.

## Project Highlights

1. **Master-Slave Architecture:** The project employs the popular Master-Slave communication paradigm to enable efficient data exchange. The Master device can interact with multiple Slave devices, facilitating scalable and dynamic control capabilities.

2. **ESP-NOW Communication Protocol:** To ensure low-latency and reliable communication, the ESP-NOW protocol is utilized. ESP-NOW is well-suited for one-way data transmission, making it ideal for sending commands from the Master to Slaves.

3. **Web-based Control Interface:** The intuitive web-based interface offers a user-friendly way to interact with the connected Slave devices. Users can remotely toggle GPIO pins and control various peripherals on the Slaves with ease.

4. **Wi-Fi Network Resilience:** The project incorporates intelligent Wi-Fi network management. In case the initial Wi-Fi connection attempt fails, the Master device gracefully switches to Access Point (AP) mode, providing a fallback option to ensure continued functionality.

5. **Dynamic Reconnection:** In Access Point mode, the Master device periodically attempts to reconnect to the specified Wi-Fi network. This dynamic reconnection mechanism enables seamless transitions between Wi-Fi modes.

## Repository Contents

1. **Main Sketch:** The main sketch file ("Master_Slave_Communication.ino") contains the code for the ESP32 Master device. It initializes the communication, sets up the web server, and handles GPIO control requests from the web interface.

2. **Slave Communication:** The repository includes the code for configuring the ESP-NOW communication protocol on the Slave devices. By providing the Slave device's MAC address, communication with multiple Slaves can be easily established.

3. **Web Interface:** The "PageIndex.h" file contains the HTML code for the user interface served by the Master device. This interface allows users to control the Slave devices through a modern and responsive web layout.

## Getting Started

To get started with the project, follow these steps:

1. Clone the repository to your local machine using the provided GitHub link.
2. Upload the sketch to your ESP32 Master device using the Arduino IDE or your preferred programming tool.
3. Configure the Slave devices by loading the appropriate code and specifying their MAC addresses in the Master sketch.
4. Power on the devices and ensure that the Master is connected to a Wi-Fi network or switches to Access Point mode if necessary.
5. Access the Master's IP address (either from the Wi-Fi network or the Access Point) in your web browser to access the control interface.
6. Start controlling the GPIO pins on the connected Slave devices through the web interface.

## Contributions and Support

We welcome contributions to the project! If you encounter any issues or have ideas for improvements, feel free to open an issue or submit a pull request. Let's make this project even better together!

## License

[Include your chosen license information here.]
