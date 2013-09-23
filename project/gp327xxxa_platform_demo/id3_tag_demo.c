#include "gplib.h"

#define USE_DISK	FS_SD
//#define USE_DISK	FS_NAND1	
//#define USE_DISK	FS_USBH
#define GBK_SUPPORT  0
#define Demo_Message	print_string 

static void ID3_Tag_Test(void);

/*==================================================================
unicode sent to console
====================================================================*/
static int  unicode_filename_strlen(char *unicode)
{
	unsigned short *temp=(unsigned short *)unicode;
	int count=0;
	while(*temp!=0x0000)
	{
		temp++;
		count +=2;
	}
	return count;
}
extern INT16S oemuni_to_utf8(INT8S *Chart, INT8U *UTF8Chart);
static void sent_unicode_consol(char *unicode)
{
	INT8U *utf8;
	int utf8_len;
	int ret=0;
	utf8_len=unicode_filename_strlen(unicode)*3/2;
	utf8=gp_malloc(utf8_len);
	ret=oemuni_to_utf8((INT8S *)unicode,utf8);
	if(ret==0)
		Demo_Message("%s",utf8);
	else
		Demo_Message("change to utf8 err");
	gp_free(utf8);
}
/*================================================================================================
GBK sent to console
==================================================================================================*/
#if GBK_SUPPORT
static void init_gbk()
{
	nls_cp936_table.init_nls();
}
static void exit_gbk()
{
	nls_cp936_table.exit_nls();
}
static void sent_gbk_consol(INT8U *gbk)
{
	INT16U  unicode[256];
	int  postion=0;
	INT8U **CharUnit =&gbk;
	//change gbk to unicode
	while(**CharUnit!=0)
       {
         	unicode[postion]=nls_cp936_table.char2uni(CharUnit);
		if(unicode[postion]==0)
			break;
		postion++;
       }
	unicode[postion]=0;
	sent_unicode_consol((char *)unicode);
	
}
#endif

//=================================================================================================
//	ID3 tag parser demo code
//=================================================================================================
void id3_tag_demo(void)
{
	while(1)
	{
		if(_devicemount(USE_DISK))					// Mount device
		{
			DBG_PRINT("Mount Disk Fail[%d]\r\n", USE_DISK);
	#if	USE_DISK == FS_NAND1
			nRet = _format(FS_NAND1, FAT32_Type);
			DrvNand_flush_allblk();	
			_devicemount(FS_NAND1); 		
	#endif									
		}
		else
		{
			DBG_PRINT("Mount Disk success[%d]\r\n", USE_DISK);
			break;
		}
	}

	ID3_Tag_Test();

  DBG_PRINT("ID3 tag parsing of all MP3 files is doneh\r\n");		
  while(1){    	    
	  OSTimeDly(10);
  }	
}

