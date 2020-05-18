#include <Arduino.h>
#include "MG126_Ble.h"
#include "SPI.h"
#include "Seeed_SHT35.h"


#define SCLPIN  21

SHT35 sht3x(SCLPIN);

#define CS_PIN 47
#define IRQ_PIN 7

/**
 *  * description:
 * The log info print to SerialUSB
 * The functionality implemented by this example:
 *   Connect a sht31(if sht35,only should change the i2c address) to wio-lite-ble,The app grabs data from sht31 in real time.
 * 
* Operating procedures : 
 * 1.download the example
 * 2.Open the "nRF Connet"(an popular bluetooth app developed by nordic)，
 *   Search the bluetooth name which specified in file app.hogp.c:DeviceInfo[].
 * 3.Clean the cache of the "nRF Connet" App's cache data when you have modified the bluetooth example,
 *   Usually you should reboot the phone.Because you have modified the character of bluetooth.
 * 4.Connect bluetooth.
 * 5.Click Environmental Sensing in bluetooth connection interface.  
 * 6.In the second level interface, Intermediate Temperature, the first icon in the upper right corner is to get a temperature value, the second icon
      indicates the listening data, 
      Humidity, the first icon in the upper right corner is to get the humidity value, and the second icon is the listening data.
 * 7.The data is displayed in the value part in hex form, for example: 0x32372E3635 corresponds to 27.65.
 * 
 * */


T_H_val_t  T_H_data;
extern uint16_t cur_notifyhandle;

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

	if(sht3x.init())
	{
		SERIAL.println("sensor init failed!!!");
		//while(1);
	}
	
}




void loop() {
	float temp,hum;
	// char temp_str[10] = {0};
	// char hum_str[10] = {0};
	String temp_str;
	String hum_str;
	if(NO_ERROR!=sht3x.read_meas_data_single_shot(HIGH_REP_WITH_STRCH,&temp,&hum))
	{
		SERIAL.println("read temp failed!!");
		SERIAL.println("   ");
		SERIAL.println("   ");
		SERIAL.println("   ");
		
	}
	temp_str += temp; 
	hum_str += hum; 
	
	

	cur_notifyhandle = 0x12;
	sconn_notifydata((uint8_t*)temp_str.c_str(),strlen(temp_str.c_str()));
	delay(10);
	cur_notifyhandle = 0x15;
	sconn_notifydata((uint8_t*)hum_str.c_str(),strlen(hum_str.c_str()));
	delay(1000);

	memcpy(T_H_data.temp_str,temp_str.c_str(),strlen(temp_str.c_str()));
	memcpy(T_H_data.hum_str,hum_str.c_str(),strlen(hum_str.c_str()));

	SERIAL.println(T_H_data.temp_str);
	SERIAL.println(T_H_data.hum_str);
}