#ifndef HELPERS_HARDWARE_H
#define HELPERS_HARDWARE_H

#include <Arduino.h>

#include "../DataStructs/DeviceModel.h"
#include "../DataStructs/PinMode.h"
#include "../Globals/ResetFactoryDefaultPref.h"
#include "../../ESPEasy-Globals.h"

/********************************************************************************************\
 * Initialize specific hardware settings (only global ones, others are set through devices)
 \*********************************************************************************************/
void hardwareInit();

void initI2C();

void checkResetFactoryPin();

int espeasy_analogRead(int pin);

#ifdef ESP32
int espeasy_analogRead(int pin, bool readAsTouch);
#endif


/********************************************************************************************\
   Hardware information
 \*********************************************************************************************/
uint32_t getFlashRealSizeInBytes();

/********************************************************************************************\
   Hardware specific configurations
 \*********************************************************************************************/
String getDeviceModelBrandString(DeviceModel model);

String getDeviceModelString(DeviceModel model);

bool modelMatchingFlashSize(DeviceModel model);

void setFactoryDefault(DeviceModel model);

/********************************************************************************************\
   Add pre defined plugins and rules.
 \*********************************************************************************************/
void addSwitchPlugin(taskIndex_t taskIndex, byte gpio, const String& name, bool activeLow);

void addPredefinedPlugins(const GpioFactorySettingsStruct& gpio_settings);

void addButtonRelayRule(byte buttonNumber, byte relay_gpio);

void addPredefinedRules(const GpioFactorySettingsStruct& gpio_settings);

// ********************************************************************************
// Get info of a specific GPIO pin.
// ********************************************************************************
// return true when pin can be used.
bool getGpioInfo(int gpio, int& pinnr, bool& input, bool& output, bool& warning);


#ifdef ESP32

// Get ADC related info for a given GPIO pin
// @param gpio_pin   GPIO pin number
// @param adc        Number of ADC unit (0 == Hall effect)
// @param ch         Channel number on ADC unit
// @param t          index of touch pad ID
bool getADC_gpio_info(int gpio_pin, int& adc, int& ch, int& t);
int touchPinToGpio(int touch_pin);

#endif


// ********************************************************************************
// change of device: cleanup old device and reset default settings
// ********************************************************************************
void setTaskDevice_to_TaskIndex(pluginID_t taskdevicenumber, taskIndex_t taskIndex);

// ********************************************************************************
// Initialize task with some default values applicable for almost all tasks
// ********************************************************************************
void setBasicTaskValues(taskIndex_t taskIndex, unsigned long taskdevicetimer,
                        bool enabled, const String& name, int pin1, int pin2, int pin3);

#endif // HELPERS_HARDWARE_H

