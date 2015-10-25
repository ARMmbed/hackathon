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

#ifndef __BLE_FLEX_SERVICE_H__
#define __BLE_FLEX_SERVICE_H__

#include "ble/BLE.h"

/**
* @class FlexService
* @brief BLE Service for FlexSensor. This BLE Service contains the location of the sensor, the flex level. <br>
*/
class FlexService {
public:
    /**
    * @enum SensorLocation
    * @brief Location of Flex sensor on body.
    */
    enum {
        LOCATION_OTHER = 0, /*!< Other Location */
        LOCATION_CHEST,     /*!< Chest */
        LOCATION_WRIST,     /*!< Wrist */
        LOCATION_FINGER,    /*!< Finger */
        LOCATION_HAND,      /*!< Hand */
        LOCATION_EAR_LOBE,  /*!< Earlobe */
        LOCATION_FOOT,      /*!< Foot */
    };

public:
    /**
     * @brief Constructor with 8bit flex value.
     *
     * @param[ref] _ble
     *               Reference to the underlying BLE.
     * @param[in] flexValue (8-bit)
     *               initial value for the flex.
     * @param[in] location
     *               Sensor's location.
     */
    FlexService(BLE &_ble, uint8_t flexValue, uint8_t location) :
        ble(_ble),
        valueBytes(flexValue),
        flexLevelChar(GattCharacteristic::UUID_FLEX_MEASUREMENT_CHAR, valueBytes.getPointer(),
                valueBytes.getNumValueBytes(), FlexValueBytes::MAX_VALUE_BYTES,
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        flexLocation(GattCharacteristic::UUID_BODY_SENSOR_LOCATION_CHAR, &location),
        controlPoint(GattCharacteristic::UUID_FLEX_CONTROL_POINT_CHAR, &controlPointValue) {
        setupService();
    }

    /**
     * @brief Constructor with a 16-bit HRM Counter value.
     *
     * @param[in] _ble
     *               Reference to the underlying BLE.
     * @param[in] flexValue (8-bit)
     *               initial value for the flex.
     * @param[in] location
     *               Sensor's location.
     */
    FlexService(BLE &_ble, uint16_t flexValue, uint8_t location) :
        ble(_ble),
        valueBytes(flexValue),
        flexLevelChar(GattCharacteristic::UUID_FLEX_MEASUREMENT_CHAR, valueBytes.getPointer(),
                valueBytes.getNumValueBytes(), FlexValueBytes::MAX_VALUE_BYTES,
                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY),
        flexLocation(GattCharacteristic::UUID_BODY_SENSOR_LOCATION_CHAR, &location),
        controlPoint(GattCharacteristic::UUID_FLEX_CONTROL_POINT_CHAR, &controlPointValue) {
        setupService();
    }

    /**
     * @brief Set a new 8-bit value for flex.
     *
     * @param[in] flexValue
     */
    void updateFlexValue(uint8_t flexValue) {
        valueBytes.updateFlexValue(flexValue);
        ble.gattServer().write(flexLevelChar.getValueHandle(), valueBytes.getPointer(), valueBytes.getNumValueBytes());
    }

    /**
     * Set a new 16-bit value for flex.
     *
     * @param[in] flexValue
     */
    void updateFlexValue(uint16_t flexValue) {
        valueBytes.updateFlexValue(flexValue);
        ble.gattServer().write(flexLevelChar.getValueHandle(), valueBytes.getPointer(), valueBytes.getNumValueBytes());
    }

    /**
     * This callback allows the FlexService to receive updates to the
     * controlPoint Characteristic.
     *
     * @param[in] params
     *     Information about the characterisitc being updated.
     */
    virtual void onDataWritten(const GattWriteCallbackParams *params) {
        if (params->handle == controlPoint.getValueAttribute().getHandle()) {
            /* Do something here if the new value is 1; else you can override this method by
             * extending this class.
             * @NOTE: if you are extending this class, be sure to also call
             * ble.onDataWritten(this, &ExtendedHRService::onDataWritten); in
             * your constructor.
             */
        }
    }

protected:
    void setupService(void) {
        GattCharacteristic *charTable[] = {&flexLevelChar, &flexLocation, &controlPoint};
        GattService         flexService(GattService::UUID_FLEX_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));

        ble.addService(flexService);
        ble.onDataWritten(this, &FlexService::onDataWritten);
    }

protected:
    /* Private internal representation for the bytes used to work with the value of the flex characteristic. */
    struct FlexValueBytes {
        static const unsigned MAX_VALUE_BYTES  = 3; /* FLAGS + up to two bytes for flex value */
        static const unsigned FLAGS_BYTE_INDEX = 0;

        static const unsigned VALUE_FORMAT_BITNUM = 0;
        static const uint8_t  VALUE_FORMAT_FLAG   = (1 << VALUE_FORMAT_BITNUM);

        FlexValueBytes(uint8_t flexValue) : valueBytes() {
            updateFlexValue(flexValue);
        }

        FlexValueBytes(uint16_t flexValue) : valueBytes() {
            updateFlexValue(flexValue);
        }

        void updateFlexValue(uint8_t flexValue) {
            valueBytes[FLAGS_BYTE_INDEX]    &= ~VALUE_FORMAT_FLAG;
            valueBytes[FLAGS_BYTE_INDEX + 1] = flexValue;
        }

        void updateFlexValue(uint16_t flexValue) {
            valueBytes[FLAGS_BYTE_INDEX]    |= VALUE_FORMAT_FLAG;
            valueBytes[FLAGS_BYTE_INDEX + 1] = (uint8_t)(flexValue & 0xFF);
            valueBytes[FLAGS_BYTE_INDEX + 2] = (uint8_t)(flexValue >> 8);
        }

        uint8_t       *getPointer(void) {
            return valueBytes;
        }

        const uint8_t *getPointer(void) const {
            return valueBytes;
        }

        unsigned       getNumValueBytes(void) const {
            return 1 + ((valueBytes[FLAGS_BYTE_INDEX] & VALUE_FORMAT_FLAG) ? sizeof(uint16_t) : sizeof(uint8_t));
        }

    private:
        uint8_t valueBytes[MAX_VALUE_BYTES];
    };

protected:
    BLE                 &ble;

    FlexValueBytes		valueBytes;
    uint8_t             controlPointValue;

    GattCharacteristic                   flexLevelChar;
    ReadOnlyGattCharacteristic<uint8_t>  flexLocation;
    WriteOnlyGattCharacteristic<uint8_t> controlPoint;
};

#endif /* #ifndef __BLE_FLEX_SERVICE_H__*/
