# SOLE-A — IoT-Based Smart Insole Safety System

SOLE-A is an IoT safety device built into footwear to protect women and children in high-risk situations, without needing any manual action like pressing a button or unlocking a phone. The system uses embedded sensors to continuously monitor body movement and vital signs, detects abnormal patterns such as panic, restraint, or falls, and automatically triggers a GPS-based emergency alert to saved contacts and authorities via GSM/Bluetooth.

This repository contains the three core Arduino/ESP8266 modules that make up the system's sensing and detection layer:

*pulse-oximeter-max30100.ino - Reads real-time heart rate and SpO2 (blood oxygen) using the MAX30100 sensor. Sudden heart rate spikes are one of the physiological signals used to help flag potential distress.

*activity-detection-v1-basic.ino - An early prototype using an MPU6050 accelerometer to classify activity (Walking, Jogging, Running) based on smoothed motion magnitude, tilt angle, and vertical-axis movement.

*activity-detection-v2-fall-detection.ino - An improved version using a simpler moving-average filter on motion magnitude, adding real-time fall detection and a state-confirmation delay to reduce false alarms between Still, Walking, Running, and Fall Detected states.

Together, these modules form the Sensor Layer and part of the Data Processing & Detection Mechanism of the full SOLE-A pipeline (Sensor Layer → Data Processing & Control Unit → Detection Mechanism → Response Protocol), which shares live GPS location with emergency contacts once an abnormal event is confirmed.
