# Smart Factory Monitoring System

An end-to-end, IoT-based industrial monitoring and control system designed to provide real-time visibility, automated alerting, and remote equipment control across factory environments.

Built using **Arduino Uno**, **W5100 Ethernet Shield**, **MQTT Protocol**, **PICSimLab Simulator**, and **ThingsBoard IoT Cloud / Community Edition (CE)** platform.

---

## 📌 Project Overview

Traditional manufacturing environments often suffer from a lack of real-time floor visibility, leading to delayed incident responses, unexpected machine downtime, and high operational costs.

The **Smart Factory Monitoring System** addresses these issues through a distributed, multi-node architecture that continuously gathers environmental and mechanical telemetry, triggers automated alerts upon threshold breaches, and enables bidirectional remote control via Remote Procedure Calls (RPC). 

The platform supports both physical hardware deployments and virtual Hardware-in-the-Loop (HIL) testing via **PICSimLab**, connected seamlessly to **ThingsBoard IoT Cloud** for centralized control.

---

## 🏗️ Architecture & System Topology

The system uses a **3-Tier / 4-Layer IoT Architecture**:

1. **Edge / Sensor Layer**: Distributed Arduino Uno nodes embedded with sensors (DHT22, LM35, PIR, LDR, Potentiometers) and actuators (Relays, Status LEDs). Supports virtual HIL simulation via **PICSimLab**.
2. **Transport Layer**: MQTT v3.1.1 over Ethernet (W5100 Shield, TCP/IP stack) with TLS/Token security.
3. **Application / Processing Layer**: **ThingsBoard IoT Cloud / CE** (Rule Engine, PostgreSQL Time-Series Store, Alarm Manager).
4. **Presentation Layer**: Interactive Web Dashboard on ThingsBoard with live gauges, trend charts, alarm notifications, and RPC relay controls.

```text
+-----------------------------------------------------------------------+
|                             EDGE LAYER                                |
|  [ Physical Arduino Uno Nodes ]  OR  [ Virtual PICSimLab Simulation ] |
|  +---------------------------+       +-----------------------------+  |
|  |    Node 1: Production     |       |      Node 2: Warehouse      |  |
|  |  - IP: 192.168.1.101      |       |  - IP: 192.168.1.102        |  |
|  |  - DHT22, LM35, Pot (Vib) |       |  - DHT22, LDR, PIR Motion   |  |
|  |  - Relays, Status LEDs    |       |  - Status Indicators        |  |
|  +-------------+-------------+       +--------------+--------------+  |
+----------------|------------------------------------|-----------------+
                 | SPI                                | SPI
                 +-----------------+------------------+
                                   |
                                   v
+-----------------------------------------------------------------------+
|                    TRANSPORT & BROKER LAYER                           |
|  Ethernet W5100 Shield (TCP/IP Interface / Virtual Tap Bridge)        |
|  MQTT v3.1.1 | Port 1883/8883 (QoS 1, JSON Telemetry every 5s)      |
|                                  |                                    |
|                                  v                                    |
|  ThingsBoard IoT Cloud / CE                                           |
|  (Rule Engine + PostgreSQL DB + Alarm Manager + Device Registry)      |
+----------------------------------+------------------------------------+
                                   | WebSocket / REST API
                                   v
+-----------------------------------------------------------------------+
|                         PRESENTATION LAYER                            |
|  ThingsBoard Web Dashboard: Live Charts | Gauges | Alarms | RPC Control|
+-----------------------------------------------------------------------+
```

---

## 📂 Repository Structure

```text
Smart-Factory-Monitoring-System/
├── Production Line node1/
│   ├── node1/
│   │   ├── node1.ino           # Main Arduino firmware entry point for Node 1
│   │   ├── config.h            # Pin configurations & network settings
│   │   ├── SensorManager.h     # Sensor abstraction layer (DHT22, LM35, Vib)
│   │   ├── network.h / .cpp    # Ethernet & MQTT network management
│   │   ├── telemetry.h / .cpp  # JSON telemetry construction & transmission
│   │   ├── actuator.h / .cpp   # Relay & status LED driver logic
│   │   └── rpc.h / .cpp        # Remote Procedure Call (RPC) command handlers
│   └── node2/                  # Node 2 firmware files (Warehouse node) as similar to node 2 slight changes
└── README.md                   # Project Documentation
```

---

## 📟 Node Deployment Summary

