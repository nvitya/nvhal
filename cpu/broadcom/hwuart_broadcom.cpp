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
 *  file:     hwuart_broadcom.cpp
 *  brief:    BROADCOM UART
 *  version:  1.00
 *  date:     2020-09-29
 *  authors:  nvitya
*/

#include <stdio.h>
#include <stdarg.h>
#include "string.h"

#include "hwuart.h"
#include "hw_utils.h"

#define HWUART_MAX   6

uint8_t * g_hwuart_base_mem = nullptr;

const uintptr_t hwuart_dev_offsets[6] = {0x000, 0xFFFF, 0x400, 0x600, 0x800, 0xA00};

bool THwUart_broadcom::Init(int adevnum)  // devnum: 0, 2, 3, 4, 5 , (UART1 is mini-uart, not supported here yet)
{
	devnum = adevnum;
	initialized = false;
	regs = nullptr;

	if ((adevnum < 0) or (adevnum >= HWUART_MAX))
	{
		return false;
	}

	uintptr_t devoffs = hwuart_dev_offsets[devnum];

	if (0xFFFF == devoffs)
	{
		return false;
	}

	if (!g_hwuart_base_mem)
	{
		g_hwuart_base_mem = (uint8_t *)hw_memmap(HWUART_BASE_ADDRESS, 4096);
		if (!g_hwuart_base_mem)
		{
			return false;
		}
	}

	regs = (THwUartRegs *)(g_hwuart_base_mem + devoffs);

	dr_dma_address = ((HWUART_BASE_ADDRESS + devoffs) & 0x7FFFFFFF);

	regs->CR = 0; // disable the uart

	// set baudrate
	unsigned baseclock = HWUART_BASE_CLOCK;

	// baudrate = baseclock / (16 * brdiv)
	// the brdiv has a 6 bit fractional part, therefore is the multiplication with 64:
  //   brdiv * 64 = (baseclock * 64) / (16 * baudrate)

	unsigned brdiv_x64 = (baseclock * 4) / baudrate;

	regs->IBRD = (brdiv_x64 >> 6);  // 16 bit integer part
	regs->FBRD = (brdiv_x64 & 63);  // 6 bit fractional part

	if (databits > 8)  databits = 8;
	if (databits < 5)  databits = 5;

	unsigned lcrh = 0
	  | (0  <<  7)  // SPS: Stick parity select
	  | ((databits - 5)  <<  5)  // WLEN(2): 00 = 5 bits, 11 = 8 bits
	  | (1  <<  4)  // FEN: 1 = enable FIFO (32 bytes)
	  | (0  <<  3)  // STP2: 0 = 1 stop bit, 1 = 2 stop bits sent, but not checked
	  | (0  <<  2)  // EPS: 0 = odd parity, 1 = even parity
	  | (0  <<  1)  // PEN: 0 = disable parity, 1 = enable parity
	  | (0  <<  0)  // BRK: Send Break
	;

	if (halfstopbits > 2)  lcrh |= (1 << 3);
	if (parity)
	{
		lcrh |= (1 << 1);
		if (!oddparity)  lcrh |= (1 << 2);
	}

	regs->LCRH = lcrh;

	regs->IFLS = 0;
	regs->IMSC = 0x000; // disable all interrupts
	regs->ICR = 0x7FF; // clear all interrupts
	regs->DMACR = 0;

	regs->CR = 0
		| (0  << 15)  // CTSEN
		| (0  << 14)  // RTSEN
		| (0  << 11)  // RTS
		| (1  <<  9)  // RXE: 1 = receive enable
		| (1  <<  8)  // TXE: 1 = transmit enable
		| (0  <<  7)  // LBE: 1 = loopback enable
		| (1  <<  0)  // UARTEN: 1 = enable uart
	;

	initialized = true;

	return true;
}

bool THwUart_broadcom::TrySendChar(char ach)
{
	if (0 == (regs->FR & (1 << 5)))  // Transmit FIFO not Full?
	{
		regs->DR = ach;
		return true;
	}
	else
	{
		return false;
	}
}

bool THwUart_broadcom::TryRecvChar(char * ach)
{
	if (regs->FR & (1 << 4)) // receive FIFO empty ?
	{
		return false;
	}
	else
	{
		*ach = regs->DR;
		return true;
	}
}

void THwUart_broadcom::DmaAssign(bool istx, THwDmaChannel * admach)
{
	if (istx)
	{
		txdma = admach;
	}
	else
	{
		rxdma = admach;
	}

	admach->Prepare(istx, dr_dma_address);
}


bool THwUart_broadcom::DmaStartSend(THwDmaTransfer * axfer)
{
	if (!txdma || !axfer)
	{
		return false;
	}

	regs->DMACR |= (1 << 1); // enable TX DMA

	txdma->StartTransfer(axfer);

	return true;
}

bool THwUart_broadcom::DmaStartRecv(THwDmaTransfer * axfer)
{
	if (!rxdma || !axfer)
	{
		return false;
	}

	regs->DMACR |= (1 << 0); // enable RX DMA

	rxdma->StartTransfer(axfer);

	return true;
}


