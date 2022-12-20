# 1. ```teensy_pd```: Proportional-Derivative control over UDP packets

This is a PD controller project intended to command a Teensy 4.1 microcontroller from a Raspberry Pi 4 B running RaspOS over UDP packets. 

---  

- [1. ```teensy_pd```: Proportional-Derivative control over UDP packets](#1-teensypd-proportional-derivative-control-over-udp-packets)
- [2. Dependencies](#2-dependencies)
- [3. Quick Start](#3-quick-start)
	- [3.1. Build firmware using PlatformIO](#31-build-firmware-using-platformio)
	- [3.2. Copy files to Raspberry Pi](#32-copy-files-to-raspberry-pi)
	- [3.3. Upload the Teensy firmware](#33-upload-the-teensy-firmware)
	- [3.4. Build the Raspberry Pi executable](#34-build-the-raspberry-pi-executable)
	- [3.5. Check if the Teensy was assigned a local IP](#35-check-if-the-teensy-was-assigned-a-local-ip)
	- [3.6. Run the Raspberry Pi main executable](#36-run-the-raspberry-pi-main-executable)
- [4. ```send_udp``` usage](#4-sendudp-usage)
	- [4.1. Flags and their meanings:](#41-flags-and-their-meanings)
	- [4.2. Examples:](#42-examples)

# 2. Dependencies
1.  [udp_msg](https://github.com/cinaral/udp_msg) 
2.  [cxxopts](https://github.com/jarro2783/cxxopts)

# 3. Quick Start

## 3.1. Build firmware using PlatformIO
   Open the [teensy/](teensy) folder using PlatformIO IDE on VS Code, and compile the firmware for the Teensy 4.1.

## 3.2. Copy files to Raspberry Pi
   You need to transfer the firmware and the source files in the ```rasppi/``` folder to the Raspberry Pi. If you can use ```rsync```, you can use the [sync script](shared/scripts/sync_to_rasppi.sh) to do this automatically.
   
   Navigate to the ```teensy41_udp/``` or the project folder on the Raspberry Pi, which should look like (Expand the details to display):
   
<details>

```
│teensy41_pd/
│
├── │rasppi/
│   │
│   ├── │include/
│   │   │  print_echo_resp.hpp
│   │
│   ├── │scripts/build/
│   │   │  build.sh
│   │   │  clear.sh
│   │   │  configure.sh
│   │
│   ├── │src/
│   │   │  main.cpp
│   │   │  send_udp.cpp
│   │
│   │  README.md
│
├── │shared/
│   │
│   ├── │include/
│   │   │  config.hpp
│   │
│	│  README.md
│
├── │teensy/
│   │
│   ├── │.pio/build/teensy41/
│   │   │  firmware.hex
│   │
│   ├── │include
│   │   │  MotorDriver.hpp
│   │   │  UdpHandler.hpp
│   │
│   │
│   ├── │lib
│   │   │
│   │   ├── │pid_control/include/
│   │   │   │
│   │   └── ├── │ pid_control
│   │       │   │   pdf_filter.hpp
│   │       │   │   types.hpp
│   │       │
│   │       │  pid_control.hpp
│   │
│   ├── │scripts/teensy_loader_cli/
│   │   │  upload.sh
│   │   │  upload_to_TEENSY41.sh
│   │
│   ├── │src
│   │   │  main.cpp
│   │   │  MotorDriver.cpp
│   │   │  UdpHandler.cpp
│   │
│   │  README.md
│
│  LICENSE
│  README.md
```
</details>

## 3.3. Upload the Teensy firmware
   Upload the firmware. You can use the [upload script](teensy/scripts/teensy_loader_cli/upload.sh).

## 3.4. Build the Raspberry Pi executable
   Configure and build the Raspberry Pi executable using the [configure script](rasppi/scripts/build/configure.sh) and the [build script](rasppi/scripts/build/build.sh). Delete existing configuration using the [clear script](rasppi/scripts/build/clear.sh).

## 3.5. Check if the Teensy was assigned a local IP
   Check if the Teensy was assigned a local IP:
   ```bash
   rasppi: ~ $ cat /dev/ttyACM0 
   ``` 
   Modify ```/dev/ttyACM0``` with the USB serial port that your Teensy is connected to. You should see something like:
   ```
		Local IP    = 192.168.1.10
		Subnet mask = 255.255.255.0
		Gateway     = 192.168.0.1
		DNS         = 192.168.1.1
	Listening for clients on port 11337...
   ```
   If you get the "Failed to start Ethernet" or "Failed to get IP address from DHCP" error, you should attempt a power cycle. If the error persists, uou should check your Teensy's ethernet hardware configuration and the Raspberry Pi DHCP server configuration.

## 3.6. Run the Raspberry Pi main executable
```bash
rasppi: ~ $ rasppi/build/bin/main
```
You should see the telemetry data printed in your console.

![Telemetry example](https://too.lewd.se/959560c23f2b.gif)

# 4. ```send_udp``` usage

You can use use ```send_udp``` executable to send invididual flags as defined in [config.hpp](shared/include/config.hpp). 

## 4.1. Flags and their meanings:
| Flag (ASCII) | Meaning                                                   | Argument         |
| :----------: | :-------------------------------------------------------- | :--------------- |
| 0x0 ('NUL')  | Null, no message.                                         | N/A              |
|  0x30 ('0')  | Stop the motor.                                           | N/A              |
|  0x31 ('1')  | Reset everything by their default values.                 | N/A              |
|  0x32 ('2')  | Start tracking PD control and start relaying packets.     | target pos (rad) |
|  0x33 ('3')  | Set reference to track.                                   | target pos (rad) |
|  0x34 ('4')  | Set the controller's proportional gain (Kp).              | gain             |
|  0x35 ('5')  | Set the controller's derivative gain (Kd).                | gain             |
|  0x36 ('6')  | Request an echo response.                                 | N/A              |
|  0x60 ('<')  | Marks the beginning of the interrupts for syncronization. | N/A              |
|  0x61 ('=')  | Marks the echo response.                                  | N/A              |

## 4.2. Examples:
 
1. Send a stop command to the default IP and port:
```bash
$ rasppi/build/bin/send_udp -f 0
Sent 0x30: 0 to 192.168.1.10:11337
```

2. Send a track reference command to track $x = 1$ radians to 192.168.1.22 at port 6667:
```bash
$ rasppi/build/bin/send_udp 192.168.1.22 -f 2 -v 1-p 6667
Sent 0x32: 1 to 192.168.1.10:6667
```
