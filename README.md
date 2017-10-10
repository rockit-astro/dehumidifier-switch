## W1m dehumidifier relay / light switch unit
Firmware for an Arduino Uno that connects to a solid state relay to control the dome dehumidifier and to a button/LED that toggles the dome lights.
Communicates with the dome computer via [powerd](https://github.com/warwick-one-metre/powerd/tree/centos).


### Software Setup

This firmware targets the atmega328p microcontroller on the Arduino board directly, avoiding the standard Arduino toolchain.

Compilation and installation is done using the `avr-gcc` toolchain:
* On macOS add the `osx-cross/avr` Homebrew tap then install the `avr-gcc`, `avr-libc`, `avr-binutils`, `avrdude` packages.
* On Ubuntu install the `gcc-avr`, `avr-libc`, `binutils-avr`, `avrdude` packages.
* On Windows WinAVR should work, but hasn't been tested.

Compile the firmware by running `make` in the source code directory, and then install it by running `make install`.
The Makefile is configured to try and install the firmware to the first `/dev/tty.usbmodem*` device, so make sure that you only have the target arduino connected!
If you are on Linux or Windows then you will need to change `/dev/tty.usbmodem*` to the appropriate value for your system (`/dev/ttyACM*` or `COM*`).
If the RESET EN trace has been cut on the Arduino then you will need to press and release the reset button simultaneously with running the `make install` command.

### Hardware Setup

The solid state relay should be connected between digital pin 2 and ground.
The status LED should be connected between digital pin 3 and ground (via a current limiting resistor).
The button should be connected between digital pin 4 and ground.