#include <Arduino.h>
#include <MG126_Ble.h>
#include "SPI.h"


/**
 * description:
 * The log info print to SerialUSB
 * The functionality implemented by this example:
 *   The bluetooth device as a echo server.
 * 
 * Operating procedures : 
 * 1.download the example
 * 2.Open the "nRF Connet"(an popular bluetooth app developed by nordic)，
 *   Search the bluetooth name which specified in file app.hogp.c:DeviceInfo[].
 * 3.Clean the cache of the "nRF Connet" App's cache data when you have modified the bluetooth example,
 *   Usually you should reboot the phone.Because you have modified the character of bluetooth.
 * 4.Connect bluetooth.
 * 5.Click Nordic UART Service in bluetooth connection interface.
 * 6.In the second level interface, Nordic UART TX, click the notify icon in the upper right corner,
 *   and the serial port will display the value when it receives the data.
 * 7.Nordic UART RX，Click the upload icon in the top right corner to send data to Bluetooth
 * 8.In this example, Bluetooth will return the user to send past data.
 * 9.The data is displayed in the value part in hex format, for example: 0x313233 corresponds to 123.
 * 
 * */



extern uint32_t SysTick_Count;
extern uint32_t TIMER1_Count ;
extern uint32_t ext_count;

#define CS_PIN 47
#define IRQ_PIN 7


MG126_Ble_Class MG126_Ble(CS_PIN,IRQ_PIN);
//Ble Shutter example
void setup() {
	//char byte;
	// put your setup code here, to run once:
	SERIAL.begin(115200);
	// while (!SERIAL);
	SERIAL.println("\n");
	SERIAL.println("configure Ble  ...");

	MG126_Ble.ble_init();
	
	
	
}

void loop() {
	
	uint32_t mili;
	delay(1000);
}