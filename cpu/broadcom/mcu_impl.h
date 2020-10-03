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
 *  file:     mcu_impl.h (broadcom)
 *  brief:    Broadcom list of implemented NVHAL peripherals
 *  version:  1.00
 *  date:     2020-10-29
 *  authors:  nvitya
*/

#ifdef HWPINS_H_
  #include "hwpins_broadcom.h"
#endif

#ifdef HWUART_H_
  #include "hwuart_broadcom.h"
#endif

#ifdef HWDMA_H_
  #include "hwdma_broadcom.h"
#endif
