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
 *  brief:    BROADCOM DMA
 *  version:  1.00
 *  date:     2020-10-03
 *  authors:  nvitya
*/

#ifndef HWDMA_BROADCOM_H_
#define HWDMA_BROADCOM_H_

#define HWDMA_PRE_ONLY
#include "hwdma.h"

#define MAX_DMA_CHANNELS   7  // normal DMA channels only

#define HWDMA_BUFFER_SIZE  4096 * 4  // 16k

// BCM2385 ARM Peripherals 4.2.1.2
#define DMA_CB_TI_NO_WIDE_BURSTS (1<<26)
#define DMA_CB_TI_SRC_INC        (1<<8)
#define DMA_CB_TI_DEST_INC       (1<<4)
#define DMA_CB_TI_TDMODE         (1<<1)

#define DMA_CS_RESET    (1<<31)
#define DMA_CS_ABORT    (1<<30)
#define DMA_CS_DISDEBUG (1<<28)
#define DMA_CS_END      (1<<1)
#define DMA_CS_ACTIVE   (1<<0)

#define DMA_CB_TXFR_LEN_YLENGTH(y) (((y-1)&0x4fff) << 16)
#define DMA_CB_TXFR_LEN_XLENGTH(x) ((x)&0xffff)
#define DMA_CB_STRIDE_D_STRIDE(x)  (((x)&0xffff) << 16)
#define DMA_CB_STRIDE_S_STRIDE(x)  ((x)&0xffff)

#define DMA_CS_PRIORITY(x) ((x)&0xf << 16)
#define DMA_CS_PANIC_PRIORITY(x) ((x)&0xf << 20)

typedef struct
{
	volatile uint32_t   CS;           // Control and Status
	volatile uint32_t   CONBLK_AD;    // Control Block Address
	// these are loaded from the control block:
	volatile uint32_t   TI;           // Transfer Information
	volatile uint32_t   SOURCE_AD;    // Source Address
	volatile uint32_t   DEST_AD;      // Destination Address
	volatile uint32_t   TXFR_LEN;     // Transfer Length
	volatile uint32_t   STRIDE;       // 2D Stride
	volatile uint32_t   NEXTCONBK;    // Next Control Block Address
	volatile uint32_t   DMADEBUG;     // Debug info
//
} TDmaChannelRegs;

typedef struct
{
	volatile uint32_t   TI;           // Transfer Information
	volatile uint32_t   SOURCE_AD;    // Source Address
	volatile uint32_t   DEST_AD;      // Destination Address
	volatile uint32_t   TXFR_LEN;     // Transfer Length
	volatile uint32_t   STRIDE;       // 2D Stride
	volatile uint32_t   NEXTCONBK;    // Next Control Block Address
	volatile uint32_t   _reserved[2];
//
} TDmaControlBlock;  // 32 bytes

class THwDmaChannel_broadcom : public THwDmaChannel_pre
{
public:
	int                  dmarq = -1;

	TDmaChannelRegs *    regs = nullptr;
	TDmaControlBlock *   cb = nullptr;

	bool Init(int achnum, int admarq);

	void Prepare(bool aistx, unsigned aperiphaddr);
	inline void Enable() { regs->CS = (cs_reg_base | DMA_CS_ACTIVE); }

  inline void Disable() { regs->CS = (cs_reg_base); }  // remove the ACTIVE flag
	inline bool Enabled() { return ((regs->CS & DMA_CS_ACTIVE) != 0); }
	inline bool Active()  { return Enabled(); }
	unsigned Remaining();

	void PrepareTransfer(THwDmaTransfer * axfer);
	inline void StartPreparedTransfer()              { Enable(); }

protected:
	unsigned    cs_reg_base = 0;

};

#define HWDMACHANNEL_IMPL  THwDmaChannel_broadcom

uint8_t * hwdma_allocate_dma_buffer(unsigned asize);  // allocates uncached DMA buffer
unsigned hwdma_bus_address(void * aaddr);

#endif // def HWDMA_BROADCOM_H_
