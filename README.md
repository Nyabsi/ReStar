# ReStar

ReStar is a SteamVR driver patch for the StarVR One that aims to reimplement native SteamVR integration and restore original driver functionality, with modern software compatibility in mind.

> [!CAUTION]
> ReStar is highly experimental. The author accepts **zero liability** for any damage, injury, hardware failure, or undefined behavior caused by this software. No guarantees are provided—your device **may brick**. Use at your own risk.

## Patches

- Settings are now integrated to SteamVR, third-party software is no longer required
- Device icon has been upgraded to use the modern SteamVR gradient variant
- An proper input profile is now provided, which is required for modern game engines to function
- The tracking override has been replaced with robust tracking code
- Manual IPD adjustment that modifies UV bounds instead of distortion mesh to improve visual clarity

## Installation

### 1. Software preparation

To be able to launch and use ReStar correctly, you will have to obtain version `1.00.2001` of *StarVRCompass*, and *Tobii Eye Tracking Core* version `2.13.4.7864`, once you have both of these pieces of software installed you can proceed to the next step.

### 2. Firmware upgrade

> [!IMPORTANT]
> If your device firmware is already V08.03.05 or *newer*, you should / can skip this step, and continue to proceed.

You should add the launch argument `server=staging` to the *StarVRCompass* and then launch the software. Ensure your HMD is plugged in, all four cables (2x DP, 2x USB) and then navigate to the "System Updates" navigation tab.

It should say "Updates are available." and an button to install the "Update".

<img width="1129" height="636" alt="image" src="https://github.com/user-attachments/assets/6342e1ee-d25b-4f58-861f-2f73f4c0f533" />

Install the update and wait til you see the following screen:

<img width="1591" height="886" alt="image" src="https://github.com/user-attachments/assets/11653b70-017a-4195-b369-bd06c3a77c1e" />

> [!WARNING]
> When updating the firmware, the software *might* crash, if this happens, re-open the software and begin the update process again til you see the view above.

Once the update is complete, power cycle the device by unplugging the hub from the power supply. After that you can remove the launch argument and navigate to the "System Updates" navigation tab and you should see the firmware version being "08.03.05" or newer.

<img width="1630" height="859" alt="image" src="https://github.com/user-attachments/assets/8e57bba9-1112-4008-8464-0f550b7478e7" />

### 3. Folder creation

For sake of doing it, create an `StarVR` folder to `%appdata%` because the software attempts to load files from there, and it does not create the folder by default.

### 4. SteamVR settings

Add these arguments to the `C:\\Program Files (x86)\\Steam\\config\\steamvr.vrsettings` to the `SteamVR` section when SteamVR is not running.

```
"renderCameraMode" : "raw"
```

> [!NOTE]
> This documentation will be obsolete in the near future as the driver will be automatically setting there for the user, during the driver initialization.

### 5. Driver registration

> [!IMPORTANT]
> The driver is not currently available for download, everything will be released shortly.

Now you can download the ReStar driver from [here](maybe) and move it to either `C:\\Program Files (x86)\\Steam\\steamapps\\common\\SteamVR\\drivers\\` or optionally register it using the `vrreg.exe` utility to anywhere you desire.

#### 6. Launch SteamVR

Now you should be able to launch SteamVR and power on any lighthouse based tracker of your preference and the device should work.

## Known issues

- None I am aware of, if you encounter something. Let me know.

## Troubleshooting

### Why is my HMD rendering fine for 5 seconds, then having massive lag spike and the display goes black

This issue happened to me on RTX 4090 few times, and the solution I found was to power cycle the HMD cable box, and afterwards it worked completely fine.

### Why is my SteamVR completely just pink lines

This issues also happened to me on RTX 4090, I am not quite sure of the specific root cause but what helped was unplugging all DSC devices from my GPU and ensuring there is no monitors plugged on the iGPU, this made the HMD behave properly as expected.

### My view has black bars

If you skipped the step 2 despite it being instructed, the reason you see black bars is because the firmware version V06.06 renders the image incorrectly on the firmware level and is incompatible.

### Does your driver already do autoipd though

Yes, launch the Tobii Runtime and ensure the device is detected by it before launching SteamVR, then you should see three lines in the headset which you need to align for it to calibrate the IPD.

### Tobii eye tracking doesn't detect the HMD / the IR leds don't turn on

Just plug out both USB cables from your computer and re-plug them one by one, and then the device should be detected by the software.

## Hardware compatibility 

> [!NOTE]
> Technically, anything >=1080 Ti *should* work with the device. This list is not indicative of working devices, but devices which have been tested.

| GPU                  | Operating System | Driver Version | Status  |
|----------------------|------------------|----------------|---------|
| RTX 3080             | Windows 10       | N/A            | Working |
| RTX 4090             | Windows 10       | N/A            | Working |
| RTX 5090             | Windows 10       | N/A            | Working |
| RTX 4090             | Windows 11       | N/A            | Working |
| NVIDIA RTX Pro 6000  | Windows 11       | N/A            | Working |

## Game compatibility

> [!NOTE]
> This is non-exhaustive list of very small sample set of games, you can make an issue to let me know what's working/not working, if you want help with imporving the known compatibility list.

| Game              | GPU      | OS         | SteamVR | State   | Comments |
|-------------------|----------|------------|---------|---------|----------|
| BONELAB           | RTX 4090 | Windows 11 | 2.15.6  | Working |          |
| BONEWORKS         | RTX 4090 | Windows 11 | 2.15.6  | Working |          |
| Beat Saber        | RTX 3080 | Windows 10 | 2.15.6  | Working | The game will start misbehaving when the per-eye rendering resolution is too high, if the game has abnormal behaviour i.e., non-stopping lagging or notes rendering incorrectly, decrease the rendering resolution. All effects are only rendered on the right eye. |
| Half-Life: Alyx   | RTX 4090 | Windows 11 | 2.15.6  | Working | Requires launch arguments: `-console -vconsole +sc_no_cull 1 +vr_enable_volume_fog 0 +vr_multiview_instancing 0` |
| POOLS             | RTX 4090 | Windows 11 | 2.15.6  | Working |          |
| Superhot VR       | RTX 4090 | Windows 11 | 2.15.6  | Working |          |
| Google Earth VR   | RTX 4090 | Windows 11 | 2.15.6  | Working |          |
| The Lab           | RTX 4090 | Windows 11 | 2.15.6  | Working |          |
| VRChat            | RTX 4090 | Windows 11 | 2.15.6  | Working |          |
| ChilloutVR        | RTX 3080 | Windows 10 | 2.15.6  | Working | The game will start misbehaving when the per-eye rendering resolution is too high, if the game has abnormal lag, decrease the rendering resolution. |
| Resonite          | RTX 4090 | Windows 11 | 2.15.6  | Working |          |

## License

This repository has been licensed under the MIT license, you can read more about it in the `LICENSE` file.
