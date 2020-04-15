/*
 * Written by HungVu<hungvu98.hust@gmail.com> in April, 2020
 */

#ifndef ANDROID_HARDWARE_HVULIGHT_V2_0_HVULIGHT_H
#define ANDROID_HARDWARE_HVULIGHT_V2_0_HVULIGHT_H

#include <android/hardware/hvulight/2.0/IHvulight.h>
#include <hardware/hardware.h>
#include <hidl/MQDescriptor.h>
#include <cutils/properties.h>
#include <hidl/Status.h>
#include <map>


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <linux/msm_mdp.h>

#include <sys/ioctl.h>
#include <sys/types.h>


struct hvulight_t {
	pthread_mutex_t g_lock;
};

const static std::string RED_LED_BASE
        = "/sys/class/leds/led:rgb_red/";

const static std::string GREEN_LED_BASE
        = "/sys/class/leds/led:rgb_green/";

const static std::string BLUE_LED_BASE
        = "/sys/class/leds/led:rgb_blue/";

const static std::string RED_LED_FILE
        = RED_LED_BASE + "brightness";

const static std::string GREEN_LED_FILE
        = GREEN_LED_BASE + "brightness";

const static std::string BLUE_LED_FILE
        = BLUE_LED_BASE + "brightness";

const static std::string RED_BLINK_FILE
        = RED_LED_BASE + "blink";

const static std::string GREEN_BLINK_FILE
        = GREEN_LED_BASE + "blink";

const static std::string BLUE_BLINK_FILE
        = BLUE_LED_BASE + "blink";

namespace android {
	namespace hardware {
		namespace hvulight {
			namespace V2_0 {
				namespace implementation {
					using::android::hardware::hvulight::V2_0::IHvulight;
                    using::android::hardware::hvulight::V2_0::Status;
                    using::android::hardware::hvulight::V2_0::Led;
                    using::android::hardware::Return;
                    using::android::hardware::Void;
                    using::android::hardware::hidl_vec;
                    using::android::hardware::hidl_string;
                    using::android::sp;

					struct Hvulight : public IHvulight {
						public:
							Hvulight();
							static IHvulight *getInstance();
							// Methods from ::android::hardware::hvulight::V2_0::IHvulight follow.
							Return<Status> setLed(Led led, uint32_t intensity) override;
							Return<int32_t> getLed(Led led) override;

						private:
							static Hvulight *sInstance;
							hvulight_t *mDevice;
							int getBrightnessLed(const std::string &path);
							int setBrightnessLed(const std::string &path, unsigned int intensity);
							static int writeInt(const std::string &path, int value);
                        	static int readInt(const std::string &path);
                        	void openHal();
					};
				} // namespace implementation
			} // namespace V2_0
		} // namespace hvulight
	} // namespace hardware
} //namespace android

#endif // ANDROID_HARDWARE_HVULEDS_V2_0_HVULEDS


							
