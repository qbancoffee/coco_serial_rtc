# coco_serial_rtc
An external Real Time Clock circuit (RTC) for the Tandy CoCo.

This is a realtime clock prototype for the Tandy CoCo. The board connects to the CoCo's bit banger serial port via a male DIN connector pigtail.
Whith the appropriate commands from the CoCo, one can get the date/time or one can set the date/time. The rest of the time it just sits in place and works as a serial passthrough so that you can still connect other serial devices to the CoCo. The other serial devices can connect to the onboard DIN connector. The board requires +5V DC and can be powered from the CoCo's joystick port or one can provide the voltage to a 2 pin terminal strip. The board also serves a passthrough when using the joystick port.

The board was designed using KiCAD 6 and the firmware was written in C and compiled with the [CCS C Compiler](https://www.ccsinfo.com/compilers.php).


## CoCo Serial RTC
- [Schematic](coco_serial_rtc_schematic.pdf)
- [Rendered Image](images/coco_serial_rtc_top.png)
- [Picture 1](images/coco_serial_rtc.jpg)
- [Picture 2](images/coco_serial_rtc_assembled_1.jpg)
- [Picture 3](images/coco_serial_rtc_assembled_2.jpg)
- [Picture 4](images/coco_serial_rtc_assembled_3.jpg)

## How it works
Add stuff here.


### Current status
- [x] Ordered
- [x] Assembled
- [x] Testing

The board is still in the prototype stage but so far things are looking good.
