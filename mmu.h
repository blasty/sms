#ifndef __MMU_H__
#define __MMU_H__

#include <z80ex/z80ex.h>
#include "types.h"

u8 *mmu_mem_get();
u8 mmu_mem_read(u16 a);
void mmu_mem_write(u16 a, u8 data);
u8 mmu_io_read(u16 address);
void mmu_io_write(u16 address, u8 data);
//u8 mmu_int_read(u);

#endif
