#include "stdio.h"

/*------------------Error Num-----------------*/
#define QR_OK					0
#define QR_ERR_FINDERPATTERN	-1
#define QR_ERR_ALIGNPATTERN		-2
#define QR_ERR_IMAGE_SIZE       -3
#define QR_ERR_IMAGE_BPP        -4
#define QR_ERR_MEMORY           -5
#define QR_ERR_VERSION          -6
#define QR_ECC_FAILUR			-7
#define QR_ERR_TOO_MUCH_LINE    -8


long QR_GetErrorNum(unsigned char *qr_working_memory);
long QR_GetVersion(unsigned char *qr_working_memory);	//Version of QRcode
long QR_GetECCLevel(unsigned char *qr_working_memory);
long QR_GetWorkingMemorySize(void);
long QR_Init(unsigned char *qr_working_memory,long width, long height,unsigned char* image, unsigned char image_type, unsigned char* resultstream);
unsigned char* QR_Decode(unsigned char *qr_working_memory);
const char* QR_Version(void);							//version of Decoder 

