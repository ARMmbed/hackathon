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
#include "LPD8806/LPD8806.h"

LPD8806 leds(6);

BLEDevice ble;
DigitalOut led(LED1);
uint16_t customServiceUUID  = 0xA000;
uint16_t readCharUUID       = 0xA001;
uint16_t writeCharUUID      = 0xA002;

const static char     DEVICE_NAME[]        = "CJB123"; // change this
static const uint16_t uuid16_list[]        = {0xFFFF}; //Custom UUID, FFFF is reserved for development

// Set Up custom Characteristics
static uint8_t readValue[10] = {0};
ReadOnlyArrayGattCharacteristic<uint8_t, sizeof(readValue)> readChar(readCharUUID, readValue);

static uint8_t writeValue[10] = {0};
WriteOnlyArrayGattCharacteristic<uint8_t, sizeof(writeValue)> writeChar(writeCharUUID, writeValue);

// Set up custom service
GattCharacteristic *characteristics[] = {&readChar, &writeChar};
GattService        customService(customServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));




void periodicCallback(void) {

}

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *) {
    printf("disconnected\r\n");
	
    ble.gap().startAdvertising();
}

void scroll_lights(int loops, int r, int g, int b) { // loops= number of times it scrolls through, r,g,b= intensity of each colour 0-127
			for (int LED_number=0; LED_number<7; LED_number++){
				leds.setPixelColor(LED_number, 0, 0 ,0);
				}
				
			for (int loop_number=0; loop_number<loops; loop_number++){
				for (int LED_number=0; LED_number<7; LED_number++){
				leds.setPixelColor(LED_number, r, g ,b);
				leds.show();wait_ms(100);
				leds.setPixelColor(LED_number, 0, 0 ,0);
				}
			}
}
	
void flash_lights(int flashes, int r, int g, int b) { // loops= number of times it scrolls through, r,g,b= intensity of each colour 0-127
						
			for (int flash_number=0; flash_number<flashes; flash_number++){
				for (int LED_number=0; LED_number<7; LED_number++){
				leds.setPixelColor(LED_number, r, g ,b);				
				}
				leds.show();wait_ms(150);
				for (int LED_number=0; LED_number<7; LED_number++){
				leds.setPixelColor(LED_number, 0, 0 ,0);
				}
				leds.show();wait_ms(150);
			}
}
	
void buzzer_tone(int time, float freq){ 
	PwmOut speaker(p28);
		int i;
			// generate a (freq)Hz tone using PWM hardware output
			speaker.period(1.0/freq); 
			speaker =0.5; //50% duty cycle - max volume
			wait(time);
			speaker=0.0;
}


/*
 *  handle writes to writeCharacteristic
*/
void writeCharCallback(const GattWriteCallbackParams *params)
{
	
	led = !led;

    // check to see what characteristic was written, by handle
    if(params->handle == writeChar.getValueHandle()) {
        // toggle LED if only 1 byte is written
        if(params->len == 1) {
            led = params->data[0];
            (params->data[0] == 0x00) ? printf("\n\rled on ") : printf("\n\rled off "); // print led toggle
        }
        // print the data if more than 1 byte is written
        else {
            printf("\n\r Data received: length = %d, data = 0x",params->len);
            for(int x=0; x < params->len; x++) {
                printf("%x",params->data[x]);
            }
        }
        // update the readChar with the value of writeChar
        ble.updateCharacteristicValue(readChar.getValueHandle(),params->data,params->len);
    }
}


void app_start(int, char**) {
    minar::Scheduler::postCallback(periodicCallback).period(minar::milliseconds(500));
	
	leds.begin();
	
	/* initialize stuff */
    ble.init();
    ble.gap().onDisconnection(disconnectionCallback);
    ble.gattServer().onDataWritten(writeCharCallback);

    /* setup advertising */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE); // BLE only, no classic BT
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED); // advertising type
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME)); // add name
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list)); // UUID's broadcast in advertising packet
    ble.gap().setAdvertisingInterval(100); // 100ms.

    // add our custom service
    ble.addService(customService);

    // start advertising
    ble.gap().startAdvertising();
	
	
	
}
