#include "bsp.h"

#include <Arduino.h>

#define LED 13
#define IRQ_PIN 7

volatile unsigned int SysTick_Count = 0;
volatile unsigned int TIMER1_Count = 0;

unsigned char cs;
unsigned char irq;
unsigned char state = 0;

volatile unsigned char ext_flag = 0;

void configure_TCCn_clock(){
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TCC0_TCC1)) ;
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 );         // Wait for synchronization
  Tcc* TC = (Tcc*) TCC0;                            // get timer struct
  TC->CTRLA.reg &= ~TCC_CTRLA_ENABLE;               // Disable TC
  while (TC->SYNCBUSY.bit.ENABLE == 1);             // wait for sync
  TC->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1;        // Set perscaler, use 48M
  TC->WAVE.reg |= TCC_WAVE_WAVEGEN_NFRQ;            // Set wave form configuration 
  while (TC->SYNCBUSY.bit.WAVE == 1);               // wait for sync
  TC->PER.reg = 0xBB80;                             // Set counter Top using the PER register , we need 1ms ticker
  while (TC->SYNCBUSY.bit.PER == 1);                // wait for sync
  TC->INTENSET.reg = 0;                             // disable all interrupts
  TC->INTENSET.bit.OVF = 1;                         // enable overfollow
  NVIC_EnableIRQ(TCC0_IRQn);                        // Enable InterruptVector
  TC->CTRLA.reg |= TCC_CTRLA_ENABLE ;               // Enable TC
  while (TC->SYNCBUSY.bit.ENABLE == 1);             // wait for sync 
}//end configure_TCn_clock


void configure_TCC1_clock(){
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TCC0_TCC1)) ;
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 );         // Wait for synchronization
  Tcc* TC = (Tcc*) TCC1;                            // get timer struct
  TC->CTRLA.reg &= ~TCC_CTRLA_ENABLE;               // Disable TC
  while (TC->SYNCBUSY.bit.ENABLE == 1);             // wait for sync
  TC->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV1;        // Set perscaler, use 12M
  TC->WAVE.reg |= TCC_WAVE_WAVEGEN_NFRQ;            // Set wave form configuration 
  while (TC->SYNCBUSY.bit.WAVE == 1);               // wait for sync
  TC->PER.reg = 0x80;                             // Set counter Top using the PER register , we need 1ms ticker
  while (TC->SYNCBUSY.bit.PER == 1);                // wait for sync
  TC->INTENSET.reg = 0;                             // disable all interrupts
  TC->INTENSET.bit.OVF = 1;                         // enable overfollow
  NVIC_EnableIRQ(TCC1_IRQn);                        // Enable InterruptVector
  TC->CTRLA.reg |= TCC_CTRLA_ENABLE ;               // Enable TC
  while (TC->SYNCBUSY.bit.ENABLE == 1);             // wait for sync 
}//end configure_TCn_clock


void TCC1_Handler(){
	Tcc* TC = (Tcc*) TCC1;    // get timer struct
  	TC->INTFLAG.bit.OVF = 1;

	
	if(ext_flag)
	{
		NVIC_DisableIRQ(TCC1_IRQn);
		ext_flag = 0;
		TIMER1_Count++;
		ble_run(0);
		NVIC_EnableIRQ(TCC1_IRQn);
	}
}



void TCC0_Handler(){        // Interrupt on overflow
  Tcc* TC = (Tcc*) TCC0;    // get timer struct
  TC->INTFLAG.bit.OVF = 1;  // writing a one clears the flag ovf flag

	SysTick_Count ++;
	state = !state;
	//digitalWrite(LED,state);
	ble_nMsRoutine();
	
		
}

uint32_t ext_count;
void External_Handler(){

	//digitalWrite(LED,HIGH);
	//arduino_c_print(">");
	// detachInterrupt(digitalPinToInterrupt(IRQ_PIN));
	// noInterrupts();
	// ble_run(0);
	// interrupts();
	// attachInterrupt(digitalPinToInterrupt(IRQ_PIN), External_Handler, FALLING);
	ext_count++;
	//arduino_c_print("<");
	//digitalWrite(LED,LOW);
	ext_flag = 1;
}

void BSP_Init(int cs_pin, int irq_pin)
{
	cs = cs_pin;
	irq = irq_pin;
	pinMode(cs, OUTPUT);
//	pinMode(irq, INPUT);
	pinMode(LED, OUTPUT);
	configure_TCCn_clock();

	configure_TCC1_clock();
	//
	attachInterrupt(digitalPinToInterrupt(irq_pin), External_Handler, FALLING);
	
	
#if 0
	  NVIC_DisableIRQ(EIC_IRQn);
	  NVIC_ClearPendingIRQ(EIC_IRQn);
	  NVIC_SetPriority(EIC_IRQn, 0);
	  NVIC_EnableIRQ(EIC_IRQn);

#endif  
	/* Set TCC0 Interrupt Priority to Level 1 */
	NVIC_SetPriority(TCC0_IRQn, 1);
	/* Enable TCC0 NVIC Interrupt Line */
	NVIC_EnableIRQ(TCC0_IRQn);
	
	NVIC_SetPriority(TCC1_IRQn, 2);
	NVIC_EnableIRQ(TCC1_IRQn);
}
 
void SPI_init(){
	arduino_c_spi_init();
}

void SetBaudRate(unsigned long bps)
{
	
}


void LED_ONOFF(unsigned char OnOff)
{
//	digitalWrite(LED, OnOff);
}


unsigned char SPI_WriteRead(unsigned char SendData,unsigned char WriteFlag)
{	
	unsigned char ReadData;
	arduino_c_spi_writeread(SendData,&ReadData);
	return ReadData;
}


void GetLedInfo(unsigned char* data)
{

}

void UpdateLEDValueAll(void);

void SetLedInfo(unsigned char* data)
{

}

void UpdateLEDValueAll(void)
{

}

void EnableLED(char Enable)
{

}

void SetLEDLum(int r, int g, int b, int L/*101 means not used*/)//[0~255]
{
}

void SetLEDLumPercent(int percent)
{

}

//unsigned GetIRQSta(void)
//{
//	if (!(GPIO_ReadInputData(GPIOA) & 0x01))
//	{
//		IWDG_ReloadCounter();
//		return 1;
//	}
//	else
//	{
//		return 0;
//	}
//}

void SPI_CS_Enable_(void) //porting api
{
	digitalWrite(cs, LOW);
}

void SPI_CS_Disable_(void) //porting api
{
	digitalWrite(cs, HIGH);
}

char IsIrqEnabled(void) //porting api
{
	return digitalRead(irq) == 0 ? 1: 0;
}

/****************************/
void SysClk8to48(void)
{
}
void SysClk48to8(void)
{
}

void DisableEnvINT(void)
{ 
}
void EnableEnvINT(void)
{
}

void McuGotoSleepAndWakeup(void)
{
}
void IrqMcuGotoSleepAndWakeup(void)
{
}

unsigned int GetSysTickCount(void)
{
    return SysTick_Count;
}

