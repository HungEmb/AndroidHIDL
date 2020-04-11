# Android full-stack Project
This project focuses on programming a full-stack application in order to support new hardware in Android. My example is written on Android 9 for Sony Xperia XZ. The idea is to control indicator rdg led of Sony Xperia XZ. Based on my toturial, Readers are able to program a full-stack aplication for any specific device with some changes.
## HAL Layer
### Creat a sketch for HIDL interface
These step is responsible for creating new HAL layer sketch formated under HIDL(HAL interface definition language)
- Creat all necessary folders
```
# cd android_source_tree
# mkdir -p hardware/interfaces/hvulight/2.0/default
```
- Creat 2 files in ```hardware/interfaces/hvulight/2.0```: IHvulight.hal and types.hal define interfaces and types of data respectively
- IHvulight.hal content
```
package android.hardware.hvulight@2.0;

interface IHvulight {
    setLed(Led led, int32_t intensity) generates (Status status);
};
```
- types.hal content
```
package android.hardware.hvulight@2.0;

enum Led : int32_t {
    RED,
    GREEN,
    BLUE,
    UNKNOWN,
};

enum Status : int32_t {
    SUCCESS,
    LED_NOT_SUPPORTED,
    BRIGHTNESS_NOT_SUPPORTED,
    UNKNOWN,
};
```
- Next, Use hidl-tools to generate HAL files(inplementing and Android.bp files in ```hardware/interfaces/hvulight/2.0/default```)
```
# PACKAGE=android.hardware.hvulight@2.0
# LOC=hardware/interfaces/hvulight/2.0/default/
# make hidl-gen -j16
# hidl-gen -o $LOC -Lc++-impl -randroid.hardware:hardware/interfaces -randroid.hidl:system/libhidl/transport $PACKAGE
# hidl-gen -o $LOC -Landroidbp-impl -randroid.hardware:hardware/interfaces -randroid.hidl:system/libhidl/transport $PACKAGE
```
- Update Android.bp makefile for compiling HIDL(.hal file in ```hardware/interfaces/hvulight/2.0```)
```
# ./hardware/interfaces/update-makefiles.sh
```
- Add 2 new empty files into ```hardware/interfaces/hvulight/2.0/default```. Its content and purpose I'm going to explain later
```
# touch hardware/interfaces/hvulight/2.0/default/service.cpp
# touch hardware/interfaces/hvulight/2.0/default/android.hardware.hvulight@2.0-service.rc
```
- After performing all above steps, We have a folder for implementing HAL with structure like below
```
hardware/interfaces/hvulight/
└── 2.0
    ├── Android.bp
    ├── default
    │   ├── Android.bp
    │   ├── Hvulight.cpp
    │   └── Hvulight.h
    ├── IHvulight.hal
    └── types.hal
```
### Acomplish HAL layer
- Hvulight.h



