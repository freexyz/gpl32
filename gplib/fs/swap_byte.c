#include "fsystem.h"
#include "globals.h"

#ifdef unSP	
INT8U GetBytefromWord(void *p, INT16S pos)
{
	 INT8U rval;
	
	 rval = (( INT16U *) p) [pos>>WORD_PACK_VALUE];

	 if (pos & 0x1)
	 {
		 rval = rval >> 8;
	 }
	 else
	 {
		 rval = rval & 0xFF;
	 }
	
	 return (rval);
}

INT16U getWordfromWord(void *p, INT16S pos)
{
	 INT16U rval;
	 INT16U * pdata;
	
	 pdata = &(( INT16U *) p) [pos>>WORD_PACK_VALUE];
	
	 if (pos & 0x1)
	 {
		 rval = (*pdata) >> 8;
		 rval |= (*++pdata) << 8;
	 }
	 else
	 {
		 rval = *pdata;
	 }
	
	 return (rval);
}

INT32U getLongfromWord(void *p, INT16S pos)
{
	 INT32U rval;
	
	 p = p + (pos>>1);
	
	 if (pos & 0x1)
	 {
		 (( INT16U *) &rval) [0] = (*( INT16U *) p) >> 8;
		 p++;
		 (( INT16U *) &rval) [0] |= (*( INT16U *) p) << 8;
		 (( INT16U *) &rval) [1] = (*( INT16U *) p) & 0xFF;
		 (( INT16U *) &rval) [1] |= (*( INT16U *) p) & 0xFF00;
	 }
	 else
	 {
		 (( INT16U *) &rval) [0] = (*( INT16U *) p);
		 p++;
		 (( INT16U *) &rval) [1] = (*( INT16U *) p);
	 }

	 return (rval);
}
#endif

#ifdef BIG_ENDIAN
INT32U getlong(void *p)
{
	INT32U rval;
	
	((INT8U *) &rval) [3] = ((INT8U *) p) [0];
	((INT8U *) &rval) [2] = ((INT8U *) p) [1];
	((INT8U *) &rval) [1] = ((INT8U *) p) [2];
	((INT8U *) &rval) [0] = ((INT8U *) p) [3];
	return (rval);
}

INT16U getword(void *p)
{
	INT16U rval;

	((INT8U *) &rval) [1] = ((INT8U *) p) [0];
	((INT8U *) &rval) [0] = ((INT8U *) p) [1];
	
	return (rval);
}

void putlong(void *p, INT32U val)
{
	((INT8U *) p) [3] = ((INT8U *) &val) [0];
	((INT8U *) p) [2] = ((INT8U *) &val) [1];
	((INT8U *) p) [1] = ((INT8U *) &val) [2];
	((INT8U *) p) [0] = ((INT8U *) &val) [3];
}

void putword(void *p, INT16U val)
{
	((INT8U *) p) [1] = ((INT8U *) &val) [0];
	((INT8U *) p) [0] = ((INT8U *) &val) [1];
}

/* Long File Name Endian Switch Table*/
INT8U const tLfnEntry_endian_table[] =
{0,2,1,4,3,6,5,8,7,10,9,11,12,13,15,14,17,16,19,18,21,20,23,22,25,24,27,26,29,28,31,30};

/* Short File Name Endian Switch Table*/
INT8U const tDirEntry_endian_table[] =
{0,1,2,3,4,5,6,7,8,9,10,11,12,13,15,14,17,16,19,18,21,20,23,22,25,24,27,26,31,30,29,28};

void getdirent(INT8U * bp, struct dirent * dp)
{
	INT8U i;
	INT8U const *tp;
	if (((INT8U *) bp)[11] == 0x0f)//bp->Attr
		{tp = tLfnEntry_endian_table;}
	else
		tp = tDirEntry_endian_table;
	for (i=0;i<=31;i++)
		((INT8U *) dp)[i]  = ((INT8U *) bp)[tp[i]] ;
}

void putdirent(struct dirent *dp, INT8U *bp)
{
	INT8U i;
	INT8U const *tp;
	if (((INT8U *) dp)[11] == 0x0f)//dp->Attr
		tp = tLfnEntry_endian_table;
	else
		tp = tDirEntry_endian_table;
	for (i=0;i<=31;i++)
		((INT8U *) bp)[i]  = ((INT8U *) dp)[tp[i]] ;
}

