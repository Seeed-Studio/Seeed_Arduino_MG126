#include <Arduino.h>
#include "MG126_Ble.h"
#include "SPI.h"

/** 
 * description:
 * The log info print to SerialUSB
 * The functionality implemented by this example:
 *   Using a analog device connect to A0,The mobile phone 
 *   can monitor the analog device data in real time via Bluetooth.
 * 
 * Operating procedures : 
 * 1.download the example
 * 2.Open the "nRF Connet"(an popular bluetooth app developed by nordic)，
 *   Search the bluetooth name which specified in file app.hogp.c:DeviceInfo[].
 * 3.Clean the cache of the "nRF Connet" App's cache data when you have modified the bluetooth example,
 *   Usually you should reboot the phone.Because you have modified the character of bluetooth.
 * 4.Connect bluetooth.
 * 5.Connect the annalog device.
 * 6.Click Automation IO on the connected device interface and go to the next level interface. Click the icon 
 *   in the upper right corner. The first one is to obtain an analog value, and the second is to continuously receive the analog value.
 * 7.The analog value is displayed in the hexadecimal form at the value, for example: 0x313233, and the corresponding analog value data is 123.。
 * 
 * */


/** APP : nRF Connet
 *  Notice : when user use the nRF Connet APP,it will cache the data of BLE module,like service/characteristic information.
 *  
 * 
 * */


#define CS_PIN 47
#define IRQ_PIN 7


#define ANALOG_PIN   A0

char analog_str[10];

MG126_Ble_Class MG126_Ble(CS_PIN,IRQ_PIN);

void setup() {

	SERIAL.begin(115200);
	SERIAL.println("\n");
	SERIAL.println("configure Ble  ...");
	MG126_Ble.ble_init();
}



void loop() 
{
	uint16_t val;
	val = analogRead(ANALOG_PIN);
	// SERIAL.println(val);
	sprintf(analog_str,"%d",val);
	// SERIAL.println((const char*)analog_str);

	sconn_notifydata((uint8_t*)analog_str,strlen(analog_str));
	delay(1000);
}