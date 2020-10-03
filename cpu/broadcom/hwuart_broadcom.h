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
 *  file:     hwuart_broadcom.h
 *  brief:    BROADCOM UART
 *  version:  1.00
 *  date:     2020-09-29
 *  authors:  nvitya
*/

#ifndef HWUART_BROADCOM_H_
#define HWUART_BROADCOM_H_

#define HWUART_PRE_ONLY
#include "hwuart.h"
#include "hwdma.h"

struct THwUartRegs  // UART register definition for the BCM2711
{
	volatile uint32_t   DR;      // 00 - Data Register
	volatile uint32_t   RSRECR;  // 04 -
	         uint32_t   _res08[4];
	volatile uint32_t   FR;      // 18 -
           uint32_t   _res1C;
	volatile uint32_t   ILPR;    // 20
	volatile uint32_t   IBRD;    // 24
	volatile uint32_t   FBRD;    // 28
	volatile uint32_t   LCRH;    // 2C
	volatile uint32_t   CR;      // 30
	volatile uint32_t   IFLS;    // 34
	volatile uint32_t   IMSC;    // 38
	volatile uint32_t   RIS;     // 3C
	volatile uint32_t   MIS;     // 40
	volatile uint32_t   ICR;     // 44
	volatile uint32_t   DMACR;   // 48
           uint32_t   _res4C[13];
	volatile uint32_t   ITCR;    // 80
	volatile uint32_t   ITIP;    // 84
	volatile uint32_t   ITOP;    // 88
	volatile uint32_t   TDR;     // 8C
};

class THwUart_broadcom : public THwUart_pre
{
public:
	unsigned  dr_dma_address = 0;

	bool Init(int adevnum);

	bool TrySendChar(char ach);
	bool TryRecvChar(char * ach);

	inline bool SendFinished()   { return (0 != (regs->FR & (1 << 3))); }  // UART BUSY?

	void DmaAssign(bool istx, THwDmaChannel * admach);

	bool DmaStartSend(THwDmaTransfer * axfer);
	bool DmaStartRecv(THwDmaTransfer * axfer);

public:
	THwUartRegs *      regs = nullptr;
};

#define HWUART_IMPL THwUart_broadcom

#endif // def HWUART_BROADCOM_H_
