# XgunnerGUI

Windows configuration tool for XGUNNER light guns: button remapping, screen calibration, firmware updates, vibration settings, and gamepad testing. Plug the receiver into USB and power on the light gun to get started.

**Quick start:** Run `dist/XgunnerGUI.exe` → select your light gun at the top → set button mappings and click **Start Assign** → click **Calibrate** when needed. The app checks for GUI updates on startup; you can also click **Check for Updates** manually.

---

## Repository contents

| Folder | Description |
|--------|-------------|
| `dist/` | Windows GUI app `XgunnerGUI.exe` (double-click to run; no Python required) |
| `gun/` | Light gun firmware UF2 files (P1–P4, players 1–4) |
| `Receiver/` | Receiver firmware UF2 files (P1–P4) |

**Firmware update:** Hold the device **Upgrade** button → plug in USB → release the button to enter BOOT drive → drag the matching UF2 from `gun/` or `Receiver/` onto the drive letter.

---

## Before you begin

1. **Hardware:** Connect the receiver via USB; power on the light gun (wireless link to the receiver).
2. **Driver:** Windows should enumerate a serial port (USB VID `1209`, PID `0001`–`0004`, shown as XGUNNER-P1–P4).
3. **Multiple guns:** Use the **Gun Select** dropdown at the top; open the list again after hot-plugging to refresh devices.

---

## Main features

### Button mapping (remap)

1. Plug in the receiver and power on the light gun.
2. Choose a mapping for each button below (keyboard keys, arrows, ESC/Enter/Space, etc.). Left mouse is fixed to the trigger and cannot be changed.
3. Click **Start Assign**: the app handshakes with the gun, writes the mapping, and reboots the device on success.
4. If assignment fails, power-cycle the gun and receiver and try again.
5. **Highlight**: highlights the mapped button on the gun diagram for reference.
6. **Profiles:** **Load Profile** / **Save Profile** as JSON, or pick a preset and click **Apply**.

With **Auto sync on connect** enabled (**Advanced Settings** menu), mappings are read back from the device automatically after connection.

### Calibration

1. Click **Calibrate** to send the enter-calibration command.
2. **Turn off rapid fire** if your gun has a rapid-fire switch.
3. **Pull the trigger** to start; move the gun so the cursor aligns with each on-screen target and shoot.
4. Complete all calibration points; after vibration, fire once or follow the prompt to exit calibration.

### Firmware upgrade

1. Click **Firmware Upgrade** to open the upgrade screen.
2. On gun/receiver: **hold Upgrade → plug USB → release** to enter BOOT mode.
3. Choose device type (light gun / receiver), player slot (P1–P4), then **Start Upgrade** and write the UF2 as prompted.  
   Firmware files are in the `gun/` and `Receiver/` folders in this repo.

### IR positioning test

Click **IR Test** for raw infrared data passthrough to verify the IR LEDs and camera. Click **Exit Test** when finished.

### IR LED installation

Click **LED Install** to view the recommended IR LED placement on the gun.

### Gamepad test

Click **Gamepad Test**, choose left or right stick mode, and watch live button/stick feedback (requires device support).

### Vibration settings

Click **Vibration Settings** to adjust:

- Solenoid pulse interval and on-time  
- Trigger hold threshold for continuous vibration  
- Motor rumble duration  
- Swap solenoid and motor pins (applies after save)  

Save to reboot the device and apply settings.

### Check for updates (GUI app)

**Check for Updates** at the top fetches the latest `XgunnerGUI.exe` from GitHub. A background check also runs at startup and prompts when a new version is available.

### Advanced settings

**Advanced Settings** in the menu bar:

- **Baud rate:** `115200` is usually fine.  
- **Auto sync on connect:** read mappings from the device after connect.  
- **Manual sync from device:** read mappings once on demand.  
- **Debug log:** view serial traffic; copy logs for troubleshooting.

### UI shortcuts

| Action | Description |
|--------|-------------|
| **F11** | Toggle fullscreen / windowed |
| **Esc** | Exit fullscreen; close some sub-windows |
| **Language** (top) | Switch UI language (Chinese, English, Japanese, etc.) |

---

## Troubleshooting

| Issue | Suggestion |
|-------|------------|
| Gun not detected | Check USB and that the gun is powered on; try another USB port |
| Assign failed / no SU response | Power-cycle gun and receiver, then **Start Assign** again |
| Cursor inaccurate after calibration | Re-run calibration with rapid fire off |
| No BOOT drive for firmware | Upgrade button sequence: hold → plug in → release |

---

## Author

[cmjdbb](https://github.com/cmjdbb)
