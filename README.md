# CryoSkills Data - Receiver (Firmware)
Arduino firmware for the CryoSkills datalogger receiver.

## Requirements
The project requires the following project and external libraries to be installed in the Arduino IDE or PlatformIO before compiling.

To install each library the Arduino IDE, download a `*.zip` archive of the library and install using the `Sketch > Include Library > Add .ZIP Library` menu option.

| Library Name     | Description                                       | URL                                                |
| ---------------- | ------------------------------------------------- | -------------------------------------------------- |
| CryoSkills       | Auxiliary libraries for the CryoSkills datalogger | https://github.com/cryoskills/sensor-kit-libraries |
| INA3221          | CryoSkills modification of INA3221 library        | https://github.com/cryoskills/INA3221              |
| ZeroPowerManager | Required to compile CryoSkills library            | https://github.com/ee-quipment/ZeroPowerManager    |
| RadioHead        | Required to control the RFM96W radio module       | https://github.com/PaulStoffregen/RadioHead/       | 

## Installation
Open the `cryoskills-receiver.ino` file in the Arduino IDE and ensure that the file builds on your PC.

If there are build errors, check that you have the correct libraries installed.

Select the receiver mode that you want to use by modifying the `RECEIVER_MODE` macro.

| Receiver Mode | Description                                                                       |
| MODE_TEXT     | Select this mode if you wish to view the receiver output in the a Serial Monitor. |
| MODE_BYTES    | Select this mode if you are using the CryoSkills receiver GUI.                    |

When you are ready, connect the Adalogger to your PC and upload the firmware. Make a note of the COM port or serial address that you are using and you will need this for the receiver GUI.