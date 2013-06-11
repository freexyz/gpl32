/*
* Purpose: NV PACKER access
*
* Author: Dominant
*
* Date: 2008/04/23
*
* Copyright Generalplus Corp. ALL RIGHTS RESERVED.
*
* Version : 1.00
* History :
*         1 2008/04/23 Created by Dominant.  v1.00
*/

#include "nvpack.h"


//=== This is for code configuration DON'T REMOVE or MODIFY it ===========//
#if (defined GPLIB_NVRAM_SPI_FLASH_EN) && (GPLIB_NVRAM_SPI_FLASH_EN == 1) //
//========================================================================//

static FLASHRSINDEX *NVP_RS_TABLE =NULL; //Index table of RES
static INT16U total_nvp_rs;
static INT16U nvpack_main_fd;
static INT32U nvp_User_area_position;
static INT32U nvpack_retrieval(INT16U nvpack_main_fd);
static INT16U nvpack_name2index(INT8U *inpack_upper_name);
INT16U nvpack_init(INT8U* nvpack_upper_name);
INT16S nvpack_uninit(void);
INT16U nvp_fopen(INT8U* inpack_upper_name);
INT32S nvp_fread(INT16U nvp_infd, INT32U pbuf, INT32U byte_size);
INT16S nvp_fclose(INT16U nvp_infd);
INT32U nvp_lseek(INT16U nvp_infd, INT32S foffset);
INT32U nvp_fsize_get(INT16U nvp_infd);

#if 0
void nvpack_test(void)
{
    INT16U nvpack_infd;

    /* initial nv pack's main body */
    nvpack_init((INT8U*) "NVPACKER.NVP");
    nvpack_infd=nvp_fopen((INT8U*)"GPRS.PAK");
    nvp_fread(nvpack_infd,(INT32U)TEST_BUF, 512);
    
    
    nvpack_uninit();
}
#endif

INT16U nvpack_init(INT8U* nvpack_upper_name)
{
    nvpack_main_fd = nv_open((INT8U *) nvpack_upper_name);
    nvpack_retrieval(nvpack_main_fd);
	return nvpack_main_fd;
}



INT16S nvpack_uninit(void)
{
   if (NVP_RS_TABLE !=NULL) 
   {
      gp_free((void *) NVP_RS_TABLE);
      NVP_RS_TABLE =NULL;
      return STATUS_OK;
   }
   return STATUS_FAIL;
}

INT16U nvp_fopen(INT8U* inpack_upper_name)
{
    INT16U nvp_infile_idx;

    nvp_infile_idx=nvpack_name2index(inpack_upper_name);
    NVP_RS_TABLE[nvp_infile_idx].offset = 0;
    return nvp_infile_idx;
}


#define NVP_ALIGN_DUMMY_BYTE  0x00
#define NVP_DATA_END_TOKEN    0x80

INT32U nvp_fsize_get(INT16U nvp_infd)
{
    INT8U *NvpBuffer;
    INT16S i;
    INT8U  data_pedding_token_get=FALSE;
    INT16S dummy_count=0;
    INT32U ret;

    NvpBuffer = (INT8U *) gp_malloc_align(4096, 16);
    /* remove dummy bytes */
    nv_lseek(nvpack_main_fd, NVP_RS_TABLE[nvp_infd].pos*512+(NVP_RS_TABLE[nvp_infd].size-1)*512, SEEK_SET);
    nv_read(nvpack_main_fd, (INT32U) NvpBuffer, 512); 

    for (i=0; i<512; i++)
    {
        if(NvpBuffer[511-i]==NVP_ALIGN_DUMMY_BYTE)
        {
            dummy_count++;
            continue;
        }
        else if (NvpBuffer[511-i]==NVP_DATA_END_TOKEN)
        {
            data_pedding_token_get=TRUE;
            dummy_count++;
            break;
        }
        else
        {
            break;
        }
    }
    
    if (data_pedding_token_get==FALSE) {dummy_count=0;}

    gp_free((void *) NvpBuffer);
    NVP_RS_TABLE[nvp_infd].offset = 0;
    ret = (NVP_RS_TABLE[nvp_infd].size*512-dummy_count);
    return ret;
}

INT16S nvp_fclose(INT16U nvp_infd)
{
	return STATUS_OK;
}

INT32U nvp_lseek(INT16U nvp_infd, INT32S foffset)
{
    NVP_RS_TABLE[nvp_infd].offset = foffset;
    return STATUS_OK;
}

