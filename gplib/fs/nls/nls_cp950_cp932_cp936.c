#include "gplib.h"

extern INT8U unitab_GBK[];
extern INT8U acunitab_GBK_BIG5[];
extern INT8U acunitab_GBK_BIG5_sjis[];

static	INT32U	pUniTabAdd;
static	INT32U	pUniToCharTab;		//UniToCharTab在整个unitab中的偏移量
static	INT32U	pCharToUniTab;		//CharToUniTab在整个unitab中的偏移量

INT32U fs_read_dword(INT32U p)
{
	INT32U rval;
	
	((INT8U *) &rval) [3] = ((INT8U *) p) [3];
	((INT8U *) &rval) [2] = ((INT8U *) p) [2];
	((INT8U *) &rval) [1] = ((INT8U *) p) [1];
	((INT8U *) &rval) [0] = ((INT8U *) p) [0];
	
	return (rval);
}

INT16U fs_read_word(INT32U p)
{
	INT16U rval;

	((INT8U *) &rval) [1] = ((INT8U *) p) [1];
	((INT8U *) &rval) [0] = ((INT8U *) p) [0];
	
	return (rval);
}

INT32U get_unitab_add(void)
{
	if(gUnicodePage == UNI_GBK)
	return (INT32U)unitab_GBK;
	else if(gUnicodePage == UNI_BIG5)
		return (INT32U)acunitab_GBK_BIG5;
	else if(gUnicodePage == UNI_SJIS)
		return (INT32U)acunitab_GBK_BIG5_sjis;
	else
		return (INT32U)unitab_GBK;;
}

static INT32U GetUni2OEMDataIndex(INT16U index)
{
	if (pUniToCharTab == 0)
		return 0;
	
	return fs_read_dword(pUniToCharTab + ((INT32U)index << 2)); 
}

static INT32U GetOEM2UniDataIndex(INT16U index)
{
	if (pCharToUniTab == 0)
		return 0;
	
	return fs_read_dword(pCharToUniTab + ((INT32U)index << 2)); 
}

static INT16U GetUniTabData(INT32U index)
{
	return fs_read_word(pUniTabAdd + (index << 1));
}

static INT16S  cpxxx_init_nls(INT32U wCodePage)
{
	INT32U Flag1, Flag2;
	
	pUniTabAdd = get_unitab_add();
	if (pUniTabAdd == 0)
		return -1;
	
	pCharToUniTab = fs_read_dword(pUniTabAdd + (wCodePage << 3));
	pUniToCharTab = fs_read_dword(pUniTabAdd + 4 + (wCodePage << 3));
	Flag1 = fs_read_dword(pUniTabAdd + 15 * 8);
	Flag2 = fs_read_dword(pUniTabAdd + 15 * 8 + 4);
	
	if ((Flag1 != 0x1001) || (Flag2 != 0x55aa) || (pCharToUniTab == 0) || (pUniToCharTab == 0))
	{
		pCharToUniTab = 0;
		pUniToCharTab = 0;
		return -1;
	}
	pCharToUniTab = pUniTabAdd + (pCharToUniTab << 1);
	pUniToCharTab = pUniTabAdd + (pUniToCharTab << 1);
	return 0;
}

static INT16S cpxxx_exit_nls(void)
{
//	ReleaseUniTabData();
	pUniToCharTab = 0;		//UniToCharTab在整个unitab中的偏移量
	pCharToUniTab = 0;		//CharToUniTab在整个unitab中的偏移量
	return 0;
}

// returns the character from unicode
// return 0 is error
static INT16U cpxxx_uni2char(INT16U uni)
{
	INT8U cl = uni&0xFF;
	INT8U ch = (uni>>8)&0xFF;
	INT32U idx;	
	
	if ((INT16U)uni < (INT16U)0x80 )	// 遇到ASCII码,不用查表直接返回!!
	{	
		return uni;
	}
	
	idx = GetUni2OEMDataIndex(ch);
	if (idx)
		return GetUniTabData(cl+idx);
	else if ((ch == 0) && cl)
		return cl;
	else
		return 0;
}

// returns the unicode from character
// return 0 is error
static INT16U cpxxx_char2uni(INT8U **rawstring)
{
	INT8U	ch, cl;
	INT16U	uni;
	INT32U	idx;
	INT8U	*temp = *rawstring;
	
	ch = temp[0];
	cl = temp[1];
	
	idx = GetOEM2UniDataIndex(ch);
	
	if (idx && cl)
	{
		uni = GetUniTabData(cl+idx); 		
		*rawstring += 2;				//the char point increase 2
	} 
	else
	{
		uni = ch;
		*rawstring += 1;				//the char point increase 1
	}
	
	return uni; 		 //success
}

static INT16S  cp936_init_nls(void)
{
	return  cpxxx_init_nls(UNI_GBK);
}

static INT16S  cp950_init_nls(void)
{
	return  cpxxx_init_nls(UNI_BIG5);
}

static INT16S  cp932_init_nls(void)
{
	return  cpxxx_init_nls(UNI_SJIS);
}

//GBK/cp936
const struct nls_table nls_cp936_table = {
	"GBK",
	0, 
	cp936_init_nls,
	cpxxx_exit_nls,
	cpxxx_uni2char,
	cpxxx_char2uni,
};

//BIG5/cp950
const struct nls_table nls_cp950_table = {
	"BIG5",
	0, 
	cp950_init_nls,
	cpxxx_exit_nls,
	cpxxx_uni2char,
	cpxxx_char2uni,
};

//SJIS/cp932
const struct nls_table nls_cp932_table = {
	"SJIS",
	0, 
	cp932_init_nls,
	cpxxx_exit_nls,
	cpxxx_uni2char,
	cpxxx_char2uni,
};

