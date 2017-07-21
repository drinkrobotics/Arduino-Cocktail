# Arduino Cocktail

[![Build Status](https://travis-ci.org/xythobuz/Arduino-Cocktail.svg?branch=master)](https://travis-ci.org/xythobuz/Arduino-Cocktail)

This Arduino sketch can be used to control a cocktail machine, consisting in my case of four solenoids, controlled by relais, that open or close the path for up to four ingredients.

The user interface is implemented using an 128x64 monochrome I2C OLED and three pushbuttons.

Depends on the [u8glib](https://github.com/olikraus/u8glib)!

## Hardware

The hardware for this project consists of an [Arduino Nano](https://store.arduino.cc/usa/arduino-nano) controlling a [4-relay module](https://www.sainsmart.com/4-channel-5v-relay-module-for-pic-arm-avr-dsp-arduino-msp430-ttl-logic.html) connected to four [12V solenoid valves](http://www.magnetventile-shop.de/magnetventile-2/magnetventile-kunststoff-2/magnetventile-kunststoff-direktgesteuert/elektro-magnetventil-g1/8-zoll-nc-direktgesteuert-v23-pps.html). For the user-interface there's an [128x64 I2C OLED](http://blog.simtronyx.de/kurz-vorgestellt-096-zoll-oled-display-i²c-mit-128x64-pixel/) and three [big red buttons](https://www.adafruit.com/product/1445) as well as a power switch rated for at least 2A at 12V. Everything is powered by a [3S 4Ah LiPo battery](https://hobbyking.com/en_us/multistar-high-capacity-3s-4000mah-multi-rotor-lipo-pack.html?___store=en_us) and two [LM2596 Step-Down voltage regulator modules](https://www.sainsmart.com/tools-equipments/converters/sainsmart-lm2596-dc-dc-buck-converter-step-down-module-power-supply-1-23v-30v.html) set to 5V. To monitor the LiPo voltage, I've included a [small 7-segment voltmeter](https://www.adafruit.com/product/460).

Also include some diodes, connected in reverse parallel over each solenoid, to suppress voltage spikes when turning them off. Otherwise, your Arduino may reset whenever the last solenoid is turned off.

[![Fritzing Schematic](http://xythobuz.de/img/arduino-cocktail-schem-small.png)](http://xythobuz.de/img/arduino-cocktail-schem.png) (Click image for full-size)

## Licensing

    ----------------------------------------------------------------------------
    "THE BEER-WARE LICENSE" (Revision 42):
    <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
    you can do whatever you want with this stuff. If we meet some day, and you
    think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
    ----------------------------------------------------------------------------

