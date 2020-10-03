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
 *  file:     hwpins_broadcom.h
 *  brief:    Broadcom Pin/Pad and GPIO configuration
 *  version:  1.00
 *  date:     2020-09-27
 *  authors:  nvitya
*/

#ifndef _HWPINS_BROADCOM_H
#define _HWPINS_BROADCOM_H

#define HWPINS_PRE_ONLY
#include "hwpins.h"

struct THwGpioRegs  // gpio register definition for the BCM2711
{
	volatile uint32_t   GPFSEL[7];   // 00..18, index 6 is invalid!
	volatile uint32_t   GPSET[3];    // 1C..24, index 2 is invalid!
	volatile uint32_t   GPCLR[3];    // 28..30, index 2 is invalid!
	volatile uint32_t   GPLEV[3];    // 34..3C, index 2 is invalid!
	volatile uint32_t   GPEDS[3];    // 40..48, index 2 is invalid!
	volatile uint32_t   GPREN[3];    // 4C..54, index 2 is invalid!
	volatile uint32_t   GPFEN[3];    // 58..60, index 2 is invalid!
	volatile uint32_t   GPHEN[3];    // 64..6C, index 2 is invalid!
	volatile uint32_t   GPLEN[3];    // 70..78, index 2 is invalid!
	volatile uint32_t   GPAREN[3];   // 7C..84, index 2 is invalid!
	volatile uint32_t   GPAFEN[3];   // 88..90, index 2 is invalid!
	         uint32_t   _pad[19];    // 94..E0
	volatile uint32_t   PUP_PDN_CNTRL_REG[4];   // E4..F0
};

class THwPinCtrl_broadcom : public THwPinCtrl_pre
{
public:
	THwGpioRegs *    regs = nullptr;

	// platform specific
	bool PinSetup(int aportnum, int apinnum, unsigned flags);

	THwGpioRegs * GetGpioRegs(int aportnum);

	bool GpioPortEnable(int aportnum);

	void GpioSet(int aportnum, int apinnum, int value);

	inline bool GpioSetup(int aportnum, int apinnum, unsigned flags)  { return PinSetup(aportnum, apinnum, flags); }
};

class TGpioPin_broadcom : public TGpioPin_common
{
public:
	THwGpioRegs *    regs = nullptr;
	unsigned         regidx = 0;

	bool Setup(unsigned flags);
	void Assign(int aportnum, int apinnum, bool ainvert);

	void Toggle();

	void SwitchDirection(int adirection);
};

#define HWPINCTRL_IMPL   THwPinCtrl_broadcom
#define HWGPIOPIN_IMPL   TGpioPin_broadcom

#endif /* HWPINS_BROADCOM_H_ */
