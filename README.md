# ReStar

ReStar is a SteamVR driver patch for the StarVR One that aims to reimplement native SteamVR integration and restore original driver functionality, with modern software compatibility in mind.

> [!CAUTION]
> ReStar is experimental. The author accepts **zero liability** for any damage, injury, hardware failure, or undefined behavior caused by this software. No guarantees are provided—your device **may brick**. Use at your own risk.

## Modifications

- Tracking Override registration has been removed, and pose is now supplied by ReStar
- All settings have been migrated to the SteamVR menu natively
- Properties have been changed to reflect modern SteamVR driver behaviour
- Input profile has added so newer games track properly
- Device icon has been upgraded to the modern SteamVR gradient
- Implements OpenXR Eye-Tracking (XR_EXT_eye_gaze_interaction)

## Installation

### 1. Prequisites

> [!IMPORTANT]
> If your device is not running on firmware **V08.03.05** or *newer* already, you SHOULD do an firmware upgrade, look more for more information [here](https://github.com/Nyabsi/ReStar/wiki/Firmware-Upgrade).

> [!WARNING]
> You need to have an NVIDIA GPU, anything >=1080 Ti *should* work with the device, AMD does not work because the driver relies on proprietary NvAPI functionality for Direct Mode.

You need *StarVRCompass* version `2.99.9999`, you can download this below:

 - [StarVRCompass v2.99.9999 (StarVR Official)](https://stg-starvr-installer.s3-ap-northeast-1.amazonaws.com/v2.99.9999/StarVRCompass.2.99.9999.installer.msi)
 - [StarVRCompass v2.99.9999 (archive.org)](https://archive.org/details/star-vrcompass.-2.99.9999.installer)

*Other versions are not compatible with ReStar, support will not be provided.*

### 2. Download ReStar

Head over to [downloads](https://github.com/Nyabsi/ReStar/releases) and download the latest version of ReStar.

Once the installation is complete, it should automatically register itself and work.

### 3. Launch SteamVR + Pair Tracker

Once you have it running, you can turn on the tracker on the headset and it should automatically start tracking.

## Known issues

- Unreal Engine 5 crash on boot

## License

This repository has been licensed under the MIT license, you can read more about it in the `LICENSE` file.
