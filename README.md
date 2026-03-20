# ReStar

ReStar is a SteamVR driver patch for the StarVR One that aims to reimplement native SteamVR integration and restore original driver functionality, with modern software compatibility in mind.

> [!CAUTION]
> ReStar is highly experimental. The author accepts **zero liability** for any damage, injury, hardware failure, or undefined behavior caused by this software. No guarantees are provided—your device **may brick**. Use at your own risk.

## Patches

- Some of the driver interfaces have been version upgraded through shim interfaces
- Device controls are now native—no third-party software required (like the Valve Index)
- StarVR One XT now uses mounted Vive, Tundra or other lighthouse tracker for HMD tracking
- Driver properties have been spoofed to reflect modern driver behaviours
- The StarVR tracking override behaviour has been removed

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

### 4. Driver registration

Now you can download the ReStar driver from [here](maybe) and move it to either `C:\\Program Files (x86)\\Steam\\steamapps\\common\\SteamVR\\drivers\\` or optionally register it using the `vrreg.exe` utility to anywhere you desire.

#### 5. Launch SteamVR

Now you should be able to launch SteamVR and power on any lighthouse based tracker of your preference and the device should work.

## Known issues

- There is issues with game compatibility, ie. VRChat, Resonite and ChilloutVR are known to either crash, have extreme lag or lack of positional data for HMD.
- Currently the driver will constantly complains about chaperone configuration, even if you have valid chaperone configuration
- The pose submitted for the render layer is incorrect as of right now, so you might see the left eye behave properly but the right eye is incorrectly behaving.

## Troubleshooting

### Why is my HMD rendering fine for 5 seconds, then having massive lag spike and the display goes black

This issue happened to me on RTX 4090 few times, and the solution I found was to power cycle the HMD cable box, and afterwards it worked completely fine.

### Why is my SteamVR completely just pink lines

This issues also happened to me on RTX 4090, I am not quite sure of the specific root cause but what helped was unplugging all DSC devices from my GPU and ensuring there is no monitors plugged on the iGPU, this made the HMD behave properly as expected.

### My view has black bars

If you skipped the step 2 despite it being instructed, the reason you see black bars is because the firmware version V06.06 renders the image incorrectly on the firmware level and is incompatible.

## Hardware compatibility 

Confirmed to work:

- RTX 3080 (Running on Windows 10)
- RTX 4090 (Running on Windows 11)

## Game compatibility

What works:

- BONELAB
- BONEWORKS
- Beat Saber
- Half Life Alyx
- POOLS
- Superhot
- Google Earth VR
- The Lab

What doesn't work:

- VRChat
- Chillout VR
- Resonite

Games that have not been listed as "What works" or "What doesn't work" are simply untested, this is non-exhaustive list of very small sample set of games I have tried and I remember trying.

## License

This repository has been licensed under the MIT license, you can read more about it in the `LICENSE` file.
