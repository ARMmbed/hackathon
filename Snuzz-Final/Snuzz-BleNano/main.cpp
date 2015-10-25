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
#include "ble/DiscoveredCharacteristic.h"
#include "ble/DiscoveredService.h"
#include "LPD8806/LPD8806.h"

LPD8806 leds(6);

static DiscoveredCharacteristic ledCharacteristic;

void periodicCallback(void) {
//    alivenessLED = !alivenessLED; /* Do blinky on LED1 while we're waiting for BLE events */
}

void advertisementCallback(const Gap::AdvertisementCallbackParams_t *params) {
    if (params->peerAddr[0] != 0x29) { /* !ALERT! Alter this filter to suit your device. */
        return;
    }
    printf("adv peerAddr[%02x %02x %02x %02x %02x %02x] rssi %d, isScanResponse %u, AdvertisementType %u\r\n",
           params->peerAddr[5], params->peerAddr[4], params->peerAddr[3], params->peerAddr[2], params->peerAddr[1], params->peerAddr[0],
           params->rssi, params->isScanResponse, params->type);

    BLE::Instance().gap().connect(params->peerAddr, Gap::ADDR_TYPE_RANDOM_STATIC, NULL, NULL);
}

void serviceDiscoveryCallback(const DiscoveredService *service) {
    if (service->getUUID().shortOrLong() == UUID::UUID_TYPE_SHORT) {
        printf("S UUID-%x attrs[%u %u]\r\n", service->getUUID().getShortUUID(), service->getStartHandle(), service->getEndHandle());
    } else {
        printf("S UUID-");
        const uint8_t *longUUIDBytes = service->getUUID().getBaseUUID();
        for (unsigned i = 0; i < UUID::LENGTH_OF_LONG_UUID; i++) {
            printf("%02x", longUUIDBytes[i]);
        }
        printf(" attrs[%u %u]\r\n", service->getStartHandle(), service->getEndHandle());
    }
}

void updateLedCharacteristic(void) {
    if (!BLE::Instance().gattClient().isServiceDiscoveryActive()) {
        ledCharacteristic.read();
    }
}

void characteristicDiscoveryCallback(const DiscoveredCharacteristic *characteristicP) {
    printf("  C UUID-%x valueAttr[%u] props[%x]\r\n", characteristicP->getUUID().getShortUUID(), characteristicP->getValueHandle(), (uint8_t)characteristicP->getProperties().broadcast());
    if (characteristicP->getUUID().getShortUUID() == 0xa001) { /* !ALERT! Alter this filter to suit your device. */
        ledCharacteristic        = *characteristicP;
        minar::Scheduler::postCallback(updateLedCharacteristic);
    }
}

void discoveryTerminationCallback(Gap::Handle_t connectionHandle) {
    printf("terminated SD for handle %u\r\n", connectionHandle);
}

void connectionCallback(const Gap::ConnectionCallbackParams_t *params) {
    if (params->role == Gap::CENTRAL) {
        BLE &ble = BLE::Instance();
        ble.gattClient().onServiceDiscoveryTermination(discoveryTerminationCallback);
        ble.gattClient().launchServiceDiscovery(params->handle, serviceDiscoveryCallback, characteristicDiscoveryCallback, 0xa000, 0xa001);
    }
}

void triggerToggledWrite(const GattReadCallbackParams *response) {
    if (response->handle == ledCharacteristic.getValueHandle()) {
#if DUMP_READ_DATA
        printf("triggerToggledWrite: handle %u, offset %u, len %u\r\n", response->handle, response->offset, response->len);
        for (unsigned index = 0; index < response->len; index++) {
            printf("%c[%02x]", response->data[index], response->data[index]);
        }
        printf("\r\n");
#endif

        uint8_t toggledValue = response->data[0] ^ 0x1;
        ledCharacteristic.write(1, &toggledValue);
    }
}

void triggerRead(const GattWriteCallbackParams *response) {
    if (response->handle == ledCharacteristic.getValueHandle()) {
        ledCharacteristic.read();
    }
}

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *) {
    printf("disconnected\r\n");
}

void app_start(int, char**) {
    minar::Scheduler::postCallback(periodicCallback).period(minar::milliseconds(500));
	
	leds.begin();
	
	while(1) {	
		leds.show();
		//leds.setPixelColor(1, 127,0,0);
		//leds.setPixelColor(2, 0,127,0);
		//leds.setPixelColor(3, 127,127,127);
		//int xr,yr,zr,increment,lightno,fundamentalchoice;
		//Switch options
		
		//Shut is state zero
		DigitalIn pb(P0_8);
		pb.mode(PullUp);
		//int closed=0;
		//One Program Forever
		int waitcounter=0;
		PwmOut speaker(p28);
		for (int ripple=0; ripple<6;ripple++){
			leds.setPixelColor(ripple,rand()%127,rand()%127,rand()%127);
			wait(1);leds.show(); speaker=0.0;
			if (pb==0){break;}
		}
		while (pb==0){
			if (waitcounter==0){waitcounter=1;}
			for (int counter=0;counter<6;counter++){
				leds.setPixelColor(counter,0,127,0);
			}
			wait(1);leds.show();
			if (waitcounter==1){wait(7); waitcounter=2;}
			for (int ij=0; ij<26; ij=ij+2) {
			speaker.period(1.0/969.0);
			speaker = float(ij)/50.0;
			}
			wait(.5);
		}
		/*for (int ij=0; ij<26; ij=ij+2) {
			speaker.period(1.0/969.0);
			speaker = float(ij)/50.0;
			for (int fl=0;fl<6;fl++){
				if (fl%2==0){
					leds.setPixelColor(fl,0,0,127);
				}
				if (fl%2==1){
					leds.setPixelColor(fl,127,0,0);
				}
			}
			leds.show();
			wait_ms(1);if (pb==1){speaker=0.0; break;}
			wait(.5);
			speaker.period(1.0/800.0);
			for (int fl=0;fl<6;fl++){
				if (fl%2==1){
					leds.setPixelColor(fl,0,0,127);
				}
				if (fl%2==0){
					leds.setPixelColor(fl,127,0,0);
				}
			}
			leds.show();
			wait_ms(1);if (pb==1){speaker=0.0; break;}
			wait(.5);
		}
		*/

	
	
	}
    BLE &ble = BLE::Instance();
    ble.init();
    ble.gap().onDisconnection(disconnectionCallback);
    ble.gap().onConnection(connectionCallback);

    ble.gattClient().onDataRead(triggerToggledWrite);
    ble.gattClient().onDataWrite(triggerRead);

    ble.gap().setScanParams(500, 400);
    ble.gap().startScan(advertisementCallback);
}
