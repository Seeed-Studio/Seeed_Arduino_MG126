#include <Arduino.h>
#include "MG126_Ble.h"
#include "SPI.h"
#include "grove_two_rgb_led_matrix.h"


/**
 * description:
 * The log info print to SerialUSB
 * The functionality implemented by this example:
 *   Connect a rgb-led-matrix module(https://www.seeedstudio.com/Grove-RGB-LED-Matrix-w-Driver.html)
 *   to Wio lite ble's I2C port,So that the phone can control the rgb modules via bluetooth App。
 * 
 * Operating procedures : 
 * 1.download the example
 * 2.Open the "nRF Connet"(an popular bluetooth app developed by nordic)，
 *   Search the bluetooth name which specified in file app.hogp.c:DeviceInfo[].
 * 3.Clean the cache of the "nRF Connet" App's cache data when you have modified the bluetooth example,
 *   Usually you should reboot the phone.Because you have modified the character of bluetooth.
 * 4.Connect bluetooth.
 * 5.Click Unknown Serivice in bluetooth connection interface.
 * 6.In the second level interface, Serivice Characteristic, click on the first icon in the upper right corner to control the led matrix.
 * 7.There are four forms of control: display num, string, graph, block, respectively. The format is: num:123/str:123/graph:2/block:00ff00(rgb in the form of hex)
 * 
 * */


#define CS_PIN 47
#define IRQ_PIN 7

#define CMD_STR_CNT      4
extern ble_trans_t ble_recv_data;

const char *func_str[CMD_STR_CNT] = {"num:","str:","graph:","block:"};

#define DISPLAY_COLOR 0x33

MG126_Ble_Class MG126_Ble(CS_PIN,IRQ_PIN);
GroveTwoRGBLedMatrixClass matrix;
void setup() 
{

	SERIAL.begin(115200);
        delay(1000);
        SERIAL.println("\n");
	SERIAL.println("configure Ble  ...");
        MG126_Ble.ble_init();

        Wire.begin();
        uint16_t VID = 0;
        VID = matrix.getDeviceVID();
        SERIAL.println(VID);
        if(VID != 0x2886)
        {       
                SERIAL.println("Can not detect led matrix!!!");
                while(1);
        }
	
}


void display_num(char* str)
{
        char *tmp = str;
        while(*tmp++ != ':');
        uint32_t num = strtoul(tmp,NULL,10);
        matrix.displayNumber(num,5000,true,DISPLAY_COLOR);
}

void display_string(char* str)
{
        char *tmp = str;
        while(*tmp++ != ':');
        matrix.displayString(tmp,10000,true,DISPLAY_COLOR);
}

void display_graph(char* str)
{
        char *tmp = str;
        while(*tmp++ != ':');   
        uint8_t num = *tmp - 0x30;
        matrix.displayColorAnimation(num,5000,true);
}

void display_block(char* str)
{
        char *tmp = str;
        while(*tmp++ != ':');
        uint32_t num = strtoul(tmp,NULL,16);
        matrix.displayColorBlock(num,0,true);
}

char deal_ble_data(void)
{
        
        if(ble_recv_data.recv_len)
        {
                for(int i=0;i<CMD_STR_CNT;i++)
                {
                        if(strstr((const char*)ble_recv_data.buf,func_str[i]))
                        {
                                switch(i)
                                {
                                        case 0:
                                        display_num((char*)ble_recv_data.buf);
                                        ble_recv_data.recv_len = 0;
                                        sconn_notifydata((unsigned char*)"num",strlen("num"));
                                        return 1;
                                        break;
                                        
                                        case 1:
                                        display_string((char*)ble_recv_data.buf);
                                        ble_recv_data.recv_len = 0;
                                        sconn_notifydata((unsigned char*)"string",strlen("string"));
                                        // SERIAL.println("string");
                                        return 1;
                                        break;
                                        
                                        case 2:
                                        display_graph((char*)ble_recv_data.buf);
                                        ble_recv_data.recv_len = 0;
                                        sconn_notifydata((unsigned char*)"graph",strlen("graph"));
                                        SERIAL.println("graph");
                                        return 1;
                                        break;
                                        
                                        case 3:
                                        display_block((char*)ble_recv_data.buf);
                                        ble_recv_data.recv_len = 0;
                                        sconn_notifydata((unsigned char*)"block",strlen("block"));
                                        SERIAL.println("block");
                                        return 1;
                                        break;

                                        default:
                                        ble_recv_data.recv_len = 0;
                                        return 0;
                                        break;
                                }
                                break;
                        }
                }
                
        }
        return 0;
}

void loop() 
{
        deal_ble_data();
}