//=================================================================================================
// Note:
//=================================================================================================
#if(GBK_SUPPORT==0)
static void
PrintFrameContentInHex(CHAR name[], INT32U size, INT8U *content)
{
	INT8U i;

	DBG_PRINT("%s (hex): [", name);
	for (i=0; i<size; i++)
		DBG_PRINT(" %x", *(content+i));
	DBG_PRINT("]\r\n");
}
#endif
//=================================================================================================
//	ID3 tag test
//=================================================================================================
#define C_FRAME_COUNT	8
static void
Print_ID3_Frames(ID3_Tag_T *tag)
{
	CHAR title[C_FRAME_COUNT][15] = { "Title", "Artist", "Album", "Year", "Comment", "Track", "Genre", "Picture Data" };
	CHAR fid[C_FRAME_COUNT][5] = { C_ID3_FID_TITLE, C_ID3_FID_ARTIST, C_ID3_FID_ALBUM, C_ID3_FID_YEAR, C_ID3_FID_COMMENT, C_ID3_FID_TRACK, C_ID3_FID_GENRE, C_ID3_FID_APIC };
	INT8U *ptr, version;
	INT32S ret;
	INT32U i, size;
#if   GBK_SUPPORT
	for(i=0;i<C_FRAME_COUNT;i++)
	{
		ret = id3_get_frame_size(tag, fid[i], &size, &version);
		if(ret==C_ID3_RET_SUCCESS)
		{
			ptr = (INT8U*) gp_malloc(size+1);	// Size of ID3v2 strings does not include a trailing zero
			gp_memset((INT8S*)ptr,0,size+1);
			id3_get_frame_content(tag, fid[i], ptr, &version);
			//skip the none inforamtion
	        do
	        {
		       	if(*ptr==0)
					ptr++;
				else
					break;
	        }
			 while(size--);
			 //print message
			 if(size ==0)
                		DBG_PRINT("**WARN** '%s' not found\r\n", title[i]);
			 else
			 {
			 	DBG_PRINT("%s:",title[i]);
				sent_gbk_consol(ptr);
				DBG_PRINT("\r\n");
			 }
			
			gp_free(ptr);
		}
		else
			DBG_PRINT("**WARN** '%s' not found\r\n", title[i]);
	}
#else
	INT32S  mime_len, desc_len, apic_len;
	for (i=0; i<C_FRAME_COUNT; i++)
	{
		// Get the frame size of title
		ret = id3_get_frame_size(tag, fid[i], &size, &version);
		if (ret == C_ID3_RET_SUCCESS)
		{
			ptr = (INT8U*) gp_malloc(size+1);	// Size of ID3v2 strings does not include a trailing zero
			if (ptr == NULL)
				continue;

			// Get the frame content of title
			ret = id3_get_frame_content(tag, fid[i], ptr, &version);
			if (ret == C_ID3_RET_SUCCESS)
			{
				if (version == C_ID3_V1)
				{
					switch (i)
					{
					case 5:	// Track
					case 6:	// Genre
						DBG_PRINT("%s: [%u]\r\n", title[i], *ptr);
						break;

					default:
						// Print in both ASCII and hex for non-ASCII codes
						DBG_PRINT("%s: [%s]\r\n", title[i], ptr);
						PrintFrameContentInHex(title[i], size, ptr);
						break;
					}
				}
				else if (version == C_ID3_V2)
				{
					switch(i)
					{
					// <Header for 'Comment', ID: "COMM">
					// Text encoding          $xx
					// Language               $xx xx xx
					// Short content descrip. <text string according to encoding> $00 (00)
					// The actual text        <full text string according to encoding>
					case 4:	// comment
						*(ptr+size) = '\0';
						DBG_PRINT("Comment Encoding: [%u]\r\n", *ptr);
						DBG_PRINT("Comment Language: [%s]\r\n", ptr+1);
						DBG_PRINT("%s: [%s]\r\n", title[i], ptr+5);
						PrintFrameContentInHex(title[i], size-5, ptr);
						break;

					// <Header for 'Attached picture', ID: "APIC">
					// Text encoding      $xx
					// MIME type          <text string> $00
					// Picture type       $xx
					// Description        <text string according to encoding> $00 (00)
					// Picture data       <binary data>
					case 7:
						DBG_PRINT("Picture Text Encoding: [%u]\r\n", *ptr);
						DBG_PRINT("Picture MIME Type: [%s]\r\n", ptr+1);
						mime_len = gp_strlen((INT8S*)(ptr+1));
						DBG_PRINT("Picture Type: [%u]\r\n", *(ptr+mime_len+2));
						DBG_PRINT("Picture Description: [%s]\r\n", ptr+mime_len+3);
						desc_len = gp_strlen((INT8S*)(ptr+mime_len+3));
						apic_len = mime_len+desc_len+4;
						size -= apic_len;
						DBG_PRINT("Picture Size: [%u]\r\n", size);
						PrintFrameContentInHex(title[i], (size>32) ? 32 : apic_len, ptr+apic_len);
						break;

					// <Header for 'Text information frame', ID: "T000" - "TZZZ",
					// excluding "TXXX" described in 4.2.6.>
					// Text encoding          $xx
					// Information            <text string(s) according to encoding>
					default:
						*(ptr+size) = '\0';
						DBG_PRINT("%s: [%s]\r\n", title[i], ptr+1);	// 'ptr+1' is to skip the text encoding byte
						PrintFrameContentInHex(title[i], size, ptr);
						break;
					}
				}
			}

			gp_free((void*)ptr);
		}
		else
		{
			DBG_PRINT("**WARN** '%s' not found\r\n", title[i]);
		}
	}
#endif
}

