#ifndef _DRV_L2_ITOUCH_H_
#define _DRV_L2_ITOUCH_H_
#include "driver_l2.h"

extern void itouch_init(void);
extern void itouch_reduce_write (INT8U salve_reg_addr, INT8U write_data); 
extern INT8U itouch_read_nBytes (INT8U slave_id, INT8U slave_reg_addr, INT8U *data_buf, INT32U data_size);
extern INT8U itouch_reduce_read_nBytes (INT8U slave_reg_addr, INT8U *data_buf, INT32U data_size);
extern INT8U itouch_version_get(void);
extern void itouch_sensitive_set(INT8U level);
extern INT8U itouch_sensitive_get(void);

#endif	/*_DRV_L2_ITOUCH_H_*/

