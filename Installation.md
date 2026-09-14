# Installing WazamonoCore

WazamonoCore is the dedicated core for developing the Wazamono series (boards based on the AVR DU) in the Arduino IDE.

## Supported IDEs

- **Arduino IDE 1.8.13 or later** (recommended)
- **Arduino IDE 2.x**

> **On Linux:** Always use the Arduino IDE distributed by [arduino.cc](https://www.arduino.cc). The versions in distribution package managers are modified and are known not to work correctly with third-party board packages.

## Supported hosts (toolchain)

Installation via the Board Manager currently supports the following hosts.

| Host | Status |
|------|--------|
| Windows x64 | ✅ Supported |
| Linux x64 | ✅ Supported |
| macOS (Intel / Apple Silicon), Linux ARM64 | 🚧 In preparation |

---

## Via Board Manager (recommended)

1. Open **File > Preferences** in the Arduino IDE and add the following to **Additional boards manager URLs**:

   ```
   https://wazamono.ws-asahi.net/package_wazamono_index.json
   ```

2. Open **Tools > Board > Boards Manager** and search for "**Wazamono**".

3. Select **Wazamono Boards (AVR DU series)** and click **Install**.
   Along with the core itself, the dedicated toolchain wazamono-toolchain (avr-gcc 15.2.0 / avrdude 8.1) is downloaded and configured automatically.

4. Installation is complete when **Wazamono Tachi / Tsurugi / Kunai** appear under **Tools > Board > WazamonoCore**.

> Several hundred MB will be downloaded, including the toolchain. The installation location is
> the Arduino IDE's standard package folder (Windows: `%LOCALAPPDATA%\Arduino15\packages\WazamonoCore\`).

---

## Manual installation (for developers; hardware folder)

This is how to install when developing or modifying the core itself. **For normal use, installation via the Board Manager is recommended.**

1. `git clone` this repository, or download and extract the ZIP.

2. Place it in the `hardware` folder of your sketchbook with the folder name **`WazamonoCore`**. The layout should look like this:

   ```
   <sketchbook>/
     └─ hardware/
          └─ WazamonoCore/
               └─ megaavr/
                    ├─ boards.txt
                    ├─ platform.txt
                    ├─ cores/
                    ├─ variants/
                    └─ ...
   ```

   The sketchbook location is shown under **File > Preferences > Sketchbook location** in the Arduino IDE.

   - Windows example: `Documents\Arduino\hardware\WazamonoCore\`
   - macOS example: `~/Documents/Arduino/hardware/WazamonoCore/`
   - Linux example: `~/Arduino/hardware/WazamonoCore/`

3. **Install the toolchain (required for manual installation).**
   With a manual installation the IDE does not resolve the toolchain automatically, so
   the IDE's stock avr-gcc 7.3.0 (which does not support the AVR DU) would be used and the build would fail.
   Run the script in `tools\`. It reads `docs\package_wazamono_index.json` (the Boards Manager
   index), downloads the avr-gcc / avrdude versions it lists from the
   [wazamono-toolchain](https://github.com/ws-asahi/wazamono-toolchain) releases (SHA-256 verified)
   and places them under `hardware\WazamonoCore\tools\`:

   - Windows: double-click `tools\setup_toolchain.bat` (or run it from `cmd`)
   - macOS / Linux: `tools/setup_toolchain.sh` (needs `python3` or `jq`)

   ```
   WazamonoCore/
     megaavr/                          <- the core
     tools/avr-gcc/15.2.0-wazamono2/   <- bin/avr-gcc ...
     tools/avrdude/8.1-wazamono2/      <- bin/avrdude, etc/avrdude.conf
   ```

   Arduino IDE 2 / arduino-cli registers `hardware\<vendor>\tools\<name>\<version>\` as
   `{runtime.tools.<name>-<version>.path}`, and `platform.txt` pins exactly those versions, so
   compiling, sketch upload and **Burn Bootloader** all use this toolchain. `platform.local.txt`
   is no longer used (it only affected compilation, not uploads; the script deletes it if present).

   > **Note:** a toolchain placed under the sketchbook's `Arduino\tools\` is *not* seen by IDE 2.
   > It must live under `hardware\WazamonoCore\tools\`.
   > macOS and Linux (aarch64) have no avr-gcc build yet, so only avrdude is installed there.
   > Re-run the script after updating the index (`--force` reinstalls).

4. Restart the Arduino IDE.

> **If both the Board Manager and manual installations are present:** the sketchbook
> (hardware folder) copy takes precedence. To test the Board Manager copy, temporarily
> rename the hardware folder copy. You can tell which one is in use from the line
> `Using board ... from platform in folder:` at the start of the build log.

---

## Flashing the bootloader for the first time (if needed)

Wazamono production boards ship with the USB bootloader already programmed, so normally you can upload sketches simply by connecting over USB.

If you need to program the bootloader (for a self-built or repaired board), use a UPDI programmer.

- Supported programmers: PICkit 4 / 5, Atmel-ICE, SerialUPDI adapters, jtag2updi, etc.
- Connection: the UPDI pin / pad
- Procedure: select the programmer under **Tools > Programmer**, then run **Burn Bootloader**

---

## Verifying the installation

After installation, the following sketch can be used to confirm that everything works.

```cpp
void setup() {
  Serial.begin(115200);          // Serial = USB CDC (no converter chip)
  pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
  Serial.println(millis());
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(500);
}
```

Select the board and upload. If values appear in the Serial Monitor (115200 bps) and the on-board LED blinks, the installation is working.
