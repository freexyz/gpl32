#include "fsystem.h"
#include	"Nand_Oper.h"

extern INT16S	bNandInit;

extern INT32U fs_read_dword(INT32U addr);

static	INT32U	pUniTabAdd;
static	INT32U	pUniToCharTab;			//UniToCharTab在整个unitab中的偏移量
static	INT32U	pCharToUniTab;			//CharToUniTab在整个unitab中的偏移量
static	INT16U	gUnitabBuffer[1024];	//为加速访问UNITAB定义的buffer

UDWORD GetUni2OEMDataIndex(UWORD index)
{
	if (pUniToCharTab ==0)
		return 0;
	
	return fs_read_dword(gUnitabBuffer + 512 + index * 2);
}

UDWORD GetOEM2UniDataIndex(UWORD index)
{
	if (pCharToUniTab == 0)
		return 0;
	
	return fs_read_dword(gUnitabBuffer + index * 2); 
}

INT16U GetUniTabData(INT32U index)
{
	INT16U	value;
	INT32S	ret;
	
	ret = (UNITAB_SECTOR_OFFSET * 256 + index) << 1;
	_Nand_Direct_Read(ret, (INT32U)&value, 2);
	
	return value;
}

static INT16S cpxxx_init_nls(INT16U wCodePage)
{
	INT32U	Addr;
	INT32U	Flag1, Flag2;
		
	if (bNandInit != 0)
		return -1;
	
	Addr = UNITAB_SECTOR_OFFSET * 512;
	
	_Nand_Direct_Read(Addr + 8 * wCodePage, (INT16U)&pCharToUniTab, 4);
	_Nand_Direct_Read(Addr + 8 * wCodePage + 4, (INT16U)&pUniToCharTab, 4);
	_Nand_Direct_Read(Addr + 15 * 8, (INT32U)&Flag1, 4);
	_Nand_Direct_Read(Addr + 15 * 8 + 4, (INT32U)&Flag2, 4);
	if ((Flag1 != 0x1001) || (Flag2 != 0x55aa) || (pCharToUniTab == 0) || (pUniToCharTab == 0))
	{
		pCharToUniTab = 0;
		pUniToCharTab = 0;
		return -1;
	}
	
	_Nand_Direct_Read(Addr + pCharToUniTab * 2, gUnitabBuffer, 512);
	_Nand_Direct_Read(Addr + pCharToUniTab * 2 + 512, gUnitabBuffer + 256, 512);
	_Nand_Direct_Read(Addr + pUniToCharTab * 2, gUnitabBuffer + 512, 512);
	_Nand_Direct_Read(Addr + pUniToCharTab * 2 + 512, gUnitabBuffer + 768, 512);

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
	INT8U	cl = uni & 0x00FF;
	INT8U	ch = (uni >> 8) & 0xFF;
	INT32U	idx;
	
	if ((INT16U)uni < (INT16U)0x80 )	// 遇到ASCII码,不用查表直接返回!!
	{	
		return uni;
	}
	
	idx = GetUni2OEMDataIndex(ch);
	if (idx)
		return GetUniTabData(cl + idx);
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
	INT32U	idx, idx1;
	INT8U	*temp = *rawstring;
	
	ch = temp[0];
	cl = temp[1];
	
#ifdef unSP
	if ( (ch > 0xff) || (cl > 0xff) )        //2007.1.25 to avoid invalid data 
    { 
		return 0;					//error 
    } 
#endif
	idx1 = GetOEM2UniDataIndex(0);
	
	idx = GetOEM2UniDataIndex(ch);
	
	if (idx) {
		uni = GetUniTabData(cl+idx); 		
		*rawstring += 2;				//the char point increase 2
	}  else if (ch >= 0x80 && idx1) {
		uni = GetUniTabData(ch+idx1); 
		*rawstring += 1;				//the char point increase 1
	} else {
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

