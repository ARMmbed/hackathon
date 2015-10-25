/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __BLE_LIGHT_SERVICE_H__
#define __BLE_LIGHT_SERVICE_H__

#include "ble/BLE.h"


class LightService {
public:
    /**
    * @param[ref] _ble
    *               BLE object for the underlying controller.
    * @param[in] level
    *               8bit batterly level. Usually used to represent percentage of batterly charge remaining.
    */
    LightService(BLE &_ble, uint8_t level = 100) :
        ble(_ble),
        lightLevel(level),
        lightLevelCharacteristic(GattCharacteristic::UUID_LIGHT_LEVEL_CHAR, &lightLevel, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {

        GattCharacteristic *charTable[] = {&lightLevelCharacteristic};
        GattService         lightService(GattService::UUID_LIGHT_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.addService(lightService);
    }

    /**
     * @brief Update the flex level with a new value. Valid values range from
     * 0..100. Anything outside this range will be ignored.
     *
     * @param newLevel
     *              update to flex level.
     */
    void updateLightLevel(uint8_t newLevel) {
        lightLevel = newLevel;
        ble.gattServer().write(lightLevelCharacteristic.getValueHandle(), &lightLevel, 1);
    }

protected:
    BLE &ble;

    uint8_t    lightLevel;
    ReadOnlyGattCharacteristic<uint8_t> lightLevelCharacteristic;
};

#endif 
