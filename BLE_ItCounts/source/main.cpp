/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
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

#include "mbed.h"
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "ble/services/FlexService.h"
#include "ble/services/LightService.h"

DigitalOut led1(LED1, 1);
DigitalOut actuatedLED(D6);
AnalogIn flexIn(A3);
AnalogIn lgtIn(A4);

const static char     DEVICE_NAME[] = "ItCounts";
static const uint16_t uuid16_list[] = {GattService::UUID_FLEX_SERVICE};

#define LED_SERVICE_UUID 0x1822
#define LED_STATE_CHARACTERISTIC_UUID 0xA001

bool initialValueForLEDCharacteristic = false;
ReadWriteGattCharacteristic<bool> ledState(LED_STATE_CHARACTERISTIC_UUID,
										&initialValueForLEDCharacteristic);

static uint8_t flexValue = 100;
static uint8_t lgtValue = 100;
static FlexService *flexServicePtr;
static LightService *lgtServicePtr;

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    BLE::Instance().gap().startAdvertising(); // restart advertising
}

void updateSensorValue() {

    flexValue = (uint8_t)flexIn.read_u16();
	lgtValue = (uint8_t)lgtIn.read_u16();
    flexServicePtr->updateFlexValue(flexValue);
	lgtServicePtr->updateLightLevel(lgtValue);
}

void periodicCallback(void)
{
    led1 = !led1; /* Do blinky on LED1 while we're waiting for BLE events */
    if (BLE::Instance().getGapState().connected) {
        updateSensorValue();
    }
	else {
		actuatedLED = 0;
	}
}

void onDataWrittenCallback(const GattWriteCallbackParams *params)
{
	if ((params->handle == ledState.getValueHandle()) && 
            (params->len == 1)) {
        actuatedLED = *(params->data); 
        /* we interpret the 1-byte data
        * received from the
        * callbackParams as a value to
        * be written to the
        * LED. Writing anything
        * non-zero will turn on the
        * LED; otherwise it will be
        * reset to off */
        }
}

void app_start(int argc, char *argv[])
{
    minar::Scheduler::postCallback(periodicCallback).period(minar::milliseconds(500));

    BLE &ble = BLE::Instance();
    ble.init();
    ble.gap().onDisconnection(disconnectionCallback);
	ble.onDataWritten(onDataWrittenCallback);

    /* Setup primary service. */
    flexServicePtr = new FlexService(ble, flexValue, FlexService::LOCATION_FINGER);
	lgtServicePtr = new LightService(ble, lgtValue);
	GattCharacteristic *charTable[] = {&ledState};
	GattService	ledService(LED_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
	ble.addService(ledService);

    /* Setup advertising. */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms */
    ble.gap().startAdvertising();
}
