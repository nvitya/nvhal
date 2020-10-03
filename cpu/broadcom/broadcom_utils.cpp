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
 *  file:     broadcom_utils.cpp
 *  brief:    Special Broadcom BCM2711 (Raspberry PI 4B) utilities
 *  version:  1.00
 *  date:     2020-09-29
 *  authors:  nvitya
 *  notes:
 *    Uncached memory allocation using the VPU
*/

#include "stdio.h"
#include "stdlib.h"
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#define MEM_FLAG_DIRECT           (1 << 2)
#define MEM_FLAG_COHERENT         (2 << 2)
#define MEM_FLAG_L1_NONALLOCATING (MEM_FLAG_DIRECT | MEM_FLAG_COHERENT)

static int broadcom_vpu_fd = -1;

bool broardcom_vpu_mbox_open()
{
	if (broadcom_vpu_fd < 0)
	{
		broadcom_vpu_fd = open("/dev/vcio", 0);
	}

	return (broadcom_vpu_fd > 0);
}

void broardcom_vpu_mbox_close()
{
	if (broadcom_vpu_fd > 0)
	{
		close(broadcom_vpu_fd);
		broadcom_vpu_fd = -1;
	}
}

bool broadcom_vpu_mbox_cmd(unsigned * buf)
{
	if (!broardcom_vpu_mbox_open())
	{
		return false;
	}

  int r = ioctl(broadcom_vpu_fd, _IOWR(100, 0, char *), buf);
  if (r < 0)
  {
  	return false;
  }

  return true;
}

unsigned broadcom_vpu_mem_alloc(unsigned size)
{
   int i=0;
   unsigned p[32];
   p[i++] = 0; // size
   p[i++] = 0x00000000; // process request

   p[i++] = 0x3000c; // (the tag id)
   p[i++] = 12; // (size of the buffer)
   p[i++] = 12; // (size of the data)
   p[i++] = size; // (num bytes? or pages?)
   p[i++] = 4096; // (alignment)
   p[i++] = MEM_FLAG_L1_NONALLOCATING; // flags

   p[i++] = 0x00000000; // end tag
   p[0] = i * sizeof(p[0]); // actual size

   if (broadcom_vpu_mbox_cmd(p))
   {
     return p[5];
   }
   else
   {
     printf("broadcom_vpu_mem_alloc error\n");
  	 return 0;
   }
}

unsigned broadcom_vpu_mem_free(unsigned handle)
{
   int i=0;
   unsigned p[32];
   p[i++] = 0; // size
   p[i++] = 0x00000000; // process request

   p[i++] = 0x3000f; // (the tag id)
   p[i++] = 4; // (size of the buffer)
   p[i++] = 4; // (size of the data)
   p[i++] = handle;

   p[i++] = 0x00000000; // end tag
   p[0] = i * sizeof(p[0]); // actual size

   if (broadcom_vpu_mbox_cmd(p))
   {
     return p[5];
   }
   else
   {
     printf("broadcom_vpu_mem_free error\n");
  	 return 0;
   }
}

unsigned broadcom_vpu_mem_lock(unsigned handle)
{
   int i=0;
   unsigned p[32];
   p[i++] = 0; // size
   p[i++] = 0x00000000; // process request

   p[i++] = 0x3000d; // (the tag id)
   p[i++] = 4; // (size of the buffer)
   p[i++] = 4; // (size of the data)
   p[i++] = handle;

   p[i++] = 0x00000000; // end tag
   p[0] = i * sizeof(p[0]); // actual size

   if (broadcom_vpu_mbox_cmd(p))
   {
     return p[5];
   }
   else
   {
     printf("broadcom_vpu_mem_lock error\n");
  	 return 0;
   }
}

unsigned broadcom_vpu_mem_unlock(unsigned handle)
{
   int i=0;
   unsigned p[32];
   p[i++] = 0; // size
   p[i++] = 0x00000000; // process request

   p[i++] = 0x3000e; // (the tag id)
   p[i++] = 4; // (size of the buffer)
   p[i++] = 4; // (size of the data)
   p[i++] = handle;

   p[i++] = 0x00000000; // end tag
   p[0] = i*sizeof *p; // actual size

   if (broadcom_vpu_mbox_cmd(p))
   {
     return p[5];
   }
   else
   {
     printf("broadcom_vpu_mem_unlock error\n");
  	 return 0;
   }
}
