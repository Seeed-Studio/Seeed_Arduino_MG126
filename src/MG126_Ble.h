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

#ifndef _MG126_BLE_H_INCLUDED
#define _MG126_BLE_H_INCLUDED

#include <Arduino.h>
#include "mg_api.h"
#include "MG126_ble.h"
#include "bsp.h"

class MG126_Ble_Class 
{
public:
	MG126_Ble_Class(int cs_pin,int irq_pin);
	~MG126_Ble_Class();
	void io_init();
	void ble_init();
	void test();
	unsigned char * get_ble_mac_addr();

private:
	int cs;
	int irq;
	unsigned char *ble_mac_addr;
	unsigned char pld_adv[11] = {2, 0x1, 0x6, 3, 0x19, 0xc0, 0x03, 3, 0x3, 0x12, 0x18};
};
#endif//_MG126_BLE_H_INCLUDED