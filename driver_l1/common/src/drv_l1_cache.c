/*
* Purpose: Cache controller driver/interface
*
* Author: Tristan Yang
*
* Date: 2007/09/21
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
*/

#include "drv_l1_cache.h"


#if (defined _DRV_L1_CACHE) && (_DRV_L1_CACHE == 1)


void cache_drain_write_buffer(void);
INT32S cache_drain_line(INT32U bank, INT32U addr);
INT32S cache_invalid_line(INT32U bank, INT32U addr);
INT32S cache_drain_bank(INT32U bank);
INT32S cache_invalid_bank(INT32U bank);

// Write buffer must be drained before invalid cache/disable write buffer/disable cache/lockdown/disable burst mode
void cache_drain_write_buffer(void)
{
	// Drain write buffer only if write buffer and cache are enabled
	if ((R_CACHE_CTRL & (C_CACHE_CTRL_EN|C_CACHE_CTRL_WRITE_BUFFER_EN)) == (C_CACHE_CTRL_EN|C_CACHE_CTRL_WRITE_BUFFER_EN)) {
		R_CACHE_DRAIN_WRITE_BUFFER = C_CACHE_WRITE_BUF_DRAIN_EN;
	}
}

INT32S cache_lockdown(INT32U addr, INT32U size)
{
	INT32U vlt_reg, bank, candidate, val_lock_tag;

	if (!size || size>C_CACHE_SIZE) {
		return -1;
	}
	if (addr > C_CACHE_AREA_END) {
		return -1;
	}
	if (!(R_CACHE_CTRL & C_CACHE_CTRL_EN)) {
		return -1;
	}

	// Make sure start address is 16-byte alignment, and size is multiple of 16
	if (addr & 0xF) {
		size += addr & 0xF;
	}
	size = (size+0xF) & ~0xF;
	addr &= C_CACHE_LOCK_TAG_INDEX_MASK;

	while (size) {
		candidate = C_CACHE_BANK_NUM;
		vlt_reg = (INT32U) P_CACHE_VALID_LOCK_TAG;

	  #if _OPERATING_SYSTEM != _OS_NONE				// Soft Protect for critical section
		OSSchedLock();
	  #endif
		// Enable Valid_Lock_Tag read mode
		R_CACHE_CFG |= C_CACHE_CFG_VLT_EN;
		for (bank=0; bank<C_CACHE_BANK_NUM; bank++) {
			val_lock_tag = *((INT32U *) (vlt_reg  + (bank<<C_CACHE_VLT_BANK_SHIFT) + (addr&C_CACHE_INDEX_MASK)));
			if (val_lock_tag & C_CACHE_VLT_VALID) {
				// The entry is valid
				if ((val_lock_tag&C_CACHE_VLT_TAG_MASK) == ((addr&C_CACHE_TAG_MASK)>>C_CACHE_TAG_SHIFT)) {
					// The tag field matches the line we want to lock
					if (val_lock_tag & C_CACHE_VLT_LOCK) {			// If already locked, do nothing.
						candidate = C_CACHE_BANK_NUM;
						break;
					} else {
						// Not locked yet, break and lock it in this bank now
						candidate = bank;
						break;
					}
				} else if (candidate == C_CACHE_BANK_NUM) {
					if (!(val_lock_tag & C_CACHE_VLT_LOCK)) {
						// If we don't have a candidate and this bank is not locked, mark it as candidate bank
						candidate = bank;
					}
				}
			} else {
			 	// It's a free bank, mark it as candidate bank
				candidate =  bank;
			}
		}
		// Disable Valid_Lock_Tag read mode
		R_CACHE_CFG &= ~C_CACHE_CFG_VLT_EN;
	  #if _OPERATING_SYSTEM != _OS_NONE
		OSSchedUnlock();
	  #endif

		if (candidate < C_CACHE_BANK_NUM) {		// Found a bank for locking down the line into cache
			cache_invalid_line(candidate, addr);

			// Drain write buffer
			cache_drain_write_buffer();

			R_CACHE_LOCKDOWN = C_CACHE_LOCK_EN | (candidate << C_CACHE_LOCK_BANK_SHIFT) | addr;
		}

		size -= 16;							// 16 bytes per line
		addr += 16;
		if (addr > C_CACHE_AREA_END) {
			return 0;
		}
		addr &= C_CACHE_LOCK_TAG_INDEX_MASK;
	}

	return 0;
}

INT32S cache_drain_line(INT32U bank, INT32U addr)
{
	INT32U index;

	if (addr > C_CACHE_AREA_END) {
		return -1;
	}
	if (bank >= C_CACHE_BANK_NUM) {			// Bank = 0, 1, ... C_CACHE_BANK_NUM-1
		return -1;
	}

	index = addr & C_CACHE_INV_LINE_INDEX_MASK;
	R_CACHE_INVALID_LINE = C_CACHE_INV_LINE_EN | (bank << C_CACHE_INV_LINE_BANK_SHIFT) | index | C_CACHE_INV_LINE_DRAIN;

	// Drain write buffer
	cache_drain_write_buffer();

	return 0;
}

