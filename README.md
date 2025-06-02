# Robotic Arm Project

> Final project for Industrial Automation and Robotics - Module B  
> **University of Messina - Department of Engineering**  
> Academic Year 2024/2025  
> Authors: Giovanni Lombardo, Salvatore Marchese, Manuel Fabiano

## 🧠 Project Overview

This project implements a miniature Industry 4.0-inspired material-handling cell that integrates:

- A 3-DOF Fischertechnik robotic arm
- A conveyor belt
- An Arduino-based rail-guided cart
- A PLC Siemens S7-1200 for orchestration
- CoppeliaSim simulation environment

The cell performs automated pick-and-place operations controlled by a PLC, with real-time communication to a Wi-Fi-controlled Arduino cart and synchronized simulation in CoppeliaSim.

---

## 🛠️ Technologies

### Hardware

- **PLC:** Siemens SIMATIC S7-1200 (1215C AC/DC/RLY)
- **Robot Arm:** Fischertechnik 3D-Robot 24V with encoders and limit switches
- **Conveyor Belt:** DC motor controlled via PLC + photoelectric sensor (E3F-DS10C4)
- **Cart:** Arduino UNO WiFi + 2 servo motors + 2 ultrasonic sensors
- **Bridge Server:** Python-based TCP relay running on a laptop

### Software

- **TIA Portal:** PLC programming (Ladder Logic, FSM)
- **Arduino IDE:** Cart firmware development
- **Python:** TCP relay server
- **CoppeliaSim 4.9:** Simulation with Lua scripting and inverse kinematics

---

## ⚙️ Functional Description
![Demo of the real system](media/real.gif)

### Control Flow

1. Object detected on conveyor
2. PLC halts belt and sends `start` via TCP
3. Cart moves under robot and waits
4. Robot picks object, places it on cart
5. Cart moves to delivery point
6. (Only in simulation) Second robot places object on table

---

## 🧩 Architecture

```plaintext
[PLC] ←Ethernet→ [Laptop Bridge Server] ←WiFi→ [Arduino Cart]
  ↕                 
[Conveyor + Robot Arm] 
```

## 📁 Repository Structure

```plaintext
.
├── Cart                # Arduino firmware
    ├── Cart.ino
├── main.py                  # TCP bridge Python server
├── Coppelia/                # CoppeliaSim scenes and scripts
    ├── project.ttt
├── docs/                    # Project report, schematics, etc.
├── tiaportal                # TIA Portal project
└── README.md                # You're here
```
---

## 🔧 Installation

### 1. TIA Portal (Ladder Program)
- Open the `.ap16` project
- Flash to S7-1200
- Ensure I/O matches wiring diagram
- Ensure IP address of python server is correctly set in TIA Portal

### 2. Arduino Cart
- Flash `Cart.ino` to Arduino UNO WiFi
- Adjust SSID, password, and server IP
- Power via external source

### 3. Laptop Relay (Python)
```bash
python main.py
```

Make sure ports `2001` and `5001` are open.

### 4. CoppeliaSim
![Demo of the simulation](media/coppelia.gif)
- Load `.ttt` scene
- Run the simulation to see the full cell operation
- Scripts handle full robot behavior with inverse kinematics

---



---

## 📜 License

This project is developed for academic purposes. Feel free to use or modify it for educational use.  
For commercial use, please contact the authors.
