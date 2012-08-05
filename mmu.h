#ifndef __MMU_H__
#define __MMU_H__

#include "types.h"

u8 mmu_mem_read(int param, u16 a);
void mmu_mem_write(int param, u16 a, u8 data);
u8 mmu_io_read(int param, u16 address);
void mmu_io_write(int param, u16 address, u8 data);

u8 *mmu_mem_get();

#endif