INT32S nvp_fread(INT16U nvp_infd, INT32U pbuf, INT32U byte_size)
{
    INT32U inpk_fsize;
    INT32S ret;

    inpk_fsize = NVP_RS_TABLE[nvp_infd].size*512;
    if (NVP_RS_TABLE[nvp_infd].offset >= inpk_fsize) {return STATUS_OK;}

    nv_lseek(nvpack_main_fd, NVP_RS_TABLE[nvp_infd].pos*512+NVP_RS_TABLE[nvp_infd].offset, SEEK_SET);
    nv_read(nvpack_main_fd, pbuf, byte_size);   
    NVP_RS_TABLE[nvp_infd].offset+=byte_size; 

    if (NVP_RS_TABLE[nvp_infd].offset >= inpk_fsize)
    {
        ret = byte_size - (NVP_RS_TABLE[nvp_infd].offset - inpk_fsize);
        //NVP_RS_TABLE[nvp_infd].offset=0;
        //NVP_RS_TABLE[nvp_infd].offset+=ret;
       // return ret;
    }
    return STATUS_OK;
}

INT16U nvpack_name2index(INT8U *inpack_upper_name)
{
  INT16U i,j ;
  for (i =0 ; i<total_nvp_rs; i++) {
    for (j =0 ; j<19; j++) {
      if (*(inpack_upper_name+j) == '.') inpack_upper_name++; //by pass [.]
      if ( (NVP_RS_TABLE[i].name[j] == 0x20) && (j >3) )  {return  i ;}
      if (NVP_RS_TABLE[i].name[j] != *(inpack_upper_name+j)) {break;}
    }
  }
  return 0xffff;
}

INT32U nvpack_retrieval(INT16U nvpack_main_fd)
{
   INT32U i,j,temp;
   INT16U *PageBuffer;
   INT16U Rspages;

   //Geting Head
   PageBuffer = (INT16U *) gp_malloc_align(2048, 16);
   temp =0 ;

   nv_lseek(nvpack_main_fd,0,SEEK_SET);
   nv_read(nvpack_main_fd, (INT32U) PageBuffer, 2048);       

  total_nvp_rs = (INT16U) combin_reg_data((INT8U *)PageBuffer + 2 ,2);  
   if (NVP_RS_TABLE !=NULL) 
   {
      gp_free((void *) NVP_RS_TABLE);
      NVP_RS_TABLE =NULL;
   }
   NVP_RS_TABLE  =  (FLASHRSINDEX *) gp_malloc(((total_nvp_rs+1)* sizeof(FLASHRSINDEX)));   

#if (RESOURCE_DEV_TYPE != DEVICE_SPI) && (NAND1_EN == 1) 
    Rspages = (((((total_nvp_rs*23 +4)-1) / 512)+1) -1 ) / nand_sector_nums_per_page_get() +1;
    if (Rspages >1)  /* 若檔案 header 大於1個 page , 需重新 allocate 新 memory */
    {
        gp_free((void *) PageBuffer);
        PageBuffer = (INT16U *) gp_malloc_align(Rspages*512*nand_sector_nums_per_page_get(), 16);
        nv_lseek(nvpack_main_fd,0,SEEK_SET);
        nv_read(nvpack_main_fd, (INT32U) PageBuffer, Rspages*nand_sector_nums_per_page_get()*512);  
    }
#else
    Rspages = ((total_nvp_rs*23 +4)/2048+1);
    if (Rspages>1)
    {
        gp_free((void *) PageBuffer);
        PageBuffer = (INT16U *) gp_malloc_align(Rspages*2048, 16);
        nv_lseek(nvpack_main_fd,0,SEEK_SET);
        nv_read(nvpack_main_fd, (INT32U) PageBuffer, Rspages*2048);  
    }
#endif

    for (i =0 ; i<total_nvp_rs+1; i++) {
     for (j =0 ; j<19; j++)
        NVP_RS_TABLE[i].name[j] = *((INT8U *)PageBuffer + j+4 +(23*i) );
        NVP_RS_TABLE[i].pos = (INT32U) combin_reg_data((INT8U *)PageBuffer +23+ (23*i),4);
    }
    // The least pos is User area position
    nvp_User_area_position = NVP_RS_TABLE[i-1].pos;
    for (i =0 ; i<total_nvp_rs; i++) {
      NVP_RS_TABLE[i].size =  NVP_RS_TABLE[i+1].pos - NVP_RS_TABLE[i].pos ;
    }
   gp_free((void *) PageBuffer);
   return STATUS_OK;
}

//=== This is for code configuration DON'T REMOVE or MODIFY it =================//
#endif  //(defined GPLIB_NVRAM_SPI_FLASH_EN) && (GPLIB_NVRAM_SPI_FLASH_EN == 1) //
//==============================================================================//