void show_message()
{
	DBG_PRINT("entry ID3 TAG demo\r\n");
	#if GBK_SUPPORT
		DBG_PRINT("support GBK\r\n");
	#else
		DBG_PRINT("==================================\r\n");
		DBG_PRINT("this mode can not support GBK, suggest do something to ,support it\r\n");
		DBG_PRINT("1. set  GBK_SUPPORT to 1\r\n ");
		DBG_PRINT("2. add nls_cp950_cp932_cp936.c to Compiler \r\n");
		DBG_PRINT("3. add unitab_GBK.c   to Compiler \r\n");
		DBG_PRINT("4. add unitab_GBK_BIG5.c to Compiler \r\n");
		DBG_PRINT("5. add unitab_GBK_BIG5_sjis.c to Compiler\r\n");
		DBG_PRINT("6. sent  #define    SUPPORT_GBK_MUSIC_LIST    CUSTOM_ON  on gplib_cfg.h\r\n");
		DBG_PRINT("==================================\r\n");
	#endif
}


//=================================================================================================
// ID3 tag test
//
// How to run the test?
//	1. Set ID3_TAG_DEMO in main.c to 1.
//	2. Put some MP3 files on SD card and insert the SDC card into EV board.
//	3. Connect the EVB to PC with a UART cablew using the following settings
//			- Baud Rate: 115200
//			- Data Bits: 8
//			- Parity: None
//			- Stop Bits: 0
//			- Flow Control: None
//	4. Download the code and run
//	5. All ID3 tags about the MP3 files on the SD card will be printed to PC console
//
// How to read the ID3 tag in your own code?
//	1. See ID3_Tag_Test() and check the demo code
//	2. Check application.h to see all available ID3 tag APIs.
//	3. The following frames in v2 and all fields in v1 are tested.
//		- TIT2
//		- TPE1
//		- TALB
//		- TYER
//		- COMM
//		- TRCK
//		- TCON
//		- APIC
//=================================================================================================
static void
ID3_Tag_Test(void)
{
	CHAR path[255+1];
	INT16S fd;
	INT16U file_idx;
	INT32S ret;
	ID3_Tag_T tag;
	struct f_info   file_info;
	unsigned short mp3_unicode[6]={0x002a,0x002e,0x006d,0x0070,0x0033,0x0000};//*.mp3 unicode
	unsigned short store[]={0x0063,0x003a,0x005c,0x005c,0x0000};
	fd = (-1);
	file_idx = 0;
	
	show_message();

#if GBK_SUPPORT	
	init_gbk();
#endif
	ChangeCodePage(UNI_UNICODE);
	switch(USE_DISK)
	{
	case FS_SD:	

		chdir((CHAR *)store);
		ret = _findfirst((CHAR*)mp3_unicode, &file_info, D_ALL);
		while (ret == 0)
		{
			// Open an MP3 file
			gp_memcpy((INT8S *)path,(INT8S *)file_info.f_name,unicode_filename_strlen((char *)file_info.f_name)+2);
			//gp_strcpy((INT8S*)path, (INT8S*)file_info.f_name);
			DBG_PRINT("==================================================================\r\n");
			sent_unicode_consol( path);
			fd = open(path, O_RDONLY);
			if (fd < 0)
				continue;

			// Create an ID3_Tag_T struct
			ret = id3_create(&tag, fd);
			if (ret != C_ID3_RET_SUCCESS)
			{
				close(fd);
				continue;
			}

			DBG_PRINT("v2 tag size: [%u]\r\n", id3_get_v2_tag_size(&tag));
			Print_ID3_Frames(&tag);
			close(fd);

			ret = _findnext(&file_info);
		}
		break;

	default:
		break;
	}
#if GBK_SUPPORT
	exit_gbk();
#endif
}