#include "mbed.h"
#include "ble/BLE.h"

//Serial pc(USBTX, USBRX);
BLE ble;
DigitalOut rightIndicatorFlash(P0_5);
DigitalOut leftIndicatorFlash(P0_4);
DigitalOut brakeIndicator(P0_7);

bool rightIndicator = 0;
bool leftIndicator = 0;


uint16_t customServiceUUID  = 0xA000;
uint16_t readCharUUID       = 0xA001;
uint16_t writeCharUUID      = 0xA002;

const static char     DEVICE_NAME[]        = "TeamAkos"; // change this
static const uint16_t uuid16_list[]        = {0xFFFF}; //Custom UUID, FFFF is reserved for development

// Set Up custom Characteristics
static uint8_t readValue[10] = {0};
ReadOnlyArrayGattCharacteristic<uint8_t, sizeof(readValue)> readChar(readCharUUID, readValue);

static uint8_t writeValue[10] = {0};
WriteOnlyArrayGattCharacteristic<uint8_t, sizeof(writeValue)> writeChar(writeCharUUID, writeValue);

// Set up custom service
GattCharacteristic *characteristics[] = {&readChar, &writeChar};
GattService        customService(customServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));


/*
 *  Restart advertising when phone app disconnects
*/
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *)
{
    ble.gap().startAdvertising();
}

/*
 *  handle writes to writeCharacteristic
*/
void writeCharCallback(const GattWriteCallbackParams *params)
{
    // check to see what characteristic was written, by handle
    if(params->handle == writeChar.getValueHandle()) {
        // toggle LED if only 1 byte is written
        if(params->len == 1) {
            //led = params->data[0];
            
            rightIndicator = ((params->data[0] & 0x01) == 0x01)? 1:0;
            rightIndicatorFlash = rightIndicator;
            
            leftIndicator = ((params->data[0] & 0x04) == 0x04)? 1:0;
            leftIndicatorFlash = leftIndicator;
            
            brakeIndicator = ((params->data[0] & 0x02) == 0x02)? 1:0;
            
            (brakeIndicator) ? printf("brake is on \n\r") : printf("brake is off \n\r");
            //(params->data[0] == 0x00) ? printf("led on\n\r") : printf("led off\n\r"); // print led toggle
            
            
        }
        // print the data if more than 1 byte is written
        else {
            printf("Data received: length = %d, data = 0x",params->len);
            for(int x=0; x < params->len; x++) {
                printf("%x", params->data[x]);
            }
            printf("\n\r");
        }
        // update the readChar with the value of writeChar
        ble.updateCharacteristicValue(readChar.getValueHandle(), params->data,params->len);
    }
}

static void toggleLed()
{
    
    if(leftIndicator) 
    {
        leftIndicatorFlash = !leftIndicatorFlash;
        printf("left led toggle\n\r");
    }
    if(rightIndicator) 
    {
        rightIndicatorFlash = !rightIndicatorFlash;
        printf("right led toggle\n\r");
    }
}

/*
 *  main loop
*/
void app_start(int, char**)
{
   //  pc.baud(115200);
    /* initialize stuff */
    printf("\n\r********* Starting Main Loop *********\n\r");
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

    minar::Scheduler::postCallback(toggleLed).period(minar::milliseconds(500));
}
