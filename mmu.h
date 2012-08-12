#ifndef __MMU_H__
#define __MMU_H__

#include "types.h"

void mmu_init();
u8 *mmu_mem_get();
u8 *mmu_wram_get();
u8 mmu_mem_read(u16 a);
void mmu_mem_write(int address, int data);
u8 mmu_io_read(u16 address);
void mmu_io_write(u16 address, u8 data);
//u8 mmu_int_read(u);

#endif
