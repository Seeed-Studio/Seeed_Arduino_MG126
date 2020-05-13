/* 
 *
 * 2019 Copyright (c) Seeed Technology Inc.  All right reserved.
 * Author:Baozhu zuo (zuobaozhu@gmail.com)
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "spi_wrapper.h"
#include "SPI.h"
void arduino_c_spi_init(){
	SPI1.begin();
	SPI1.setClockDivider (8);
}
void arduino_c_spi_writeread(unsigned char WriteData,unsigned char* ReadData){
	*ReadData = SPI1.transfer(WriteData);
}
void arduino_c_print(char* str){
	SERIAL.println(str);
}

void arduino_c_print_num(int num)
{
	SERIAL.println(num);
}