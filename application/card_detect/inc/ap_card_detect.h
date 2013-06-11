#include "application.h"

typedef struct {
   INT16U   debouce;        
   INT16U   noise_debouce;  
   INT16U   noise_flag;
} PIN_BOUCE, *pPIN_BOUCE;

void card_detect_set_cf_callback(void (*cf_callback)(void));
void card_detect_set_sdms_callback(void (*sdms_callback)(void));
void card_detect_set_usb_h_callback(void (*usb_h_callback)(void));
void card_detect_set_usb_d_callback(void (*usb_d_callback)(void));
INT8U card_detect_get_plug_status(void);
INT8U card_detect_debouce(INT8U pin,INT32U ioport,INT16U  pin_id);
void card_detect_usb_h_d_isr(void);
void card_detect_cfc_reset(void);
void card_detect_init(INT16U devicetpye);
void card_detect_polling_usbh(void);
void card_detect_polling_start(void);
INT16U card_detect_polling(void);
void card_detection_isr(void);
void sdms_detection(void);
void cfc_detection(void);
INT16U card_detection(INT16U type);