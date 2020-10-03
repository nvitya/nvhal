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
 *  file:     hwpins_broadcom.cpp
 *  brief:    Broadcom Pin/Pad and GPIO configuration
 *  version:  1.00
 *  date:     2020-09-27
 *  authors:  nvitya
*/

#include "platform.h"
#include "hwpins.h"
#include "hw_utils.h"

#include "stdio.h"
#include "stddef.h"

#define MAX_PORT_NUMBER   1
#define MAX_PIN_NUMBER   57

THwGpioRegs * THwPinCtrl_broadcom::GetGpioRegs(int aportnum)
{
	if (!regs)
	{
		regs = (THwGpioRegs *)hw_memmap(HW_GPIO_BASE, sizeof(THwGpioRegs));
	}

	return regs;
}

bool THwPinCtrl_broadcom::PinSetup(int aportnum, int apinnum, unsigned flags)
{
	GetGpioRegs(aportnum); // set the regs member
	if (!regs)
	{
		return false;
	}

	if ((apinnum < 0) || (apinnum > MAX_PIN_NUMBER))
	{
		return false;
	}

  unsigned tmp;

  unsigned regidx1 = (apinnum >> 5);
  unsigned regshift1 = (apinnum & 31);
  unsigned regidx2 = (apinnum >> 4);
  unsigned regshift2 = ((apinnum & 15) << 1);
  unsigned regidx3 = (apinnum / 10);
  unsigned regshift3 = ((apinnum % 10) * 3);

  // set gpio initial state now to avoid possible glitches

  if (flags & PINCFG_GPIO_INIT_1)
  {
  	regs->GPSET[regidx1] = (1 << regshift1);
  }
  else
  {
  	regs->GPCLR[regidx1] = (1 << regshift1);
  }

  // Set to Input / Output or alternate function

  unsigned sel;
  if (flags & PINCFG_AF_MASK)
  {
  	sel = 2 + ((flags >> PINCFG_AF_SHIFT) & 7);
  }
  else if (flags & PINCFG_OUTPUT)
  {
    sel = 1;
  }
  else // input
  {
  	sel = 0;
  }

  printf("offsetof(GPFSEL) = %i\n", offsetof(THwGpioRegs, GPFSEL));

  tmp = regs->GPFSEL[regidx3];
  tmp &= ~(7 << regshift3);
  tmp |= (sel << regshift3);
  regs->GPFSEL[regidx3] = tmp;

  // Set pullup / pulldown
  if (flags & PINCFG_PULLUP)
  {
  	sel = 1; // pullup
  }
  else if (flags & PINCFG_PULLDOWN)
  {
  	sel = 2; // pulldown
  }
  else
  {
  	sel = 0;
  }

  tmp = regs->PUP_PDN_CNTRL_REG[regidx2];
  tmp &= ~(3 << regshift2);
  tmp |= (sel << regshift2);
  regs->PUP_PDN_CNTRL_REG[regidx2] = tmp;

  return true;
}

bool THwPinCtrl_broadcom::GpioPortEnable(int aportnum)
{
  return true;
}

void THwPinCtrl_broadcom::GpioSet(int aportnum, int apinnum, int value)
{
  unsigned regidx   = (apinnum >> 5);
  unsigned regshift = (apinnum & 31);

  if (1 == value)
  {
  	regs->GPSET[regidx] = (1 << regshift);
  }
  else if (value & 2) // toggle
  {
  	uint32_t cv = ((regs->GPSET[regidx] >> regshift) & 1);
  	if (cv)
  	{
  		regs->GPCLR[regidx] = (1 << regshift);
  	}
  	else
  	{
  		regs->GPSET[regidx] = (1 << regshift);
  	}
  }
  else
  {
  	regs->GPCLR[regidx] = (1 << regshift);
  }
}


// GPIO Pin

void TGpioPin_broadcom::Assign(int aportnum, int apinnum, bool ainvert)
{
	portnum = aportnum;
  pinnum = apinnum;
  inverted = ainvert;

	regs = hwpinctrl.GetGpioRegs(aportnum);
	if (!regs)
	{
		return;
	}

	if ((apinnum < 0) || (apinnum > MAX_PIN_NUMBER))
	{
		regs = nullptr;
		return;
	}

  regidx   = (apinnum >> 5);
  unsigned regshift = (apinnum & 31);
  getbitshift = regshift;

  getbitptr = (unsigned *)&(regs->GPLEV[regidx]);
  getoutbitptr = (unsigned *)&(regs->GPLEV[regidx]);
	setbitvalue = (1 << regshift);
	clrbitvalue = (1 << regshift);

  if (ainvert)
  {
    setbitptr = (unsigned *)&(regs->GPCLR[regidx]);
    clrbitptr = (unsigned *)&(regs->GPSET[regidx]);
  }
  else
  {
    setbitptr = (unsigned *)&(regs->GPSET[regidx]);
    clrbitptr = (unsigned *)&(regs->GPCLR[regidx]);
  }
}

void TGpioPin_broadcom::Toggle()
{
	// generic code without hw toggle support:
	if (Value())
	{
		Set0();
	}
	else
	{
		Set1();
	}
}

void TGpioPin_broadcom::SwitchDirection(int adirection)
{
	unsigned sel = (adirection & 1);

  unsigned regidx3 = (pinnum / 10);
  unsigned regshift3 = ((pinnum % 10) * 3);

  unsigned tmp;
  tmp = regs->GPFSEL[regidx3];
  tmp &= ~(7 << regshift3);
  tmp |= (sel << regshift3);
  regs->GPFSEL[regidx3] = tmp;
}

