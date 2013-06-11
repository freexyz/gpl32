/*
 ******************************************************************************
 * frdbm.c - face recognition database manage
 *
 * Copyright (c) 2013-2015 by ZealTek Electronic Co., Ltd.
 *
 * This software is copyrighted by and is the property of ZealTek
 * Electronic Co., Ltd. All rights are reserved by ZealTek Electronic
 * Co., Ltd. This software may only be used in accordance with the
 * corresponding license agreement. Any unauthorized use, duplication,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice MUST not be removed or modified without prior
 * written consent of ZealTek Electronic Co., Ltd.
 *
 * ZealTek Electronic Co., Ltd. reserves the right to modify this
 * software without notice.
 *
 * History:
 *	2013.05.24	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */
#include <stdarg.h>

#include "application.h"
#include "frdbm.h"

/*
 * for debug
 */
#define TRACE_NONE		0x00000000UL
#define TRACE_MSG		0x00000001UL
#define TRACE_INIT		0x00000002UL
#define TRACE_SPI		0x00000004UL
#define TRACE_CARRIER		0x00000008UL
#define TRACE_INVALID		0x00000010UL
#define TRACE_VALID		0x00000020UL



#define DRDBM_DEBUG		( \
	TRACE_MSG		| \
	TRACE_INIT		| \
	TRACE_SPI		| \
	TRACE_CARRIER		| \
	TRACE_INVALID		| \
	TRACE_VALID		| \
	TRACE_NONE)

#if (DRDBM_DEBUG & TRACE_MSG)
    #define _MSG(x)		(x)
#else
    #define _MSG(x)
#endif
#if (DRDBM_DEBUG & TRACE_INIT)
    #define _INIT(x)		(x)
#else
    #define _INIT(x)
#endif
#if (DRDBM_DEBUG & TRACE_SPI)
    #define _SPI(x)		(x)
#else
    #define _SPI(x)
#endif
#if (DRDBM_DEBUG & TRACE_CARRIER)
    #define _CARRIER(x)		(x)
#else
    #define _CARRIER(x)
#endif
#if (DRDBM_DEBUG & TRACE_INVALID)
    #define _INVALID(x)		(x)
#else
    #define _INVALID(x)
#endif
#if (DRDBM_DEBUG & TRACE_VALID)
    #define _VALID(x)		(x)
#else
    #define _VALID(x)
#endif

/*
 *
 */
static struct frdbm		frdbm_t;

/*
 *
 *
 *
 */
static int spiflash_init(void)
{
	gpio_drving_init_io(IO_C1, 3);
	gpio_drving_init_io(IO_C2, 3);
	gpio_drving_init_io(IO_C3, 3);

	SPI_Flash_init();
	return 0;
}

static int spiflash_erase(unsigned long addr, unsigned long size, unsigned char *buf)
{
	int		ret;
	unsigned long	i, n;

	_SPI(DBG_PRINT("spifl - erase\r\n"));
	n = size >> 12;
	for (i=0; i<n; i++) {
		ret = SPI_Flash_erase_sector(addr+i*FRDB_CARRIER_ALIGN);
		if (ret) {
			_SPI(DBG_PRINT("spifl - erase fail (%d)\r\n", ret));
			return ret;
		}
	}
	return 0;
}

static int spiflash_read(unsigned long addr, unsigned long size, unsigned char *buf)
{
	int		ret;
	unsigned long	i, n;
	unsigned char	*p;

	_SPI(DBG_PRINT("spifl - read\r\n"));
	p = buf;
	n = size >> 8;
	for (i=0; i<n; i++) {
		ret = SPI_Flash_read_page(addr+i*256, (p+i*256));
		if (ret) {
			_SPI(DBG_PRINT("spifl - read fail (%d)\r\n", ret));
			return ret;
		}
	}
	return 0;
}

static int spiflash_write(unsigned long addr, unsigned long size, unsigned char *buf)
{
	int		ret;
	unsigned long	i, n;
	unsigned char	*p;

	spiflash_erase(addr ,size, buf);

	_SPI(DBG_PRINT("spifl - write\r\n"));
	p = buf;
	n = size >> 8;
	for (i=0; i<n; i++) {
		ret = SPI_Flash_write_page(addr+i*256, (p+i*256));
		if (ret) {
			_SPI(DBG_PRINT("spifl - write fail (%d)\r\n", ret));
			return ret;
		}
	}
	return 0;
}


/*
 *****************************************************************************
 * frdb_init
 *
 * descriptions :
 * arguments	: spi flash address must be 4K alignment
 * returns	:  0 - pass
 *		: +1 - warning, 4K alignment
 *		: -1 - error, not memory enough
 *		: -2 - error, carrier initial error
 *		: -3 - error, frdb load error
 *****************************************************************************
 */
