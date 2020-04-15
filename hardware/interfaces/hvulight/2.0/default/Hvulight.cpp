/*
 * Written by HungVu<hungvu98.hust@gmail.com> in April, 2020
 */

#define LOG_TAG "Hvulight"

#include <android-base/logging.h>
#include <fstream>
#include "Hvulight.h"

#ifdef UCOMMSVR_BACKLIGHT
extern "C" {
#include <comm_server/ucomm_ext.h>
}
#endif

namespace android {
	namespace hardware { 
		namespace hvulight {
			namespace V2_0 {
				namespace implementation {

					Hvulight *Hvulight::sInstance = nullptr;

					Hvulight::Hvulight() {
						LOG(INFO) << "Hvulight object is constructed";
						openHal();
						sInstance = this;
					}

					void Hvulight::openHal() {
						LOG(INFO) << __func__ << ": Setup Hvulight HAL";

                        mDevice = static_cast<hvulight_t *>(malloc(sizeof(hvulight_t)));
                        memset(mDevice, 0, sizeof(hvulight_t));

                        mDevice->g_lock = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
					}

					Return<Status> Hvulight::setLed(Led led, uint32_t intensity) {
                        const static std::string *CONTROLLED_LED_FILE;
                        switch (led) {
                            case Led::RED:
                                LOG(DEBUG) << __func__ << " : Led::RED"; 
                                CONTROLLED_LED_FILE = &RED_LED_FILE;
                                break;
                            case Led::GREEN:
                                LOG(DEBUG) << __func__ << " : Led::GREEN";
                                CONTROLLED_LED_FILE = &GREEN_LED_FILE;
                                break;
                            case Led::BLUE:
                                LOG(DEBUG) << __func__ << " : Led::BLUE";
                                CONTROLLED_LED_FILE = &BLUE_LED_FILE;
                                break;
                            default:
                                LOG(DEBUG) << __func__ << " : Unknown led";
                                return Status::LED_NOT_SUPPORTED;
                        }
                        setBrightnessLed(*CONTROLLED_LED_FILE, intensity);
                        return Status::SUCCESS;
					}

                    Return<int32_t> Hvulight::getLed(Led led) {
                        const static std::string *CONTROLLED_LED_FILE;
                        switch (led) {
                            case Led::RED:
                                LOG(DEBUG) << __func__ << " : Led::RED"; 
                                CONTROLLED_LED_FILE = &RED_LED_FILE;
                                break;
                            case Led::GREEN:
                                LOG(DEBUG) << __func__ << " : Led::GREEN";
                                CONTROLLED_LED_FILE = &GREEN_LED_FILE;
                                break;
                            case Led::BLUE:
                                LOG(DEBUG) << __func__ << " : Led::BLUE";
                                CONTROLLED_LED_FILE = &BLUE_LED_FILE;
                                break;
                            default:
                                LOG(DEBUG) << __func__ << " : Unknown led";
                                return -1;
                        }
                        return getBrightnessLed(*CONTROLLED_LED_FILE);
                    }

					IHvulight *Hvulight::getInstance(void) {
                        if (!sInstance) {
                            sInstance = new Hvulight();
                        }

                        return sInstance;
                    }

                    int Hvulight::writeInt(const std::string &path, int value) {
                        std::ofstream stream(path);
                        if (!stream) {
                            LOG(ERROR) << "Failed to open " << path << ", error=" << errno
                                       << "(" << strerror(errno) << ")";
                            return -errno;
                        }
                        stream << value << std::endl;

                        return 0;
                    }

                    int Hvulight::readInt(const std::string &path) {
                        int value = 0;

                        std::ifstream stream(path);
                        if (!stream) {
                            LOG(ERROR) << "Failed to open " << path << ", error=" << errno
                                       << "(" << strerror(errno) << ")";
                            return -errno;
                        }
                        stream >> value;

                        return value;
                    }

                    int Hvulight::setBrightnessLed(const std::string &path, unsigned int intensity) {
                        if(!mDevice) {
                            return -1;
                        }

                        pthread_mutex_lock(&mDevice->g_lock);
                        writeInt(path, intensity);
                        pthread_mutex_unlock(&mDevice->g_lock);

                        return 0;
                    }

                    int Hvulight::getBrightnessLed(const std::string &path) {
                        int32_t brightness;

                        if(!mDevice) {
                            return -1;
                        }

                        pthread_mutex_lock(&mDevice->g_lock);
                        brightness = (int32_t)readInt(path);
                        pthread_mutex_unlock(&mDevice->g_lock);

                        return brightness;
                    }
                } //namespace implementation
            } // namespace V2_0
        } // namespace hvulight
    } // namespace hardware
} //namespace android
