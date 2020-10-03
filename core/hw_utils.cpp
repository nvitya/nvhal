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
 *  file:     hw_utils.cpp
 *  brief:    Special System Utilities for direct HW Access
 *  version:  1.00
 *  date:     2020-10-01
 *  authors:  nvitya
*/

#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include "stdint.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define MEM_PAGE_SIZE   (4 * 1024)

void * hw_memmap(uintptr_t aaddr, unsigned asize)
{
	int  mem_fd;

	mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (mem_fd < 0)
	{
		return nullptr;
	}

	uintptr_t startaddr = aaddr & ~(MEM_PAGE_SIZE - 1);

	size_t memblksize = ((asize + (MEM_PAGE_SIZE - 1)) & ~MEM_PAGE_SIZE);

	uint8_t * mmapresult = (uint8_t *)mmap(
    nullptr,                 // target address within our address space, nullptr = selected by the system
    memblksize,              // map length
    PROT_READ | PROT_WRITE,  // Enable reading & writting to mapped memory
    MAP_SHARED,              // shared with other processes
    mem_fd,                  // file to map
    startaddr // offset
   );

	close(mem_fd); // not required anymore

	if (mmapresult == MAP_FAILED)
	{
		return nullptr;
	}

	return mmapresult + (aaddr & (MEM_PAGE_SIZE - 1));
}
