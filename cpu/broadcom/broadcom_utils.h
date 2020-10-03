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
 *  file:     broadcom_utils.h
 *  brief:    Special Broadcom BCM2711 (Raspberry PI 4B) utilities
 *  version:  1.00
 *  date:     2020-09-29
 *  authors:  nvitya
 *  notes:
 *    Uncached memory allocation using the VPU
*/

#ifndef BROADCOM_UTILS_H_
#define BROADCOM_UTILS_H_

unsigned broadcom_vpu_mem_alloc(unsigned size);
unsigned broadcom_vpu_mem_free(unsigned handle);
unsigned broadcom_vpu_mem_lock(unsigned handle);
unsigned broadcom_vpu_mem_unlock(unsigned handle);

#endif /* BROADCOM_UTILS_H_ */
