# Android full-stack Project
This project focuses on programming a full-stack application in order to support new hardware in Android. My example is written on Android 9 for Sony Xperia XZ. The idea is to control indicator rdg led of Sony Xperia XZ. Based on my tutorial, Readers are able to program a full-stack aplication for any specific device with some changes.
## HAL Layer
### Create a sketch for HIDL interface
These steps are for creating new HAL layer sketch formated under HIDL(HAL interface definition language)
- Create all necessary folders
```
# cd android_source_tree
# mkdir -p hardware/interfaces/hvulight/2.0/default
```
- Create 2 files in ```hardware/interfaces/hvulight/2.0```: IHvulight.hal and types.hal define interfaces and types of data respectively
- IHvulight.hal content
```
package android.hardware.hvulight@2.0;

interface IHvulight {
    setLed(Led led, uint32_t intensity) generates (Status status);
    getLed(Led led) generates (int32_t intensity);
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
- Next, Use hidl-tools to generate HAL files(source code and Android.bp files in ```hardware/interfaces/hvulight/2.0/default``` for HAL implementation)
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
- Having performed all above steps, We have a folder for HAL layer with structure like below
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
- ```default/Hvulight.cpp``` contains source code of HAL implementaion. After building this file, a shared library named ```android.hardware.hvulight@2.0-impl``` which is invoked by HIDL server in executing time, will be created
- ```default/service.cpp``` contains source code of HIDL server. Then it is built into ```android.hardware.hvulight@2.0-service``` service binary.
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
### Add SElinux for HAL layer
- Attach SElinux context to ```android.hardware.hvulight@2.0-service``` HIDL server binary. Add this line to ```file_contexts``` file in specific device's sepolicy folder( ```device/sony/sepolicy/vender/file_contexts``` in my case)
```
/(system/vendor|vendor)/bin/hw/android\.hardware\.hvulight@2\.0-service                 u:object_r:hal_hvulight_default_exec:s0
```
- Create ```hal_hvulight_default.te``` in ```system/sepolicy/vendor``` and Define rules so that init process can manipulate the ```android.hardware.hvulight@2.0-service``` HIDL server binary.
```
type hal_hvulight_default, domain;
hal_server_domain(hal_hvulight_default, hal_hvulight)

type hal_hvulight_default_exec, exec_type, vendor_file_type, file_type;
init_daemon_domain(hal_hvulight_default)
```
  I will clarify above rules now. Having parsed sepolicy macros, this is content of above ```hal_hvulight_default.te``` file:
```
# define hal_hvulight_default domain for HIDL server
type hal_hvulight_default, domain;

typeattribute hal_hvulight_default halserverdomain;
typeattribute hal_hvulight_default hal_light_server;
typeattribute hal_hvulight_default hal_light;

type hal_hvulight_default_exec, exec_type, vendor_file_type, file_type;

# Old domain may exec the file and transition to the new domain.
allow init hal_hvulight_default_exec:file { getattr open read execute map };
allow init hal_hvulight_default:process transition;

# New domain is entered by executing the file.
allow hal_hvulight_default hal_hvulight_default_exec:file { entrypoint open read execute getattr map };

# New domain can send SIGCHLD to its caller.
ifelse(init, `init', `', `allow hal_hvulight_default init:process sigchld;')

# Enable AT_SECURE, i.e. libc secure mode.
dontaudit init hal_hvulight_default:process noatsecure;

# XXX dontaudit candidate but requires further study.
allow init hal_hvulight_default:process { siginh rlimitinh };

# Convert from init domain into hal_hvulight_default for HIDL server process 
# when init process runs hal_hvulight_default_exec file and forks new process
# This new process will be attach hal_hvulight_default type
type_transition init hal_hvulight_default_exec:process hal_hvulight_default;
```
- Define rules so that HAL's process can manipulate device file of led in sysfs
```
# Allow hal_hvulight_default process read and write sysfs_msm_subsys files
r_dir_rw_file(hal_hvulight_default, sysfs_msm_subsys)

# Allow hal_hvulight_default to read and write  led device file in sysfs
allow hal_hvulight_default sysfs_leds:file rw_file_perms;
```
### HAL testing

## System Service and JNI layer 
### Sketch
- Create ```IHvuledsService.aidl``` file in ```frameworks/base/core/java/android/os/``` for AIDL. This file declares abstract interface and class for System service
- Modify ```frameworks/base/Android.bp``` to build ```IHvuledsService.aidl```
- Create ```HvulightService.java``` file in ```frameworks/base/services/core/java/com/android/server/``` for system service. This file overrides class and method in AIDL file.
- Modify ```frameworks/base/services/java/com/android/server/SystemServer.java``` to initialize the service in init process.
- Modify ```frameworks/base/services/core/Android.bp``` to build java side of HIDL (HIDL's type will be used by Service)
- Create ```com_android_server_hvulight_HvulightService.cpp``` for JNI layer
- Modify ```frameworks/base/services/core/jni/onload.cpp``` to define JNI layer 
- Modify ```frameworks/base/services/core/jni/Android.bp``` to build JNI layer



