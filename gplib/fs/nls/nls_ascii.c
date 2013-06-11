#include "fsystem.h"

static short  ascii_init_nls(void)
{
	return 0;
}

static short ascii_exit_nls(void)
{
	return 0;
}

// returns the ascii character from unicode
// return 0 is error
static unsigned short ascii_uni2char(unsigned short uni)
{
	if (uni >= 0x80)
		return 0;
	return uni;
}

// returns the unicode from ascii character
// return 0 is error
static unsigned short ascii_char2uni(unsigned char **rawstring)
{
	unsigned char *temp = *rawstring;
	
	*rawstring += 1;        //the point add 1
	if (*temp >= 0x80)	
		return 0;			//error
	return *temp;
}

const struct nls_table nls_ascii_table = {
	"ascii",
	0, 
	ascii_init_nls,
	ascii_exit_nls,
	ascii_uni2char,
	ascii_char2uni,
};

