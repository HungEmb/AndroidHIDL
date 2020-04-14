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
hardware/interfaces/hvulight/2.0
├── Android.bp
├── default
│   ├── Android.bp
│   ├── android.hardware.hvulight@2.0-service.rc
│   ├── Hvulight.cpp
│   ├── Hvulight.h
│   └── service.cpp
├── IHvulight.hal
└── types.hal
```
### Acomplish HAL layer
- ```default/Hvulight.h``` is header file of HAL implementation
- ```default/Hvulight.cpp``` contains source code of HAL implementaion. After building this file, a shared library named ```android.hardware.hvulight@2.0-impl```, which is invoked by HIDL server in implementing time, will be created
- ```default/service.cpp``` contains source code of HIDL server. Then it is built into ```android.hardware.hvulight@2.0-service``` service.
```
#define LOG_TAG "android.hardware.hvulight@2.0-service"

#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>
#include <android-base/logging.h>
#include <android/hardware/hvulight/2.0/IHvulight.h>
#include "Hvulight.h"

using android::hardware::hvulight::V2_0::IHvulight;
using android::hardware::hvulight::V2_0::implementation::Hvulight;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::sp;
using android::NO_ERROR;

int main() {
    LOG(INFO) << __func__ << " : Start HAL";
    android::sp<IHvulight> hvulight = Hvulight::getInstance();

    configureRpcThreadpool(1, true /*callerWillJoin*/);

    if (hvulight != nullptr) {
        auto rc = hvulight->registerAsService();
        if (rc != NO_ERROR) {
            LOG(ERROR) << "Cannot start Hvulight service: " << rc;
            return rc;
        }
    } else {
        LOG(ERROR) << "Can't create instance of Hvulight, nullptr";
    }

    joinRpcThreadpool();

    return 0; // should never get here
}
```
- ```default/android.hardware.hvulight@2.0-service.rc``` rc file contains action associated its service(which is ```android.hardware.hvulight@2.0-service``` in this case) and it will be parsed by init process. Having parsed and run this rc file, ```android.hardware.hvulight@2.0-service``` server will run and be available for invoking by clients since that time.
```
service vendor.hvulight-hal-2-0 /vendor/bin/hw/android.hardware.hvulight@2.0-service
    interface android.hardware.hvulight@2.0::IHvulight default
    class hal
    user root
    group root
    shutdown critical
```
- ```Android.mk```(replaces ```Android.bp```) is makefile for compiling ```android.hardware.hvulight@2.0-service``` server binary and ```android.hardware.hvulight@2.0-impl.so``` shared library.
```
#===============================================
# Makefile for compliling service binary
#===============================================
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_PROPRIETARY_MODULE := true

LOCAL_MODULE := android.hardware.hvulight@2.0-service
LOCAL_INIT_RC := android.hardware.hvulight@2.0-service.rc
LOCAL_SRC_FILES := \
    service.cpp \

LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libdl \
    libbase \
    libutils \
    libhardware \
    libhidlbase \
    libhidltransport \
    android.hardware.hvulight@2.0 \
    android.hardware.hvulight@2.0-impl \

include $(BUILD_EXECUTABLE)

#=================================================
# Makefile for compiling implement shared library
#=================================================
include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.hvulight@2.0-impl
LOCAL_SRC_FILES := Hvulight.cpp

LOCAL_SHARED_LIBRARIES := \
    libbase \
    liblog \
    libhidlbase \
    libhidltransport \
    libhardware \
    libutils \
    android.hardware.hvulight@2.0 \

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
```




