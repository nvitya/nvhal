# nvhal
Portable C++ Direct Hardware Access Layer for Embedded Projects

Several years ago I've created an easy to use vendor idependent hardware access library for ARM Cortex-M processors, called NVCM.
Github link: https://github.com/nvitya/nvcm, https://github.com/nvitya/nvcmtests 
(I'm still actively maintaining these)

I just wanted to drive the UARTs on the Raspberry Pi 4B with DMA so I ended up with this NVHAL project.
The HW access way is very much the same as in the NVCM.

Currently supported:
  - GPIO Access, Pin Configuration
  - CLOCKCNT: Light-weight System Timer Access (1 MHz, 64 bit)
  - UART
  - DMA

Currently only the Raspberry Pi 4B supported, I've tested it with a 2G variant.
The older Raspberry Pi models have only one UART and does not support DMA.

The structure of the NVHAL project is similar to NVCM, so it is relative easy to add support for other CPU-s (e.g. allwinner, rockchip).
