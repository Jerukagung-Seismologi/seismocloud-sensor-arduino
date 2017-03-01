
# Hardware

## Requirements

* Arduino/Genuino IDE
* Arduino/Genuino UNO board
* Arduino ISP and SD-card (any size >= 10 MB)
* WizNet-compatible Ethernet shield (with SD reader)
* MMA7361 Accelerometer
* (optional) 3 LEDs (red-green-yellow) with 3 resistors

We recommend to buy Arduinos on official website https://store.arduino.cc

## Accelerometer MMA7361

Link these pins from Accelerometer MMA7361 to Arduino board:

* Vin: 5v
* GND: GND
* SEL: GND
* X: A0
* Y: A1
* Z: A2

Loop back **3v3** pin to **SLP** on Accelerometer.

## Burning a new bootloader with Arduino ISP for self-upgrade

In order to self-upgrade itself, Arduino must use an SD-card and a custom bootloader.

**Please note that, with avr_boot bootloader, you'll not be able to debug/upload sketch using standard method**.
To restore old bootloader, follow these instructions except for `10`, when you have to
choose `Arduino/Genuino Uno` as board (then proceed with `11`).

Steps:

1. Unplug all hardware from Arduino Uno
2. Plug the ArduinoISP into ICSP 6-pin-header of Arduino Uno (you can find ICSP
	header on the left side, having USB port on right side)
3. Power up Arduino Uno with a power supply (either with USB port or power jack port)
4. Plug ArduinoISP to PC (with microUSB port on ArduinoISP)
5. Open Arduino IDE (on Linux you may need to open it as root)
6. Open `File > Preferences`. Into `Additional Boards Manager URLs` put `https://zevero.github.io/avr_boot/package_zevero_avr_boot_index.json`
7. Save and close
8. Open `Tools > Board > Boards Manager`. Search for `avr_boot` and install.
9. Close `Boards Manager`
10. On `Tools > Board` choose `ATmega328/P`
11. Launch bootloader burn using `Tools > Burn bootloader`

On finish, you can close the IDE, unplug all and build your SeismoCloud sensor! (remember also to follow `Software` instructions)

## (optional) LEDs

Remember to put a resistor with LED (after/before is not really important) to limit
current flowing, otherwise you may damage the Arduino board.

By default, LED pins are:

* Pin 2 : Yellow
* Pin 5 : Green
* Pin 3 : Red

# Software

## Download sketch into SD-card

After hardware build, you need to setup the software:

1. Format your SD-card with FAT32 (or FAT16)
2. Download FIRMWARE.BIN from http://www.seismocloud.com/page.php?id=instr-arduino
3. Place FIRMWARE.BIN into SD-card
4. Insert SD-card into Arduino Uno and power on.
5. Open SeismoCloud app, connect to the same network of the board and register your device. Enjoy!