int frdb_init(unsigned long addr)
{
	int	ret = 0;
	int	i;


	/* initial function pointer */
	frdbm_t.frdb_carrier_init = spiflash_init;
	frdbm_t.frdb_erase	  = spiflash_erase;
	frdbm_t.frdb_load	  = spiflash_read;
	frdbm_t.frdb_store	  = spiflash_write;

	/* initial variable */
	frdbm_t.curr_rd		  = 0;
	frdbm_t.curr_wr		  = 0;
	frdbm_t.curr		  = 0;

	/* check address value */
	if (addr & ~(FRDB_CARRIER_ALIGN-1)) {
		ret  = 1;
	}
	frdbm_t.base   = FRDB_ALIGN(addr);
	frdbm_t.offset = FRDB_ALIGN(sizeof(struct frdb));
	_INIT(DBG_PRINT("base   = 0x%08x\r\n", (unsigned long) frdbm_t.base));
	_INIT(DBG_PRINT("size   = 0x%08x\r\n", sizeof(struct frdb)));
	_INIT(DBG_PRINT("offset = 0x%08x\r\n", frdbm_t.offset));

	/* allocation memory */
	for (i=0; i<FRDB_NUM; i++) {
		frdbm_t.db[i] = (struct frdb *) gp_malloc_align(frdbm_t.offset, 8);
		_INIT(DBG_PRINT("frdbm_t.db[%d] = 0x%08x\r\n", i, frdbm_t.db[i]));
		if (frdbm_t.db[i] == NULL) {
			_MSG(DBG_PRINT("not memory enough\r\n"));
			return -1;
		}
	}

	/* load database */
	if (frdbm_t.frdb_carrier_init()) {
		_MSG(DBG_PRINT("carrier initial error\r\n"));
		return -2;
	}

	for (i=0; i<FRDB_NUM; i++) {
		if (frdb_load(i)) {
			_MSG(DBG_PRINT("frdb load error\r\n"));
			return -3;
		}
	}
	return ret;
}

/*
 *
 */
int frdb_erase(unsigned long n)
{
	if (n >= FRDB_NUM) {
		_CARRIER(DBG_PRINT("spifl - erase n >= %0d error\r\n", FRDB_NUM));
		return 1;
	}

	frdbm_t.db[n]->lbp_valid = (-1);
	if (frdbm_t.frdb_erase(frdbm_t.base+frdbm_t.offset*n, frdbm_t.offset, (unsigned char *) frdbm_t.db[n])) {
		_CARRIER(DBG_PRINT("spifl - erase error\r\n"));
		return -1;
	}
	return 0;
}

int frdb_load(unsigned long n)
{
	if (n >= FRDB_NUM) {
		_CARRIER(DBG_PRINT("spifl - load n >= %0d error\r\n", FRDB_NUM));
		return 1;
	}

	if (frdbm_t.frdb_load(frdbm_t.base+frdbm_t.offset*n, frdbm_t.offset, (unsigned char *) frdbm_t.db[n])) {
		_CARRIER(DBG_PRINT("spifl - load error\r\n"));
		return -1;
	}
	_CARRIER(DBG_PRINT("frdbm_t.db[%d]            = 0x%08x\r\n", n, frdbm_t.db[n]));
	_CARRIER(DBG_PRINT("frdbm_t.db[%d]->id        = 0x%08x\r\n", n, frdbm_t.db[n]->id));
	_CARRIER(DBG_PRINT("frdbm_t.db[%d]->lbp_valid = 0x%08x\r\n", n, frdbm_t.db[n]->lbp_valid));
	return 0;
}

int frdb_store(unsigned long n)
{
	if (n >= FRDB_NUM) {
		_CARRIER(DBG_PRINT("spifl - store n >= %0d error\r\n", FRDB_NUM));
		return 1;
	}

	frdbm_t.db[n]->lbp_valid = 0x0000AA55;
	if (frdbm_t.frdb_store(frdbm_t.base+frdbm_t.offset*n, frdbm_t.offset, (unsigned char *) frdbm_t.db[n])) {
		_CARRIER(DBG_PRINT("spifl - store error\r\n"));
		return -1;
	}
	_CARRIER(DBG_PRINT("frdbm_t.db[%d]            = 0x%08x\r\n", n, frdbm_t.db[n]));
	_CARRIER(DBG_PRINT("frdbm_t.db[%d]->id        = 0x%08x\r\n", n, frdbm_t.db[n]->id));
	_CARRIER(DBG_PRINT("frdbm_t.db[%d]->lbp_valid = 0x%08x\r\n", n, frdbm_t.db[n]->lbp_valid));
	return 0;
}

unsigned long frdb_state(void)
{
	unsigned long	state;
	unsigned long	i;

	for (i=0, state=0; i<FRDB_NUM; i++) {
		state <<= 1;
		if (frdbm_t.db[i]->lbp_valid == 0x0000AA55) {
			state |= 0x01;
		}
	}
	return state;
}

/*
 * member variable
 */
