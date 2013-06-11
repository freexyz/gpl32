//#include "boot.h"
#include "driver_l1.h"
#include "drv_l1_sfr.h"

//MSDC
extern INT32U   Sense_Code;

extern INT32U   Bulk_Stall;



#define R_Timeout_count 0xf888;

typedef struct {
	unsigned int ScsiCmdCode;
	void	(*function)(void);
} SCSI_CMD_MAP;



////////////////////////str_USB_Lun_Info->unStatus//////////////////////////////////
#define LUNSTS_NORMAL			0x0000
#define LUNSTS_NOMEDIA			0x0001
#define LUNSTS_WRITEPROTECT		0x0002
#define LUNSTS_MEDIACHANGE		0x0004

extern unsigned int GetLBA(void);
extern unsigned int GetTransLeng(void);

extern void GetICVersion(void);
extern void GetDriveStatus(void);
extern void SetVenderID(void);
extern void CommandFail(INT32U nSenseIndex);
extern INT32U GetCMDValueEx(INT32U index );
extern void GotoExRam(INT32U LBA);
extern void CallExRom (INT32U LBA);
extern void RegRead(INT32U REG,INT32U Count);
extern void RegWrite(INT32U REG ,INT32U Count);

extern  INT32U   CSW_Residue;
extern  INT32U   CSW_Status;





