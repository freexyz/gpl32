#include "gplib.h"

#define USE_DISK	FS_SD
//#define USE_DISK	FS_NAND1	
//#define USE_DISK	FS_USBH

static void ID3_Tag_Test(void);

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
static void
PrintFrameContentInHex(CHAR name[], INT32U size, INT8U *content)
{
	INT8U i;

	DBG_PRINT("%s (hex): [", name);
	for (i=0; i<size; i++)
		DBG_PRINT(" %x", *(content+i));
	DBG_PRINT("]\r\n");
}

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
	INT32S ret, mime_len, desc_len, apic_len;
	INT32U i, size;

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

	fd = (-1);
	file_idx = 0;
	switch(USE_DISK)
	{
	case FS_SD:	

		chdir("C:\\");
		ret = _findfirst("*.mp3", &file_info, D_ALL);
		while (ret == 0)
		{
			// Open an MP3 file
			gp_strcpy((INT8S*)path, (INT8S*)file_info.f_name);
			DBG_PRINT("==================================================================\r\n");
			DBG_PRINT("File %u: [%s]\r\n", file_idx++, path);
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
}