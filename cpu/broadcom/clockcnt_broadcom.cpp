/* -----------------------------------------------------------------------------
 * This file is a part of the NVHAL project: https://github.com/nvitya/nvhal
 * Copyright (c) 2020 Viktor Nagy, nvitya
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 * --------------------------------------------------------------------------- */
/*
 *  file:     clockcnt_broadcom.cpp
 *  brief:    Broadcom BCM2711 (Raspberry PI 4B) Clock Counter implementation
 *  version:  1.00
 *  date:     2020-09-29
 *  authors:  nvitya
 *  notes:
 *    The Free Running system timer is used, which runs at 1 MHz so the resolution is not so good
*/

#include "stdint.h"
#include "platform.h"
#include "hw_utils.h"

// free running system timer access initialization for BCM2711

struct THwSystemTimer
{
	volatile uint32_t    CS;  // control / status
	volatile uint32_t    CLO; // timer low
	volatile uint32_t    CHI; // timer high
	volatile uint32_t    C0;  // timer compare 0
	volatile uint32_t    C1;  // timer compare 1
	volatile uint32_t    C2;  // timer compare 2
	volatile uint32_t    C3;	// timer compare 3
};

static THwSystemTimer *   hw_system_timer = nullptr;

void clockcnt_init()
{
  hw_system_timer = (THwSystemTimer *)hw_memmap(SYSTEM_TIMER_BASE, sizeof(THwSystemTimer));
}

uint64_t clockcnt()
{
	uint32_t low;
	uint32_t high;
	uint32_t high2;

	do
	{
	  high  = hw_system_timer->CHI;
	  low   = hw_system_timer->CLO;
	  high2 = hw_system_timer->CHI;
	}
	while (high2 != high);

  return ((uint64_t(high) << 32) | low);
}