| Feature / Node | Node 1: Production Line Monitor | Node 2: Warehouse Monitor |
| :--- | :--- | :--- |
| **IP Address** | `192.168.1.101` | `192.168.1.102` |
| **Microcontroller** | Arduino Uno (ATmega328P) / PICSimLab | Arduino Uno (ATmega328P) / PICSimLab |
| **Network Shield** | W5100 Ethernet Shield (SPI) | W5100 Ethernet Shield (SPI) |
| **Sensors** | DHT22 (Temp/Hum), LM35 (Temp), Potentiometer (Vibration) | DHT22 (Temp/Hum), LDR (Light), PIR HC-SR501 (Motion) |
| **Actuators / Indicators** | 1-Channel Relay, Green / Yellow / Red LEDs, Push Button | Status LEDs |
| **Function** | Machine condition & thermal/vibration monitoring | Environmental & security surveillance |

---

## ☁️ ThingsBoard IoT Cloud Integration

**ThingsBoard IoT Cloud** serves as the central IoT server platform hosting the application logic, time-series storage, rule engine, and visualization UI:

* **Telemetry Storage**: Stores historical sensor values in a time-series PostgreSQL database for trend visualization and predictive maintenance analysis.
* **Rule Engine Processing**: Dynamic condition monitoring and real-time alarm routing.
* **Bidirectional Control (RPC)**: Allows operators to trigger manual overrides on physical or simulated relays directly from dashboard widgets via MQTT RPC methods.
* **Device Access Management**: Authenticates nodes using 256-bit unique Device Access Tokens over MQTT.

---

## 🚨 Automated Alert Rules

The system implements automated rule logic inside the **ThingsBoard Rule Engine**:

* **Critical Temperature (`> 35°C`)**:
  * Triggers **CRITICAL Alarm** on Dashboard.
  * Sends email and Telegram notifications to operators.
  * Sends RPC/actuator event turning **Red LED ON** at Node 1.
* **Excessive Vibration (`> 700 ADC`)**:
  * Triggers **WARNING Alarm**.
  * Illuminates **Yellow LED** indicator.
  * Logs event into time-series database.
* **High Humidity (`> 80%`)**:
  * Triggers **WARNING Alarm**.
  * Displays dashboard alert and fires email notification.

---

## 📡 Communication Protocol Specifications

* **Protocol**: MQTT v3.1.1 over Ethernet TCP/IP
* **Publish Frequency**: Every 5 seconds per node
* **Quality of Service (QoS)**: QoS 1 (At least once delivery)
* **Keep-Alive**: 60 seconds
* **Payload Size**: ~150 bytes per JSON telemetry packet
* **Throughput**: ~60 bytes/sec total (2 nodes)
* **Reconnection**: Exponential backoff strategy (5s to 60s)
* **Authentication**: 256-bit Device Access Tokens

---

## 💻 Hardware-In-The-Loop Simulation Setup (PICSimLab)

**PICSimLab (Programmable IC Simulator Laboratory)** allows complete firmware testing without physical hardware components:

1. Launch **PICSimLab** and select the **Arduino Uno** board configuration.
2. Connect simulated peripheral modules:
   * **DHT22** (Temperature & Humidity)
   * **LM35** (Analog Temperature)
   * **PIR HC-SR501** & **LDR**
   * **W5100 Ethernet Module**
3. Configure the virtual network interface (TAP/Bridge) to connect the simulator to the host network and grant internet/cloud routing.
4. Compile the sketch in Arduino IDE, obtain the compiled `.hex` binary, and load it into PICSimLab.
5. Verify live telemetry streams updating on **ThingsBoard IoT Cloud**.

---

## 🛠️ Build & Flash Instructions

1. **Prerequisites**:
   * Arduino IDE (v1.8.x or v2.x)
   * Required Libraries: `Ethernet`, `PubSubClient`, `ArduinoJson`, `DHT sensor library`
2. Configure network parameters and access tokens in `config.h`:
   ```cpp
   #define MQTT_SERVER "thingsboard.cloud" // or local ThingsBoard IP
   #define TOKEN "YOUR_DEVICE_ACCESS_TOKEN"
   ```
3. **Physical Hardware Setup**: Connect Arduino Uno via USB, select correct COM port, compile, and upload `node1.ino` or `node2.ino`.
4. **Virtual Setup**: Export compiled binary (`.hex`) from Arduino IDE and load into **PICSimLab**.
