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
 *  file:     bcm2711.h
 *  brief:    Broadcom BCM2711 (Raspberry PI 4B) definitions
 *  version:  1.00
 *  date:     2020-09-29
 *  authors:  nvitya
*/

#ifndef BCM2711_H_
#define BCM2711_H_

// peripheral addresses (for hw_memmap), for the bus address apply 0x7FFFFFFF mask

#define SYSTEM_TIMER_BASE     0xFE003000
#define HW_GPIO_BASE          0xFE200000  // different than in the documentation (0xFE21500)

#define HWUART_BASE_ADDRESS   0xFE201000
#define HWDMA_BASE_ADDRESS    0xFE007000

// clocks

#define CLOCKCNT_SPEED           1000000
#define HWUART_BASE_CLOCK       48000000

#endif /* BCM2711_H_ */
