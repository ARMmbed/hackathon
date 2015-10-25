/* mbed Microcontroller Library
 *
 * Modifications by BAB, OJD, OLC, SG.
 *
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

#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/HealthThermometerService.h"

DigitalOut led1(LED1, 1);
AnalogIn strip_vol(A3);
DigitalOut v_high(P0_10, 1);
DigitalOut speaker(P0_9, 0);
InterruptIn button(P0_11);

const static char     DEVICE_NAME[]        = "Pemgineers";
static const uint16_t uuid16_list[]        = {GattService::UUID_HEALTH_THERMOMETER_SERVICE};

static float                     currentTemperature[10] = { 40, 40, 40, 40, 40, 40, 40, 40, 40, 40 };
static HealthThermometerService *thermometerServicePtr;
static float threshold = 30;
bool slouching = 0;

/* Restart Advertising on disconnection*/
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *)
{
	BLE::Instance().gap().startAdvertising();
}

void updateSensorValue(void) {
    /* Do blocking calls or whatever is necessary for sensor polling.
       In our case, we simply update the Temperature measurement. */
	currentTemperature[9] = currentTemperature[8];

	for (int i = 9; i > 0; i--) {
		currentTemperature[i] = currentTemperature[i - 1];
	}
    currentTemperature[0] = ((strip_vol - 0.48) * 3) * 100; // 0.45 to 0.8 with 20k resistor
}

void periodicCallback(void)
{
	// get sensor value every x milliseconds
	minar::Scheduler::postCallback(updateSensorValue);
	// now check if whole of array is below threshold 
	if (slouching == 0) {
		bool temp_slouching = 0;
		for (int i = 0; i < 2; i++) {
			if (currentTemperature[i] < threshold) {
				temp_slouching = 1;
			}
			else {
				temp_slouching = 0;
			}
		}
		if (temp_slouching == 1) {
			slouching = 1;
		}
	}

	if (slouching == 1) {
		bool temp_slouching = 1;
		for (int i = 0; i < 2; i++) {
			if (currentTemperature[i] > threshold) {
				temp_slouching = 0;
			}
			else {
				temp_slouching = 1;
			}
		}
		if (temp_slouching == 0) {
			slouching = 0;
		}
	}

	if (slouching == 1) {
		for (int i = 0; i < 200; i++) {
			speaker = !speaker;
			wait(1.0 / 400);
		}
	}
	thermometerServicePtr->updateTemperature(slouching);
}

float previous_baseline = 30;
float current_baseline = 30;
void button_down() {
	current_baseline= currentTemperature[0];
	threshold = (current_baseline + previous_baseline) / 2;
	previous_baseline = current_baseline;

}

void app_start(int, char**)
{
    minar::Scheduler::postCallback(periodicCallback).period(minar::milliseconds(500));
    BLE &ble = BLE::Instance();
    ble.init();
    ble.gap().onDisconnection(disconnectionCallback);
	button.fall(&button_down);
    /* Setup primary service. */
    thermometerServicePtr = new HealthThermometerService(ble, currentTemperature[0], HealthThermometerService::LOCATION_EAR);
    /* setup advertising */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::THERMOMETER_EAR);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms */
    ble.gap().startAdvertising();
}
