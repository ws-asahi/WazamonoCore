# WazamonoCore built-in libraries
WazamonoCore ships every library the Wazamono boards (Tachi, Tsurugi, Kunai - all AVR DU-series) need, built in. The DU-specific peripheral libraries follow one shared design: a pre-instantiated object (like `Serial`), fixed board-appropriate pins referred to by their Arduino numbers, and no need to open the I/O headers - `AnalogComp`, `CustomLogic` and `EventSystem` are made to be used together. Libraries for peripherals the DU-series does not have (TCD, ZCD, OPAMP, PTC, MVIO) are not included.

## Wazamono-specific

### Flash
[Flash readme](../libraries/Flash/README.md) Write to the program flash from a running sketch. On the Wazamono boards the writes go through a tiny `spm z+; ret` stub that the USB CDC bootloader carries in the last bytes of its 4 KB boot section; `Flash.checkWritable()` verifies the bootloader before anything is touched, and every write below the 4 KB boundary (the bootloader itself) is refused. Erase granularity is the 512-byte flash page. Use the highest addresses first to stay clear of your own code. Hardware-verified with the FlashDemo and FlashWriteTest examples.

### USERSIG
[USERSIG readme](../libraries/USERSIG/README.md) The USERROW ("user signature space") is 512 bytes of non-volatile memory at 0x1200 that survives a chip erase and ordinary sketch uploads - the natural home for serial numbers, calibration constants and board settings. The library provides the EEPROM-style API (`read`/`write`/`update`/`get`/`put`). Because the USERROW erases only as a whole, writes that need a bit set back to 1 are buffered in RAM (512 bytes) until `flush()`; `write()` returns 1 when the byte went straight to the row and 0 when it is waiting for a `flush()`. Endurance is limited - don't write in a loop. The UsersigTest example checks the full 512-byte range on hardware.

### DxCore
[DxCore readme](../libraries/DxCore/README.md) Inherited helper wrappers around chip configuration (also home of the PWMTest example). Parts of its API refer to peripherals the DU does not have (MVIO, OPAMP); it is kept for compatibility and will be slimmed before the board package ships.

## Peripheral libraries
The DU's analog comparator, configurable logic and event system are exposed through three small libraries that share one style and plug into each other.

### AnalogComp
[AnalogComp readme](../libraries/AnalogComp/README.md) The on-chip analog comparator (AC0) as a single ready-made object: compare two voltages, or one voltage against an internal reference (`begin(INTERNAL2V5)` etc., with a fine-grained level), add hysteresis, read the verdict, drive it onto the AC output pin, or get `attachInterrupt()`-style callbacks. The result can also feed `CustomLogic` (`LOGIC_ANALOG_COMP`) or an `EventSystem` connection (`EVENT_ANALOG_COMP`) with no pin and no CPU involved.

### CustomLogic
[CustomLogic readme](../libraries/CustomLogic/README.md) The CCL (Configurable Custom Logic) look-up tables as ready-made units on fixed pins: pick a gate (`AND`/`OR`/`XOR`/`NAND`/`NOR`/`XNOR`/`NOT`/`NOP`) or any 3-input truth table, and the result appears in hardware with zero CPU time. Inputs can come from the unit's pins, the analog comparator, the unit's own output (latches!), the other unit, or any pin via `EventSystem` (`setInputINn()`); the result can go to the unit's OUT pin, its alternate pin, and the board's event-output pins, several at once (`setOutput()`/`addOutput()`). `attachInterrupt()`-style callbacks on the output are available.

### EventSystem
[EventSystem readme](../libraries/EventSystem/README.md) The chip's internal wiring loom: six ready-made connections (`EventSystem` .. `EventSystem5`), each carrying one source - an Arduino pin, the AnalogComp result, a CustomLogic output, or a software `trigger()` pulse - to any number of destinations: the board's fixed event-output pins or the CustomLogic event inputs. `EventSystem.connect(8, 2);` is a complete program. Pin sources are limited to two per port at a time (a hardware property); timer/USART/SPI event hooks are deliberately not offered.

## USB class libraries
The Wazamono boards are native USB devices; besides the CDC serial port (`Serial`), two bundled class libraries let a sketch act as HID devices or a MIDI instrument. Both are maintained Wazamono forks carrying the AVR DU support that has been submitted upstream (see the bundling notes in each readme).

### HID-Project
[HID-Project readme](../libraries/HID-Project/Readme.md) NicoHood's extended HID library (bundled from the ws-asahi/HID fork, MIT): BootKeyboard/BootMouse, Keyboard, Mouse, AbsoluteMouse, Consumer (media keys), System, Gamepad and RawHID. 13 examples included.

### MIDIUSB
[MIDIUSB readme](../libraries/MIDIUSB/README.adoc) The Arduino MIDIUSB library (bundled from the ws-asahi/MIDIUSB fork, LGPL 2.1): the board enumerates as a USB-MIDI instrument; read and write MIDI event packets. 5 examples included.

### HID
The low-level PluggableUSB HID transport (`HID_`) that HID-Project builds on. Not used directly in sketches.

## Standard Arduino libraries

### EEPROM
[EEPROM readme](../libraries/EEPROM/README.md) The standard API for the DU's 256 bytes of on-chip EEPROM (byte erase granularity - unlike the USERROW, no buffering games are needed). Libraries that make assumptions about EEPROM internals on other architectures may need care.

### SPI
[SPI readme](../libraries/SPI/README.md) The standard SPI master API on SPI0. Each Wazamono board pins the SPI mux to match its silk screen (Tsurugi: the Uno D11-D13 positions), so no `swap()` calls are needed - or wanted - in sketches.

### Wire
[Wire readme](../libraries/Wire/README.md) TWI master and/or slave (dual mode supported) with the full Arduino API plus extras: general-call/broadcast reception, a second address or an address mask. Note that on-chip pull-ups are not enabled automatically; call `Wire.usePullups()` if your bus has none (real pull-up resistors are still the proper fix).

### SD
[SD readme](../libraries/SD/README.adoc) The standard Arduino SD card library (FAT16/FAT32 over SPI). 7 examples included.

### SoftwareSerial
Unmodified from the official megaavr core, and best avoided: every board already has the USB CDC port (`Serial`) plus hardware USARTs on fixed pins. SoftwareSerial takes over the pin interrupts it uses and burns CPU time bit-banging.

## Common hardware

### Servo
[Servo documentation](https://www.arduino.cc/reference/en/libraries/servo/) The improved megaTinyCore/DxCore reimplementation: independent of the TCA0 prescaler (changing PWM frequency does not break servos) with tighter ISR timing. If a Library-Manager copy of Servo shadows the built-in one, `#include <Servo_DxCore.h>` instead - the API is identical.

### tinyNeoPixel
[tinyNeoPixel documentation](tinyNeoPixel.md) WS2812-family ("NeoPixel") control in two flavors: `tinyNeoPixel` (Adafruit-compatible, dynamic buffer) and `tinyNeoPixel_Static` (you declare the frame buffer, so the RAM cost is visible in the compile report and there is no malloc). The show() timing is written for the AVRxt instruction timing of these parts and holds across the supported clock speeds.