INT32S cache_invalid_line(INT32U bank, INT32U addr)
{
	INT32U index;

	if (addr > C_CACHE_AREA_END) {
		return -1;
	}
	if (bank >= C_CACHE_BANK_NUM) {			// Bank = 0, 1, ... C_CACHE_BANK_NUM-1
		return -1;
	}

	index = addr & C_CACHE_INV_LINE_INDEX_MASK;
	R_CACHE_INVALID_LINE = C_CACHE_INV_LINE_EN | (bank << C_CACHE_INV_LINE_BANK_SHIFT) | index | C_CACHE_INV_LINE_INVALID;

	// Drain write buffer
	cache_drain_write_buffer();

	return 0;
}

INT32S cache_drain_bank(INT32U bank)
{
	if (bank >= C_CACHE_BANK_NUM) {			// Bank = 0, 1, ... C_CACHE_BANK_NUM-1
		return -1;
	}

	R_CACHE_INVALID_BANK = C_CACHE_INV_BANK_EN | (bank << C_CACHE_INV_BANK_BANK_SHIFT) | C_CACHE_INV_BANK_DRAIN;

	// Drain write buffer
	cache_drain_write_buffer();

	return 0;
}

INT32S cache_invalid_bank(INT32U bank)
{
	if (bank >= C_CACHE_BANK_NUM) {			// Bank = 0, 1, ... C_CACHE_BANK_NUM-1
		return -1;
	}

	R_CACHE_INVALID_BANK = C_CACHE_INV_BANK_EN | (bank << C_CACHE_INV_BANK_BANK_SHIFT) | C_CACHE_INV_BANK_INVALID;

	// Drain write buffer
	cache_drain_write_buffer();

	return 0;
}

INT32S cache_drain_range(INT32U addr, INT32U size)
{
	INT32U i;

	if (addr>C_CACHE_AREA_END || !size) {
		return -1;
	}
	if (!(R_CACHE_CTRL & C_CACHE_CTRL_WRITE_BACK_EN)) {
		return -1;
	}

	// Make sure start address is 16-byte alignment, and size is multiple of 16
	if (addr & 0xF) {
		size += addr & 0xF;
	}
	size = (size+0xF) & ~0xF;
	addr &= C_CACHE_INV_RANGE_ADDR_MASK;

	// addr and size are both multiple of 16 now, check if it exceed cache range
	if (addr+size > C_CACHE_AREA_END+1) {
		size = C_CACHE_AREA_END + 1 - addr;
	}
	if (size > C_CACHE_INV_RANGE_SIZE_MAX) {
		// Last line in cache area(0x3FFFFFF0~0x3FFFFFFF) can't be drained in this way
		for (i=0; i<C_CACHE_BANK_NUM; i++) {
			cache_drain_bank(i);
		}
		return 0;
	}

	R_CACHE_INVALID_RANGE_SEZE = size;
	R_CACHE_INVALID_RANGE_ADDR = C_CACHE_INV_RANGE_EN | addr | C_CACHE_INV_RANGE_DRAIN;

	// Drain write buffer
	cache_drain_write_buffer();

	return 0;
}

INT32S cache_invalid_range(INT32U addr, INT32U size)
{
	INT32U i;

	if (addr>C_CACHE_AREA_END || !size) {
		return -1;
	}

	// Make sure start address is 16-byte alignment, and size is multiple of 16
	if (addr & 0xF) {
		// If write back mode is enabled and addr is not 16-byte alignment, drain first line before invalid it
		if (R_CACHE_CTRL & C_CACHE_CTRL_WRITE_BACK_EN) {
			for (i=0; i<C_CACHE_BANK_NUM; i++) {
				cache_drain_line(i, addr);
			}
		}
		size += addr & 0xF;
		addr &= ~0xF;
	}
	if (size & 0xF) {
		// If write back mode is enabled and size is not 16-byte alignment, drain last line before invalid it
		if (R_CACHE_CTRL & C_CACHE_CTRL_WRITE_BACK_EN) {
			for (i=0; i<C_CACHE_BANK_NUM; i++) {
				cache_drain_line(i, addr+size);
			}
		}

		size = (size+0xF) & ~0xF;
	}
	addr &= C_CACHE_INV_RANGE_ADDR_MASK;

	// addr and size are both multiple of 16 now, check if it exceed cache range
	if (addr+size > C_CACHE_AREA_END+1) {
		size = C_CACHE_AREA_END + 1 - addr;
	}
	if (size > C_CACHE_INV_RANGE_SIZE_MAX) {
		// Last line in cache area(0x3FFFFFF0~0x3FFFFFFF) can't be invalided in this way
		for (i=0; i<C_CACHE_BANK_NUM; i++) {
			cache_invalid_bank(i);
		}
		return 0;
	}

	R_CACHE_INVALID_RANGE_SEZE = size;
	R_CACHE_INVALID_RANGE_ADDR = C_CACHE_INV_RANGE_EN | addr | C_CACHE_INV_RANGE_INVALID;

	// Drain write buffer
	cache_drain_write_buffer();

	return 0;
}

