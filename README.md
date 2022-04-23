# WORX can logger

Project to have canbus logging of accessories 

## Components

Provided you've the necessary hardware, you need to create the `secrets.h` file based on the `secrets_example.h` file with the proper environment variables.

Hardware:

* `ESP32` -> [link](https://amzn.to/3pe0XVP)
* `Can Transceiver` -> [Waveshare SN65HVD230](https://www.banggood.com/Waveshare-SN65HVD230-CAN-Bus-Module-Communication-CAN-Bus-Transceiver-Development-Board-p-1693712.html?rmmds=myorder&cur_warehouse=CN)

## Connections

  * Transceiver -> Mower
  
    * CAN_H => Yellow
    * CAN_L => Green
  
  * Transceiver-ESP32

    * GND -> GND
    * 3.3 -> 3.3
    * TX -> 17
    * RX -> 16
  
  * ESP32 -> Mower

    * GND -> BLACK
    * VCC(5V) -> RED

## Logging

### Method 1 (via MQTT and SavvyCan)

SavvyCan is an utility mostly used to debug and analyze can data. You can push directly to mosquitto server using savvycan data format to live inspect whats coming from the esp32.

To do so you need to configure SavvyCan setting the MQTT broker info within the *preferences* and adding a MQTT bus in new connection panel with topic "can2"

After doing so you should be able to see data coming from the canbus. Once you are finished capturing you can hit the "Suspend Capturing" button and save the file as csv. 

*Best Practices*:

  * Take short logs and name log file with whatever happened. Ex: `mower-into-trapped-state.csv` or `offlimits-accessory-detect-magnetic-boundary.csv`
  * You can submit log file in an issue here in github or create pull request including the log in the `canbus` folder

## CanBus info

  * 500kbit/s standard format.
  * Mower knows how to recognize the attached accessory type
  * Known Ids

    * 0x610: (2 bytes) accessory sends this message on the bus every 0.5s
        * `02 0f` is being sent by offlimits accessory
    * 0x611: (1 byte ) mower response of the message above
        * `0x01` mowing
        * `0x00` idle/home/not mowing in general
    * 0x418: (8 bytes): offlimits accessory sends such message when in mowing state . This can frame contains the information for the robot to take action. Last byte is `0x00` when in no magnetic tape is away. When magnetic tape is detected and mower is moving relatively to the tape than last byte changes. 

  * DBC file:

    * There is a preliminary dbc file with the known messages tagged.


## Why?
  
There are several reasons why I started working on this. First of all worx accessories are able to both control mower and read its full state (radiolink accessory). 

It's my belief (not confirmed) that by fully reverse engineering the canbus messages we will be able to do wonderful stuff such as:

  * have GPS RTK based navigation (or lidar or vision only?)
  * have a better "untrap routine"
  * create your own range extender (EX: radiolink clone) based on rf433mhz 
  * isolate the mower from the internet and have the status data sent locally. This may also keep the landroid running and controllable even when no internet connection is available

## Want to help?

You can help in many ways:

  1. If you've  aworx with an attached accessory you can build this with platformio and deploy it to an esp32. Once you collect the logs you can send it over as described above.
  2. If you want to help reverse engineer the bus messages you can do it. For now all investigation on such worx mowers is done in the OpenMower [discord](https://discord.gg/jE7QNaSxW7) on this  [channel/thread](https://discord.com/channels/958476543846412329/966633787133947914).


