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
 *  file:     hwdma_broadcom.cpp
 *  brief:    BROADCOM DMA Implementation
 *  version:  1.00
 *  date:     2020-10-03
 *  authors:  nvitya
 *  notes:
 *    Only channels 0-6 are supported here, excluding the Lite and DMA4 channels.
 *    Allocates an uncached memory and uses that for DMA buffers and control blocks.
 *    The uncached memory is allocated using the Video Core.
*/

#include <stdio.h>
#include <stdarg.h>

#include "hwdma.h"
#include "hw_utils.h"
#include "broadcom_utils.h"
#include "clockcnt.h"

uint8_t *  g_dma_channel_regs = nullptr;

// uncached memory for DMA buffers and control blocks
// this must be allocated using the VPU

uint8_t *  g_dma_buffer = nullptr;
unsigned   g_dma_buffer_size = 0;
unsigned   g_dma_buffer_allocated = 0;

unsigned   g_dmabuf_mem_handle = 0;
unsigned   g_dmabuf_bus_addr = 0;
unsigned   g_dmabuf_phys_addr = 0;

bool hwdma_init_dma_buffer()
{
	if (g_dma_buffer)
	{
		return true;
	}

	// the allocated memory won't be freed when this application exits

	//TODO: reuse the existing allocation

	g_dma_buffer_size = HWDMA_BUFFER_SIZE;

	printf("Allocating %u byte uncached DMA buffer\n", g_dma_buffer_size);

	g_dmabuf_mem_handle = broadcom_vpu_mem_alloc(g_dma_buffer_size);
	//printf(" mem_handle = %08X\n", mem_handle);
	g_dmabuf_bus_addr = broadcom_vpu_mem_lock(g_dmabuf_mem_handle);
	//printf(" bus_addr   = %08X\n", bus_addr);
	g_dmabuf_phys_addr = (g_dmabuf_bus_addr & 0x3FFFFFFF);
	// printf(" phys_addr  = %08X\n", phys_addr);

	g_dma_buffer = (uint8_t *)hw_memmap(g_dmabuf_phys_addr, g_dma_buffer_size);

	return true;
}

uint8_t * hwdma_allocate_dma_buffer(unsigned asize)  // no freeing yet
{
	if (!hwdma_init_dma_buffer())
	{
		return nullptr;
	}

	asize = ((asize + 31) & 0xFFFFFFE0);  // 32 byte granularity
	if (asize > g_dma_buffer_size - g_dma_buffer_allocated)
	{
		return nullptr;
	}

	uint8_t * result = (g_dma_buffer + g_dma_buffer_allocated);

	g_dma_buffer_allocated += asize;

	return result;
}

unsigned hwdma_bus_address(void * aaddr)
{
	if ((aaddr < g_dma_buffer) || (aaddr > g_dma_buffer + g_dma_buffer_size))
	{
		printf("invalid address for DMA tranfer: %p\n", aaddr);
		return 0;
	}

	unsigned offs = (uint8_t *)aaddr - g_dma_buffer;
	return g_dmabuf_bus_addr + offs;
}

bool THwDmaChannel_broadcom::Init(int achnum, int admarq)  // admarq = peripheral DMA Request ID
{
	initialized = false;

  // channel init

	chnum = achnum;
	dmarq = admarq;

	if ((chnum < 0) || (chnum >= MAX_DMA_CHANNELS))
	{
		chnum = -1;
		return false;
	}

	if (!hwdma_init_dma_buffer())
	{
		return false;
	}

	if (!g_dma_channel_regs)
	{
		g_dma_channel_regs = (uint8_t *)hw_memmap(HWDMA_BASE_ADDRESS, 4096);
	}

  regs = (TDmaChannelRegs *)(g_dma_channel_regs + 0x100);

  // allocate the control blocks in the uncached memory
  if (!cb)
  {
  	cb = (TDmaControlBlock *)hwdma_allocate_dma_buffer(sizeof(TDmaControlBlock));
  	if (!cb)
  	{
  		return false;
  	}
  }

  regs->CS = (1u << 31); // reset
  delay_us(1);

  cs_reg_base = 0
    | (1  << 29)  // 1 = do not stop during debugging
    | (1  << 28)  // 1 = wait for outstanding writes
    | (13 << 20)  // PANIC_PRIORITY(4)
    | ((priority & 15) << 20)  // PRIORITY(4)
  ;

  regs->CONBLK_AD = 0;

  regs->CS = cs_reg_base;

	initialized = true;

	return true;
}

