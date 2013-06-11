/*
 ******************************************************************************
 * display_options.c
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
 *	2013.05.15	T.C. Chiu <tc.chiu@zealtek.com.tw>
 *
 ******************************************************************************
 */

#include <ctype.h>
#include "application.h"

#define printf		print_string

/*
 * print data buffer in hex and ascii form to the terminal.
 *
 * data reads are buffered so that each memory address is only read once.
 * useful when displaying the contents of volatile registers.
 *
 * parameters:
 *    addr: Starting address to display at start of line
 *    data: pointer to data buffer
 *    width: data value width.  May be 1, 2, or 4.
 *    count: number of values to display
 *    linelen: Number of values to print per line; specify 0 for default length
 */
#define MAX_LINE_LENGTH_BYTES		(64)
#define DEFAULT_LINE_LENGTH_BYTES	(16)
int print_buffer(unsigned long addr, void *data, unsigned int width, unsigned int count, unsigned int linelen)
{
	/* linebuf as a union causes proper alignment */
	union linebuf {
		unsigned long	ui[MAX_LINE_LENGTH_BYTES/sizeof(unsigned long) + 1];
		unsigned short 	us[MAX_LINE_LENGTH_BYTES/sizeof(unsigned short) + 1];
		unsigned char	uc[MAX_LINE_LENGTH_BYTES/sizeof(unsigned char) + 1];
	} lb;
	int	i;

	if (linelen*width > MAX_LINE_LENGTH_BYTES)
		linelen = MAX_LINE_LENGTH_BYTES / width;
	if (linelen < 1)
		linelen = DEFAULT_LINE_LENGTH_BYTES / width;

	while (count) {
		printf("%08lx:", addr);

		/* check for overflow condition */
		if (count < linelen)
			linelen = count;

		/* copy from memory into linebuf and print hex values */
		for (i = 0; i < linelen; i++) {
			unsigned long	x;

			if (width == 4)
				x = lb.ui[i] = *((volatile unsigned long  *) data);
			else if (width == 2)
				x = lb.us[i] = *((volatile unsigned short *) data);
			else
				x = lb.uc[i] = *((volatile unsigned char  *) data);
			printf(" %0*lx", width*2, x);
			data = ((char *) data) + width;
		}

		/* print data in ASCII characters */
		for (i=0; i < linelen*width; i++) {
			if (!isprint(lb.uc[i]) || lb.uc[i] >= 0x80)
				lb.uc[i] = '.';
		}
		lb.uc[i] = '\0';
		printf("    %s\r\n", lb.uc);

		/* update references */
		addr  += linelen * width;
		count -= linelen;

//		if (ctrlc())
//			return -1;
	}

	return 0;
}


