INT32S cache_burst_mode_enable(void)
{
	if (!(R_CACHE_CTRL & C_CACHE_CTRL_WRITE_BUFFER_EN)) {
		return -1;
	}
	// Do not allow to set burst mode in write back mode
	if (R_CACHE_CTRL & C_CACHE_CTRL_WRITE_BACK_EN) {
		return -1;
	}

	// Drain write buffer
	cache_drain_write_buffer();

	R_CACHE_CFG |= C_CACHE_CFG_BURST_EN;

	return 0;
}

void cache_burst_mode_disable(void)
{
	INT32U old_mask;

	if (!(R_CACHE_CFG & C_CACHE_CFG_BURST_EN)) {
		return;
	}

	old_mask = vic_global_mask_set();		// Mask global interrupt to prevent ISR from writing data to write buffer

	// Drain write buffer
	cache_drain_write_buffer();

	R_CACHE_CFG &= ~C_CACHE_CFG_BURST_EN;

	vic_global_mask_restore(old_mask);		// Restore global mask register
}

void cache_init(void)
{
	INT32U i;

	R_CACHE_CTRL = 0x0;
  #if (defined CACHE_SINGLE_MODE_MEMORY) && CACHE_SINGLE_MODE_MEMORY
	R_CACHE_CFG = C_CACHE_CFG_SINGLE_EN;
  #else
  	R_CACHE_CFG = 0x0;
  #endif

	// Invalid all cache lines of each bank
	for (i=0; i<C_CACHE_BANK_NUM; i++) {
		cache_invalid_bank(i);
	}

	R_CACHE_CTRL = C_CACHE_CTRL_WRITE_BUFFER_EN | C_CACHE_CTRL_EN;		// Cache is enabled after initialization
}

void cache_enable(void)
{
	INT32U i;

	if (R_CACHE_CTRL & C_CACHE_CTRL_EN) {
		return;
	}

	// Invalid all cache lines first
	for (i=0; i<C_CACHE_BANK_NUM; i++) {
		cache_invalid_bank(i);
	}

	// Enable cache and write buffer
	R_CACHE_CTRL |= C_CACHE_CTRL_WRITE_BUFFER_EN | C_CACHE_CTRL_EN;
}

void cache_disable(void)
{
	INT32U old_mask;

	if (!(R_CACHE_CTRL & C_CACHE_CTRL_EN)) {
		return;
	}

	if (R_CACHE_CTRL & C_CACHE_CTRL_WRITE_BACK_EN) {
		cache_write_back_disable();
	}

	old_mask = vic_global_mask_set();		// Mask global interrupt to prevent ISR from writing data to write buffer

	cache_drain_write_buffer();

	R_CACHE_CTRL &= ~(C_CACHE_CTRL_WRITE_BUFFER_EN | C_CACHE_CTRL_EN);

	vic_global_mask_restore(old_mask);		// Restore global mask register
}

void cache_write_back_enable(void)
{
	INT32U i;

	if (!(R_CACHE_CTRL & C_CACHE_CTRL_EN)) {
		for (i=0; i<C_CACHE_BANK_NUM; i++) {
			cache_invalid_bank(i);
		}
	}

	cache_burst_mode_disable();				// Burst mode bit must be disabled when write back mode is used

	R_CACHE_CTRL |= C_CACHE_CTRL_WRITE_BACK_EN | C_CACHE_CTRL_WRITE_BUFFER_EN | C_CACHE_CTRL_EN;
}

void cache_write_back_disable(void)
{
	INT32U old_mask;
	INT32U i;

	if (!(R_CACHE_CTRL & C_CACHE_CTRL_WRITE_BACK_EN)) {
		return;
	}

	old_mask = vic_global_mask_set();		// Mask global interrupt to prevent ISR from writing data to write buffer

	// Disable write back mode
	R_CACHE_CTRL &= ~C_CACHE_CTRL_WRITE_BACK_EN;

	// Drain all cache lines
	for (i=0; i<C_CACHE_BANK_NUM; i++) {
		cache_drain_bank(i);
	}

	// Drain write buffer to make sure all data has been writen back to memory
	cache_drain_write_buffer();

	vic_global_mask_restore(old_mask);		// Restore global mask register
}


#endif
