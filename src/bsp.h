#ifndef __BSP_H
#define __BSP_H
#include "Arduino.h"
#include "spi_wrapper.h"
#include "mg_api.h"



#define SERIAL   SerialUSB
#define USER_BUF_SIZE  1024

typedef struct 
{
        uint8_t buf[USER_BUF_SIZE];
        uint32_t recv_len;
}ble_trans_t;


typedef struct
{
        char temp_str[10];
        char hum_str[10];
}T_H_val_t;

#ifdef __cplusplus

extern "C"{

#endif



void BSP_Init(int cs_pin, int irq_pin);
void SPI_init();
unsigned char SPI_WriteRead(unsigned char SendData,unsigned char WriteFlag);
unsigned GetIRQSta(void);
void EnableLED(char Enable);
void SetLEDLum(int r, int g, int b, int L/*101 means not used*/);//[0~255]
void SetLEDLumPercent(int percent);

void SetBaudRate(unsigned long bps);


void LED_ONOFF(unsigned char OnOff);//for ui use

#ifdef __cplusplus

};

#endif
#endif //__BSP_H