unsigned long frdb_get_curr(void)
{
	return frdbm_t.curr;
}

unsigned long frdb_get_base(void)
{
	return frdbm_t.base;
}

unsigned long frdb_get_offset(void)
{
	return frdbm_t.offset;
}

void frdb_set_c_ident(unsigned long bmap)
{
	int	i;

	for (i=0; i<FRDB_NUM; i++) {
		if (frdbm_t.db[i]->lbp_valid == 0x0000AA55) {
			frdbm_t.db[i]->c_ident = 1;
		} else {
			frdbm_t.db[i]->c_ident = 0;
		}
	}
}

void frdb_set_c_train(unsigned long bmap)
{
	int	i;

	for (i=0; i<FRDB_NUM; i++) {
		if (bmap & 0x01) {
			frdbm_t.db[i]->c_train = 1;
		} else {
			frdbm_t.db[i]->c_train = 0;
		}
		bmap >>= 1;
	}
}

int frdb_is_valid(unsigned long n)
{
	if (n >= FRDB_NUM)
		return 0;

	if (frdbm_t.db[n]->lbp_valid == 0x0000AA55) {
		return 1;
	} else {
		return 0;
	}
}

/*
 * read/write pointer
 */
unsigned char *frdb_get_invalid(void)
{
	unsigned long	i;

	/* find invalid */
	for (i=0; i<FRDB_NUM; i++) {
		if (frdbm_t.db[frdbm_t.curr_wr]->lbp_valid == 0x0000AA55) {
			_INVALID(DBG_PRINT("%0d->", frdbm_t.curr_wr));
			if (++frdbm_t.curr_wr >= FRDB_NUM) {
				frdbm_t.curr_wr = 0;
			}
			_INVALID(DBG_PRINT("%0d ", frdbm_t.curr_wr));
		} else {
			break;
		}
	}
	frdbm_t.curr = frdbm_t.curr_wr;
	if (++frdbm_t.curr_wr >= FRDB_NUM) {
		frdbm_t.curr_wr = 0;
	}

	_INVALID(DBG_PRINT("find invalid = %0d\r\n", frdbm_t.curr));
	frdbm_t.db[frdbm_t.curr]->lbp_valid = 0;
	gp_memset((INT8S *) frdbm_t.db[frdbm_t.curr]->lbp, 0, (LBP_SZ*LBP_NUM));
	return frdbm_t.db[frdbm_t.curr]->lbp;
}

unsigned char *frdb_get_valid(void)
{
	unsigned long	i;

	/* find valid */
	for (i=0; i<FRDB_NUM; i++) {
		if (frdbm_t.db[frdbm_t.curr_wr]->lbp_valid != 0x0000AA55) {
			_VALID(DBG_PRINT("%0d->", frdbm_t.curr_rd));
			if (++frdbm_t.curr_rd >= FRDB_NUM) {
				frdbm_t.curr_rd = 0;
			}
			_VALID(DBG_PRINT("%0d ", frdbm_t.curr_rd));
		} else {
			break;
		}
	}
	i = frdbm_t.curr_rd;
	if (++frdbm_t.curr_rd >= FRDB_NUM) {
		frdbm_t.curr_rd = 0;
	}
	_VALID(DBG_PRINT("find valid = %0d\r\n", i));
	return frdbm_t.db[i]->lbp;
}

unsigned char *frdb_get_c_train(void)
{
	for (frdbm_t.curr=0; frdbm_t.curr<FRDB_NUM; frdbm_t.curr++) {
		if (frdbm_t.db[frdbm_t.curr]->c_train) {
			frdbm_t.db[frdbm_t.curr]->c_train = 0;
			goto valid;
		}
	}
	_VALID(DBG_PRINT("frdbm_t.db[%0d] = 0x%08p\r\n", frdbm_t.curr, NULL));
	return NULL;

valid:
	_VALID(DBG_PRINT("frdbm_t.db[%0d] = 0x%08p\r\n", frdbm_t.curr, frdbm_t.db[frdbm_t.curr]->lbp));
	return frdbm_t.db[frdbm_t.curr]->lbp;
}

unsigned char *frdb_get_c_ident(void)
{
	for (frdbm_t.curr=0; frdbm_t.curr<FRDB_NUM; frdbm_t.curr++) {
		if (frdbm_t.db[frdbm_t.curr]->c_ident) {
			frdbm_t.db[frdbm_t.curr]->c_ident = 0;
			goto valid;
		}
	}
	_VALID(DBG_PRINT("frdbm_t.db[%0d] = 0x%08p\r\n", frdbm_t.curr, NULL));
	return NULL;

valid:
	_VALID(DBG_PRINT("frdbm_t.db[%0d] = 0x%08p\r\n", frdbm_t.curr, frdbm_t.db[frdbm_t.curr]->lbp));
	return frdbm_t.db[frdbm_t.curr]->lbp;
}