void THwDmaChannel_broadcom::Prepare(bool aistx, unsigned aperiphaddr)
{
	istx = aistx;
	periphaddr = aperiphaddr;
}

void THwDmaChannel_broadcom::PrepareTransfer(THwDmaTransfer * axfer)
{
	uint32_t tinfo = 0
		| (1 << 26)  // NO_WIDE_BURSTS: 0 = enable 2 cycle bursts, 1 = no wide bursts
		| (0 << 21)  // WAITS(5): Add Wait Cycles
		| (dmarq << 16)  // PERMAP(5): Peripheral mapping
		| (0 << 12)  // BURST_LENGTH(4): 0 = single
		| (0 << 11)  // SRC_IGNORE: 0 = read from the source address
		| (0 << 10)  // SRC_DREQ: 1 = reads are controller by the DMA request signal
		| (0 <<  9)  // SRC_WIDTH: 0 = 32 bit, 1 = 128 bit
		| (0 <<  8)  // SRC_INC: 0 = source address does not change (use this for striping), 1 = source address increments with 0
		| (0 <<  7)  // DEST_IGNORE: 0 = write data to the destination
		| (0 <<  6)  // DEST_DREQ: 1 = writes are controlled by the DMA request signal
		| (0 <<  5)  // DEST_WIDTH: 0 = 32 bit, 1 = 128 bit
		| (0 <<  4)  // DEST_INC: 0 = destination address does not change (use this for striping), 1 = the destination address increments
		| (1 <<  3)  // WAIT_RESP: 1 = wait for the write response
		| (1 <<  1)  // TDMODE: 2D Mode, 1 = use TXFR_LEN as XLENGHT+YLENGTH, 0 = TXFR_LEN is a single length
		| (0 <<  0)  // INTEN: 0 = interrupt disabled
	;

	unsigned sinc = 0;
	unsigned dinc = 0;

	if (istx)  // MEM -> PER
	{
		if ((axfer->flags & DMATR_NO_SRC_INC) == 0)
		{
			sinc = axfer->bytewidth;
		}

		tinfo |= (1 << 6);  // DEST_DREQ

		cb->SOURCE_AD = hwdma_bus_address(axfer->srcaddr);
		cb->DEST_AD = periphaddr;
	}
	else // PER -> MEM
	{
		if ((axfer->flags & DMATR_NO_DST_INC) == 0)
		{
			dinc = axfer->bytewidth;
		}

		tinfo |= (1 << 10);  // SRC_DREQ

		cb->SOURCE_AD = periphaddr;
		cb->DEST_AD = hwdma_bus_address(axfer->dstaddr);
	}

	cb->TI = tinfo;
	cb->TXFR_LEN = (((axfer->count-1) << 16) | axfer->bytewidth);
	cb->STRIDE = ((dinc << 16) | sinc);

	if (axfer->flags & DMATR_CIRCULAR)
	{
		cb->NEXTCONBK = hwdma_bus_address(cb);  // loop back to self
	}
	else
	{
		cb->NEXTCONBK = 0;
	}

  regs->CONBLK_AD = hwdma_bus_address(cb);  // set the control block address
}

unsigned THwDmaChannel_broadcom::Remaining()
{
	unsigned txfr_len = regs->TXFR_LEN;
	unsigned bw = (txfr_len & 0xFFFF);
	unsigned cnt = (txfr_len >> 16);
  return (bw * cnt + bw);
}