#elif defined LITTLE_ENDIAN /*little endian but align 4 bytes*/
INT32U getlong(void *p)
{
	INT32U rval;
	
	((INT8U *) &rval) [3] = ((INT8U *) p) [3];
	((INT8U *) &rval) [2] = ((INT8U *) p) [2];
	((INT8U *) &rval) [1] = ((INT8U *) p) [1];
	((INT8U *) &rval) [0] = ((INT8U *) p) [0];
	
	return (rval);
}

INT16U getword(void *p)
{
	INT16U rval;

	((INT8U *) &rval) [1] = ((INT8U *) p) [1];
	((INT8U *) &rval) [0] = ((INT8U *) p) [0];
	
	return (rval);
}

void putlong(void *p, INT32U val)
{
	((INT8U *) p) [3] = ((INT8U *) &val) [3];
	((INT8U *) p) [2] = ((INT8U *) &val) [2];
	((INT8U *) p) [1] = ((INT8U *) &val) [1];
	((INT8U *) p) [0] = ((INT8U *) &val) [0];
}

void putword(void *p, INT16U val)
{
	((INT8U *) p) [1] = ((INT8U *) &val) [1];
	((INT8U *) p) [0] = ((INT8U *) &val) [0];
}

void fs_memset_word(void* p, INT16U val, INT32U size)
{
	INT8U*	ptmp = (INT8U*)p;
	
	while(size--)
	{
		putword(ptmp, val);
		ptmp += 2;
	}
}

#elif defined unSP
INT32U getlong(void *p)
{
	INT32U rval;
	
	(( INT16U *) &rval) [0] = (( INT16U *) p) [0];
	(( INT16U *) &rval) [1] = (( INT16U *) p) [1];
	
	return (rval); 			  		 	
}

INT16U getword(void *p)
{
	INT16U rval;
	
	(( INT16U *) &rval) [0] = (( INT16U *) p) [0];
	return (rval);
}

void putlong(void *p, INT32U val)
{
	(( INT16U *) p) [0] = (( INT16U *) &val) [0];
	(( INT16U *) p) [1] = (( INT16U *) &val) [1];
}

void putword(void *p, INT16U val)
{
	(( INT16U *) p) [0] = (( INT16U *) &val) [0];
}

void getdirent(INT8U *bp, struct dirent *dp)
{	
	dp->dir_name[0] = bp[0] & 0xFF;
	dp->dir_name[1] = bp[0] >> 8;
	dp->dir_name[2] = bp[1] & 0xFF;
	dp->dir_name[3] = bp[1] >> 8;
	dp->dir_name[4] = bp[2] & 0xFF;
	dp->dir_name[5] = bp[2] >> 8;
	dp->dir_name[6] = bp[3] & 0xFF;
	dp->dir_name[7] = bp[3] >> 8;
	dp->dir_name[8] = bp[4] & 0xFF;
	dp->dir_name[9] = bp[4] >> 8;
	dp->dir_name[10] = bp[5] & 0xFF;
	
	dp->dir_attrib = bp[5] >> 8;
	dp->dir_case =  bp[6] & 0xFF;
	dp->dir_crtimems = bp[6] >> 8;
	
	dp->dir_crtime = bp[7];
	dp->dir_crdate = bp[8];
	dp->dir_accdate = bp[9];
	dp->dir_start_high = bp[10];
	dp->dir_time = bp[11];
	dp->dir_date = bp[12];
	dp->dir_start = bp[13];
	(*(INT16U *)&dp->dir_size) = bp[14];
	*(((INT16U *)&dp->dir_size)+1) = bp[15];
}

void putdirent(struct dirent *dp, INT8U *bp)
{
	bp[0] = (dp->dir_name[1] << 8) | (dp->dir_name[0]&0xFF);
	bp[1] = (dp->dir_name[3] << 8) | (dp->dir_name[2]&0xFF);
	bp[2] = (dp->dir_name[5] << 8) | (dp->dir_name[4]&0xFF);
	bp[3] = (dp->dir_name[7] << 8) | (dp->dir_name[6]&0xFF);
	bp[4] = (dp->dir_name[9] << 8) | (dp->dir_name[8]&0xFF);
	bp[5] = (dp->dir_attrib << 8) | (dp->dir_name[10]&0xFF);
	
	bp[6] = (dp->dir_crtimems << 8) | (dp->dir_case&0xFF);
	bp[7] = dp->dir_crtime;
	bp[8] = dp->dir_crdate;
	bp[9] = dp->dir_accdate;
	bp[10] = dp->dir_start_high;
	bp[11] = dp->dir_time;
	bp[12] = dp->dir_date;
	bp[13] = dp->dir_start;
	bp[14] = dp->dir_size & 0xFFFF;
	bp[15] = *(((INT16U *)&dp->dir_size)+1);
}

