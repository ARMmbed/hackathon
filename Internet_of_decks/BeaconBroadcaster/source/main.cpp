#include "mbed.h"
#include "ble/BLE.h"
#include "ble_radio_notification.h"

Serial pc(USBTX, USBRX);

DigitalOut led(LED1);
InterruptIn button(D3);
Timer timer;
BLE ble;

uint16_t offset = 10;
uint32_t ms_diff = 0;
uint16_t bpm = 120;

uint8_t data[25];

uint8_t c;

uint8_t serstate = 0;

uint8_t inp[4];

void serialinterrupt(){
	inp[serstate++] = pc.getc();
	if(serstate==4){
		timer.reset();
		timer.start();
		
		offset = (inp[0] << 8) | inp[1];
		bpm = (inp[2] << 8) | inp[3];
		
		serstate = 0;
		pc.putc('k');
	}
}

void setnewbeacon(){
	ble.gap().stopAdvertising();
	
	const uint8_t uuid[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
	                        0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61};
	uint16_t majorNumber = bpm & 0xffff;
	uint16_t minorNumber = ((offset-ms_diff) % (60000/bpm)) & 0xffff;
	uint16_t txPower     = 0xC8;
	
    data[0] = 0x00;
    data[1] = 0x4C;
    data[2] = 0x02;
    data[3] = 0x15;
    int i;
    for(i=0;i<16;i++){
    	data[4+i] = uuid[i];
    }
    data[20] = (majorNumber >> 8) & 0xff;
    data[21] = majorNumber & 0xff;
    data[22] = (minorNumber >> 8) & 0xff;
    data[23] = minorNumber & 0xff;
    data[24] = txPower;
    
    GapAdvertisingData d = ble.gap().getAdvertisingPayload();
    
    d.updateData(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, data, 25);
    
    ble.gap().setAdvertisingPayload(d);
    
    ble.gap().startAdvertising();
}

void blinky(bool active){
	timer.stop();
	ms_diff = timer.read_ms();
	
	if(active)
		led = !led;
}

void buttonPressedCallback(){
	timer.reset();
	timer.start();
	led = !led;
}

void app_start(int, char**){
	button.fall(buttonPressedCallback);
	button.mode(PullUp);
	
	pc.baud(9600);
	
	ble.init();
	
	int ret = ble_radio_notification_init(NRF_APP_PRIORITY_HIGH, NRF_RADIO_NOTIFICATION_DISTANCE_800US, blinky);
	
	ble.gap().stopAdvertising();
	ble.gap().clearAdvertisingPayload();
	
	const uint8_t uuid[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
	                        0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61};
	uint16_t majorNumber = 0;
	uint16_t minorNumber = 0;
	uint16_t txPower     = 0xC8;
    
    data[0] = 0x00;
    data[1] = 0x4C;
    data[2] = 0x02;
    data[3] = 0x15;
    int i;
    for(i=0;i<16;i++){
    	data[4+i] = uuid[i];
    }
    data[20] = (majorNumber >> 8) & 0xff;
    data[21] = majorNumber & 0xff;
    data[22] = (minorNumber >> 8) & 0xff;
    data[23] = minorNumber & 0xff;
    data[24] = txPower;
     
	// Generate the 0x020106 part of the iBeacon Prefix
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE );
    // Generate the 0x1AFF part of the iBeacon Prefix
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, data, 25);
    // Set advertising type
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED);
    
    ble.gap().setAdvertisingInterval(1000);
    ble.gap().startAdvertising();
	
	pc.attach(serialinterrupt);
	
	minar::Scheduler::postCallback(setnewbeacon).period(minar::milliseconds(500));
}
