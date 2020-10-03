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
 *  file:     clockcnt.h
 *  brief:    Clock Counter vendor-independent implementations
 *  version:  1.00
 *  date:     2020-10-01
 *  authors:  nvitya
 *  notes:
 *    a MCU specific clockcnt_init() required (using a HW timer)
*/

#include "platform.h"
#include "clockcnt.h"

void delay_clocks(clockcnt_t aclocks)
{
	clockcnt_t remaining = aclocks;

	clockcnt_t t0 = CLOCKCNT;
	while (CLOCKCNT - t0 < remaining)
	{
		// wait
	}
}

