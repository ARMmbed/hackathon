#include "mbed.h"
class LPD8806 {

public:

	LPD8806(uint16_t n); // Configurable pins
	LPD8806(void); // Empty constructor; init pins & strip length later
	void begin(void);
	void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
	void setPixelColor(uint16_t n, uint32_t c);
	void show(void);
	void updatePins(uint8_t dpin, uint8_t cpin); // Change pins, configurable
	void updateLength(uint16_t n);               // Change strip length
	uint16_t numPixels(void);
	uint32_t Color(uint8_t, uint8_t, uint8_t);
	uint32_t getPixelColor(uint16_t n);

private:

	uint16_t
	numLEDs,    // Number of RGB LEDs in strip
	numBytes;   // Size of 'pixels' buffer below
	uint8_t
	*pixels;    // Holds LED color values (3 bytes each) + latch bytes

	void startBitbang(void);

	bool 
	hardwareSPI, // If 'true', using hardware SPI
	begun;       // If 'true', begin() method was previously invoked
};
