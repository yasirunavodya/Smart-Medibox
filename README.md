# Smart Medibox: Intelligent Medication Management System

The **Smart Medibox** is designed to assist users in adhering to their medication schedules and ensuring medications are stored under optimal environmental conditions. It combines automated reminders with real-time environmental monitoring to preserve the effectiveness of medications.

##  Key Features

- **Automated Medication Reminders**: Set customizable alarms to remind users to take their medications on time.
- **Environmental Monitoring**: Monitors temperature and humidity levels continuously, notifying users if the environment is unsuitable for medication storage.
- **Light Control**: A motorized curtain regulates light exposure for light-sensitive medications.

##  Technologies and Components

- **OLED Display**: ADAFRUIT SSD 1306 OLED Monochrome Display (128x64) for displaying clear information.
- **Microcontroller**: ESP32 Devkit V1, offering strong performance and connectivity.
- **Sensors**: DHT11 or DHT22 for monitoring temperature and humidity.
- **Actuators**: SG90 Micro Servo Motor to control light exposure.
- **Other Components**: LDRs (Light Dependent Resistors), push buttons, and 10kΩ resistors.

##  Getting Started

### Prerequisites

**Software**:
- Git for repository management
- PlatformIO (with Arduino Framework) for development and deployment
- Node-RED for dashboard creation
- MQTT broker for communication

**Hardware**:
- ESP32 Devkit V1
- ADAFRUIT SSD 1306 OLED Display
- DHT11 or DHT22 sensor
- SG90 Micro Servo Motor
- LDRs
- Push Buttons
- 10kΩ Resistors

### Usage

####  Hardware Setup:
Connect all components as per the provided wiring diagram.

####  Upload Code:
Upload the code to the ESP32 Devkit V1 using PlatformIO.

####  Configure Settings:
- Set medication reminders.
- Define temperature and humidity thresholds.
- Adjust light control preferences.

####  Node-RED Dashboard:
- Import `flows.json` to Node-RED.
- Configure MQTT server parameters.
- Deploy the Node-RED dashboard.

## Software Architecture

The project is modular, with the following components:

- **Hardware Abstraction Layer**: Simplifies communication with hardware.
- **Sensor Management**: Gathers data from DHT sensors and LDRs.
- **Alarm Management**: Handles medication reminders and notifications.
- **Time Management**: Syncs with an NTP server for time accuracy.
- **User Interface**: OLED display interface, navigated through buttons.
- **Communication Management**: Manages MQTT communication for remote data monitoring and control.

## Project Photoes
![Screenshot 2024-09-08 124506](https://github.com/user-attachments/assets/cd31b215-5d3f-40ea-ae20-5e7a148fdb5c)
![Screenshot 2024-09-08 124526](https://github.com/user-attachments/assets/927d3d86-565d-4d57-a944-e5b15c2c991a)
![Screenshot 2024-09-08 124538](https://github.com/user-attachments/assets/31179b5f-a8c4-4ced-bc07-3e690c431fa1)
![Screenshot 2024-09-08 124452](https://github.com/user-attachments/assets/017e52e6-fe5f-499a-82d5-34d8e38880f3)
