#include "pwkey_process.h"

static INT8U pw_key_press=0;

static void pwkey_detect_isr(void);

INT8U pwkey_press_get(void);
void pwkey_process_init(void);

void pwkey_process_init(void)
{
#if POWER_CTRL_BY_GPY0200 == FALSE
    extab_edge_set(EXTA,RISING); /* rising trigger */
    extab_user_isr_set(EXTA,pwkey_detect_isr); /* register ext A interrupt callback function */
    extab_enable_set(EXTA,TRUE);
#else
	gpy0200_callback_set(GPY0200_USER_ISR_ONKEY1, pwkey_detect_isr);
    extab_enable_set(EXTA,TRUE);
#endif


}

INT8U pwkey_press_get(void)
{
    INT8U ret;

    ret = pw_key_press;
    pw_key_press = 0;
    return ret;
}


void pwkey_detect_isr(void)
{
    pw_key_press=1;
}





