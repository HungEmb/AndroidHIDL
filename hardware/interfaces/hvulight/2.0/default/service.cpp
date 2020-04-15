/*
 * Written by HungVu<hungvu98.hust@gmail.com> in April, 2020
 */

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
