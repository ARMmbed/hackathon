#include "mbed.h"
#include "ble/BLE.h"

/* Declare our variables here */

BLE ble;

DigitalOut status(LED1, 1);
DigitalOut light(P0_7);
DigitalIn lid(P0_15);

AnalogIn sensor_upper(P0_4);
AnalogIn sensor_lower(P0_5);
AnalogIn sensor_flex(P0_6);

Ticker tick;

uint16_t customServiceUUID  = 0xA000;
uint16_t readCharUUID       = 0xA001;

const static char     DEVICE_NAME[]        = "IoBins"; // change this
static const uint16_t uuid16_list[]        = {0xFFFF}; //Custom UUID, FFFF is reserved for development

// Set Up custom Characteristics
 
uint8_t readValue[5] = {0};
ReadOnlyArrayGattCharacteristic<uint8_t, sizeof(readValue)> readChar(readCharUUID, readValue);

// Set up custom service
GattCharacteristic *characteristics[] = {&readChar};
GattService        customService(customServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));


/*
 *  Restart advertising when phone app disconnects
*/
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *)
{
    ble.gap().startAdvertising();
}

/* Data packet:
0: top light
1: bottom light
2: flex, MSBs
3: flex, LSBs
4: seq_no
*/

void sensorTick(void)
{
    status = !status;
    
    light = 1;
    
    wait(0.002);// CdS sensors, wait to settle. (In interrupt context ftw)
    
    readValue[0] = sensor_upper.read() * 256;
    readValue[1] = sensor_lower.read() * 256;  // 0 <= x < 1  ; will be in range 0 -> 255.
    
    int n_flex_reads = 16; // oversample to increase resolution (this should add ~2 bits extra precision)
    float flex_val = 0;
    for (int i = 0; i < n_flex_reads; ++i)
        flex_val += sensor_flex.read();
    flex_val *= 255.f / (float)n_flex_reads;
    flex_val = (flex_val - 95.f) * 60.f; // magic numbers, code won't work without
    readValue[2] = flex_val / 256;
    readValue[3] = flex_val;

    light = 0;
    
    readValue[4]++;
    
    ble.updateCharacteristicValue(readChar.getValueHandle(), readValue, sizeof(readValue) / sizeof(*readValue));
}

/*
 *  main loop
*/
int main(int argc, char *argv[])
{
    /* initialize stuff */
    printf("\n\r********* Starting Main Loop *********\n\r");

    ble.init();
    ble.gap().onDisconnection(disconnectionCallback);
    
    tick.attach(&sensorTick, 2.0);

    /* setup advertising */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE); // BLE only, no classic BT
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED); // advertising type
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME)); // add name
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list)); // UUID's broadcast in advertising packet
    ble.gap().setAdvertisingInterval(1000); // 1000ms.

    // add our custom service
    ble.addService(customService);

    // start advertising
    ble.gap().startAdvertising();

    // infinite loop waiting for BLE interrupt events
    while (true) {
        ble.waitForEvent(); //Save power
    }
}