#include <android/hardware/hvulight/2.0/IHvulight.h>
#include <hidl/Status.h>
#include <hidl/LegacySupport.h>
#include <utils/misc.h>
#include <utils/Log.h>
#include <hardware/hardware.h>
#include <hidl/HidlSupport.h>

#include<stdio.h>

using android::hardware::hvulight::V2_0::IHvulight;
using Led       = ::android::hardware::hvulight::V2_0::Led;
using Status    = ::android::hardware::hvulight::V2_0::Status;
using android::sp;  


int main(int argc, char *argv[]) {
      Status res;
      Led led;
      int32_t brightness;
      android::sp<IHvulight> ser = IHvulight::getService();
      
      if(strcmp(argv[1], "RED") == 0) 
            led = Led::RED;
      else if (strcmp(argv[1], "GREEN") == 0)
            led = Led::GREEN;
      else if (strcmp(argv[1], "BLUE") == 0)
            led = Led::BLUE;
      else {
            led = Led::UNKNOWN;
            printf("Led is invaled\n");
      }
      
      brightness = ser->getLed(led);
      printf("Brightness: %d\n", brightness);

      res = ser->setLed(led, 0);
      printf("Status = %d\n",res);
      return 0;
}

