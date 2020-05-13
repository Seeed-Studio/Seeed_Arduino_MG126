#include <Arduino.h>

#define USER_BUTTON 38


void setup()
{
	SerialUSB.begin(115200);
	delay(100);
	SerialUSB.println("start");
	pinMode(USER_BUTTON,INPUT);
}




void loop()
{
	
	int val = digitalRead(USER_BUTTON);
	if(0 == val){
			SerialUSB.print("Key pressed ");
	}
	SerialUSB.println(".");
	delay(1000);
	
}