void getLfnEnt(tDirEntry *pDirEnt, tLfnEntry *pLfnEnt)
{
	pLfnEnt->Order = pDirEnt->Name[0];
	pLfnEnt->Name0_4[0] = (pDirEnt->Name[2]<<8)|pDirEnt->Name[1];
	pLfnEnt->Name0_4[1] = (pDirEnt->Name[4]<<8)|pDirEnt->Name[3];
	pLfnEnt->Name0_4[2] = (pDirEnt->Name[6]<<8)|pDirEnt->Name[5];
	pLfnEnt->Name0_4[3] = (pDirEnt->Name[8]<<8)|pDirEnt->Name[7];
	pLfnEnt->Name0_4[4] = (pDirEnt->Name[10]<<8)|pDirEnt->Name[9];
	pLfnEnt->Attr = pDirEnt->Attr;
	pLfnEnt->Reserved = pDirEnt->NTRes;
	pLfnEnt->Checksum = pDirEnt->CrtTimeTenth;
	pLfnEnt->Name5_10[0] = pDirEnt->CrtTime;
	pLfnEnt->Name5_10[1] = pDirEnt->CrtDate;
	pLfnEnt->Name5_10[2] = pDirEnt->LstAccDate;
	pLfnEnt->Name5_10[3] = pDirEnt->FstClusHI;
	pLfnEnt->Name5_10[4] = pDirEnt->WrtTime;
	pLfnEnt->Name5_10[5] = pDirEnt->WrtDate;
	pLfnEnt->FstClus = pDirEnt->FstClusLO;
	pLfnEnt->Name11_12[0] = *((INT16U *)&pDirEnt->FileSize);
	pLfnEnt->Name11_12[1] = *(((INT16U *)&pDirEnt->FileSize)+1);
}

void putLfnEnt(tLfnEntry *pLfnEnt, tDirEntry *pDirEnt)
{
	pDirEnt->Name[0] = pLfnEnt->Order;
	pDirEnt->Name[1] = pLfnEnt->Name0_4[0] & 0xFF;
	pDirEnt->Name[2] = pLfnEnt->Name0_4[0] >> 8;
	pDirEnt->Name[3] = pLfnEnt->Name0_4[1] & 0xFF;
	pDirEnt->Name[4] = pLfnEnt->Name0_4[1] >> 8;
	pDirEnt->Name[5] = pLfnEnt->Name0_4[2] & 0xFF;
	pDirEnt->Name[6] = pLfnEnt->Name0_4[2] >> 8;
	pDirEnt->Name[7] = pLfnEnt->Name0_4[3] & 0xFF;
	pDirEnt->Name[8] = pLfnEnt->Name0_4[3] >> 8;
	pDirEnt->Name[9] = pLfnEnt->Name0_4[4] & 0xFF;
	pDirEnt->Name[10] = pLfnEnt->Name0_4[4] >> 8;
	pDirEnt->Attr = pLfnEnt->Attr;
	pDirEnt->NTRes = pLfnEnt->Reserved;
	pDirEnt->CrtTimeTenth = pLfnEnt->Checksum;
	pDirEnt->CrtTime = pLfnEnt->Name5_10[0];
	pDirEnt->CrtDate = pLfnEnt->Name5_10[1];
	pDirEnt->LstAccDate = pLfnEnt->Name5_10[2];
	pDirEnt->FstClusHI = pLfnEnt->Name5_10[3];
	pDirEnt->WrtTime = pLfnEnt->Name5_10[4];
	pDirEnt->WrtDate = pLfnEnt->Name5_10[5];
	pDirEnt->FstClusLO = pLfnEnt->FstClus;
	
	*((INT16U *)&pDirEnt->FileSize) = pLfnEnt->Name11_12[0];
	*(((INT16U *)&pDirEnt->FileSize)+1) = pLfnEnt->Name11_12[1];
}

#endif /*define ALIGN & NATIVE*/
