#include <Arduino.h>
#include "MG126_Ble.h"
#include "SPI.h"
//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio



/**
 * 
 * description:
 * The log info print to SerialUSB
 * The functionality implemented by this example:
 *   Serial transparent transport。Use phone send data to wio-lite-ble board,Then the board
 *   print the data to serial1.  
 *   user send data to wio-lite-ble board via serial1,the data will send to user's phone.

 * Operating procedures : 
 * 1.download the example
 * 2.Open the "nRF Connet"(an popular bluetooth app developed by nordic)，
 *   Search the bluetooth name which specified in file app.hogp.c:DeviceInfo[].
 * 3.Clean the cache of the "nRF Connet" App's cache data when you have modified the bluetooth example,
 *   Usually you should reboot the phone.Because you have modified the character of bluetooth.
 * 4.Connect bluetooth.
 * 5.Click Unknown Serivice in bluetooth connection interface.  
 * */

#define USER_SERIAL  Serial1


#define CS_PIN 47
#define IRQ_PIN 7

extern ble_trans_t recv_struct;

MG126_Ble_Class MG126_Ble(CS_PIN,IRQ_PIN);

void deal_ble_data(void)
{
	if(recv_struct.recv_len)
	{
		for(int i=0;i<recv_struct.recv_len;i++)
		{
			USER_SERIAL.write(recv_struct.buf[i]);
		}
		recv_struct.recv_len = 0;
	}
}


void send_to_ble(void)
{
	int recv_len = 0;
	uint8_t buf[100];
	if(USER_SERIAL.available())
	{
		delay(10);
		recv_len = USER_SERIAL.available();
		for(int i=0;i<recv_len;i++)
		{																																																																																																																																																																																																											
			buf[i] = USER_SERIAL.read();
		}
		sconn_notifydata(buf,recv_len);
	}

}



void setup() {

	USER_SERIAL.begin(115200);
	SERIAL.begin(115200);
	// while (!SERIAL);
	SERIAL.println("\n");
	SERIAL.println("configure Ble  ...");
	MG126_Ble.ble_init();	
}

void loop() {

	deal_ble_data();
	send_to_ble();
}