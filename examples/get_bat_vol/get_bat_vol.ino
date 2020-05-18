#include <Arduino.h>

#define CTRL_PIN      25
#define VOL_PIN       8

/*
	Notice:	In this case,the Wio lite board is powered with battery instead of Micro-USB cable,
	So we use the Serial1 to output log msgs.
*/
#define SERIAL  SerialUSB

/*The ADC comparator voltage*/
#define SYS_VOL   3.3
/*The resolution of ADC*/
#define ADC_RES   1024

void setup()
{
		SERIAL.begin(115200);
		delay(100);
		SERIAL.println("start");
		pinMode(CTRL_PIN,OUTPUT);
		digitalWrite(CTRL_PIN,0);
}




void loop()
{
	float voltage = 0;
	int val = analogRead(VOL_PIN);
	SERIAL.println(val);
	
	voltage = SYS_VOL *  val * 2 / ADC_RES;
	
	SERIAL.print("The battery voltage is ");
	SERIAL.println(voltage);
	SERIAL.println("");
	delay(1000);
	
}

