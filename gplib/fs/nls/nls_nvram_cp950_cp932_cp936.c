#include "gplib.h"

static	INT16U	unitab_file_handle;
//static	INT32U	pUniTabAdd;
static	INT32U	pUniToCharTab;			//UniToCharTab在整个unitab中的偏移量
static	INT32U	pCharToUniTab;			//CharToUniTab在整个unitab中的偏移量
static	INT32U	gUnitabBuffer[512];		//为加速访问UNITAB定义的buffer

INT32U GetUni2OEMDataIndex(INT16U index)
{
	if (pUniToCharTab ==0)
		return 0;
	
	return gUnitabBuffer[256 + index];	//fs_read_dword(gUnitabBuffer + 512 + index * 2);
}

INT32U GetOEM2UniDataIndex(INT16U index)
{
	if (pCharToUniTab == 0)
		return 0;
	
	return gUnitabBuffer[index];	//fs_read_dword(gUnitabBuffer + index * 2); 
}

INT16U GetUniTabData(INT32U index)
{
	INT16U	value;
	
	if(unitab_file_handle == 0xffff)
		return 0;
	
	nv_lseek(unitab_file_handle, index << 1, SEEK_SET);
	nv_read(unitab_file_handle, (INT32U)&value, 2);
	return value;
}

static INT16S cpxxx_init_nls(INT16U wCodePage)
{
	INT32U	Flag1, Flag2;
	
	unitab_file_handle = nv_open((INT8U*)"UNITAB.RS");
	if(unitab_file_handle == 0xffff)
	{
		return -1;
	}
	
	nv_lseek(unitab_file_handle, wCodePage * 8, SEEK_SET);
	nv_read(unitab_file_handle, (INT32U)&pCharToUniTab, 4);
	nv_read(unitab_file_handle, (INT32U)&pUniToCharTab, 4);
	
	nv_lseek(unitab_file_handle, 15 * 8, SEEK_SET);
	nv_read(unitab_file_handle, (INT32U)&Flag1, 4);
	nv_read(unitab_file_handle, (INT32U)&Flag2, 4);
	if ((Flag1 != 0x1001) || (Flag2 != 0x55aa) || (pCharToUniTab == 0) || (pUniToCharTab == 0))
	{
		pCharToUniTab = 0;
		pUniToCharTab = 0;
		return -1;
	}
	
	nv_lseek(unitab_file_handle, pCharToUniTab * 2, SEEK_SET);
	nv_read(unitab_file_handle, (INT32U)gUnitabBuffer, 1024);
	nv_lseek(unitab_file_handle, pUniToCharTab * 2, SEEK_SET);
	nv_read(unitab_file_handle, (INT32U)(gUnitabBuffer + 256), 1024);

	return 0;
}

static INT16S cpxxx_exit_nls(void)
{
//	ReleaseUniTabData();
	unitab_file_handle = 0xffff;
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

