#include "fsystem.h"

//初始化unicode转换程序
static short  viet_init_nls(void)
{
	return 0;
}

//退出unicode转换程序
static short viet_exit_nls(void)
{
	return 0;
}

//把unicode转换为本地编码的字符
//parameter:  unicode
//return:     oem character
//note:       if error, return 0
static unsigned short viet_uni2char(unsigned short uni)
{
	
	
	
	
	return uni;
}

//把本地编码的字符转换为unicode
//parameter:  oem character
//return:     unicode
//note:       if error, return 0
//参数**rawstring指向一组byte型的字符串。
//注意某些字符由2个byte组成，则转换完成后需要把指针*rawstring加上2
//如果字符只由1个byte组成，则转换为unicode后只要把*rawstring加上1
static unsigned short viet_char2uni(unsigned char **rawstring)
{
	unsigned char *temp = *rawstring;
	
	*rawstring += 1;        //the point add 1
	
	
	
	
	return *temp;
}

const struct nls_table nls_viet_table = {
	"viet",
	0, 
	viet_init_nls,
	viet_exit_nls,
	viet_uni2char,
	viet_char2uni,
};

