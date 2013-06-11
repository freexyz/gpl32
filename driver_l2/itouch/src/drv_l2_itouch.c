#include	"drv_l2_itouch.h"

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#if (defined _DRV_L2_ITOUCH) && (_DRV_L2_ITOUCH == 1)             //
//================================================================//

#ifdef _PROJ_TYPE
#if  _PROJ_TYPE == _PROJ_TURNKEY
fp_msg_qsend it_msg_QSend;
void   *it_msgQId;
INT32U it_msgId;
#endif
#endif

// Global Variables for itouch Library  
static INT32U iTouchSCL;
static INT32U iTouchSDA;

void itouch_delay (INT16U i);
void itouch_start (void);
void itouch_stop (void);
void itouch_w_phase (INT8U value);
INT8U itouch_r_phase (void);
void itouch_i2c_init (INT32U nSCL, INT32U nSDA);
void itouch_reduce_write (INT8U salve_reg_addr, INT8U write_data); 
INT8U itouch_read_nBytes (INT8U slave_id, INT8U slave_reg_addr, INT8U *data_buf, INT32U data_size);
INT8U itouch_reduce_read_nBytes (INT8U slave_reg_addr, INT8U *data_buf, INT32U data_size);
INT8U itouch_read_i2c_ACK(void);
void itouch_write_i2c_ACK(void);
void itouch_ad_int_init(void);
void itouch_isr(void);
void turnkey_itouch_resource_register(void* msg_qsend,void* msgq_id,INT32U msg_id);
                                           
#define TIME_DELAY 2

void itouch_gpio_output_high(INT32U i)
{
	gpio_init_io(i, GPIO_INPUT);
	gpio_set_port_attribute(i, ATTRIBUTE_HIGH);
}

void itouch_gpio_output_low(INT32U i)
{
	gpio_init_io(i, GPIO_OUTPUT);
	gpio_write_io(i, DATA_LOW);
}

BOOLEAN itouch_gpio_read(INT32U i)
{
	gpio_init_io(i, GPIO_INPUT);
	gpio_write_io(i, DATA_HIGH);
	return gpio_read_io(i);
}


void itouch_delay (INT16U i) 
{
	INT16U j;
	for (j=0;j<(i<<8);j++)
    {   
		i=i;
    }
}

void itouch_start (void)
{
	itouch_gpio_output_high(iTouchSDA);
    itouch_gpio_output_high(iTouchSCL);
	itouch_gpio_output_low(iTouchSDA);
    sw_timer_us_delay(TIME_DELAY);
    itouch_gpio_output_low(iTouchSCL);
    sw_timer_us_delay(TIME_DELAY);
}

void itouch_stop (void)
{
	itouch_gpio_output_low(iTouchSDA);			
	itouch_gpio_output_low(iTouchSCL);
	sw_timer_us_delay(30);
	itouch_gpio_output_high(iTouchSCL);
	sw_timer_us_delay(30);
	itouch_gpio_output_high(iTouchSDA);				
    sw_timer_us_delay(30);
    itouch_gpio_output_high(iTouchSDA);				
	itouch_gpio_output_high(iTouchSCL);
    sw_timer_us_delay(30);
}

INT8U itouch_read_i2c_ACK(void)
{
	INT8U Loop,ret=0;
  	INT32U timer1, timer2;
	itouch_gpio_output_high(iTouchSCL);					
	timer1 = tiny_counter_get();
	Loop   = 100;
	while(Loop)
	{
      	if (!itouch_gpio_read(iTouchSDA))
      	{
			ret = 1;
			break;
      	}
		timer2 = tiny_counter_get();
	    if (timer1 != timer2) 
		{
			timer1 = timer2;
        	Loop--;
    	}        
    }  
	itouch_gpio_output_low(iTouchSCL);
	sw_timer_us_delay(TIME_DELAY);
    return ret;
    
}

void itouch_write_i2c_ACK(void)
{   	
    itouch_gpio_output_low(iTouchSDA);						
    itouch_gpio_output_high(iTouchSCL);						
	sw_timer_us_delay(TIME_DELAY);
	itouch_gpio_output_low(iTouchSCL);						   
    sw_timer_us_delay(TIME_DELAY);
}


void itouch_i2c_init (INT32U i2c_SLC_io_idx, INT32U i2c_SDA_io_idx) 
{
	iTouchSCL = i2c_SLC_io_idx;
	iTouchSDA = i2c_SDA_io_idx;
}

void itouch_reduce_write (INT8U salve_reg_addr, INT8U write_data) 
{
    salve_reg_addr &= 0xFE; 
	itouch_gpio_output_high(iTouchSCL);
	itouch_gpio_output_high(iTouchSDA);
	itouch_start();								
	itouch_w_phase (salve_reg_addr);				
	itouch_w_phase (write_data);					
	itouch_stop();								
}

void itouch_write_nBytes (INT8U salve_id, INT8U salve_reg_addr, INT8U *data_buf, INT32U data_size) 
{
    INT32U i;
	itouch_gpio_output_high(iTouchSCL);
	itouch_gpio_output_high(iTouchSDA);
	itouch_start();									
	itouch_w_phase (salve_id);								
	itouch_w_phase (salve_reg_addr);							
    for (i=0; i<data_size; i++)
    {
	    itouch_w_phase (data_buf[i]);							
    }
	itouch_stop();		
}

INT8U itouch_r_phase_2 (void)                                              
{                                                                       
	INT16U i;                                                             
	INT8U data;                                                          
	data = 0x00;                                                          
	for (i=0;i<8;i++)                                                     
	{   
		itouch_gpio_output_high(iTouchSCL);	
		data <<= 1; 		
		data |=(itouch_gpio_read(iTouchSDA));             
		sw_timer_us_delay(TIME_DELAY);	
        itouch_gpio_output_low(iTouchSCL);
        sw_timer_us_delay(TIME_DELAY);                                                  
	}                                                                     
	return data;		                                                      
}  

INT8U itouch_read_nBytes (INT8U slave_id, INT8U slave_reg_addr, INT8U *data_buf, INT32U data_size)
{
	INT8U  data=0;
    INT32U i;
	itouch_gpio_output_high(iTouchSCL);
	itouch_gpio_output_high(iTouchSDA);
	itouch_start();	
	itouch_w_phase (slave_id);
	itouch_w_phase (slave_reg_addr);
	itouch_start();	 
	slave_id |= 0x1;   
	itouch_w_phase (0x35);	
    for (i=0; i<data_size; i++)
    {
		data_buf[i] = itouch_r_phase_2();	   
		if(i == (data_size -1))
		{
			itouch_gpio_output_high(iTouchSDA);						
			itouch_gpio_output_high(iTouchSCL);						
			sw_timer_us_delay(TIME_DELAY);
			itouch_gpio_output_low(iTouchSCL);						   
			sw_timer_us_delay(TIME_DELAY);
		}
		else
			itouch_write_i2c_ACK();  
		
    }
	itouch_stop();	
	return data;
}

INT8U itouch_reduce_read_nBytes (INT8U slave_reg_addr, INT8U *data_buf, INT32U data_size)
{
    INT32U i;
	slave_reg_addr |= 0x1;  
	itouch_gpio_output_high(iTouchSCL);
	itouch_gpio_output_high(iTouchSDA);
	itouch_start();									
	itouch_w_phase(slave_reg_addr);							

    for (i=0; i<data_size; i++)
    {
	    data_buf[i] = itouch_r_phase();							
    }
	itouch_stop();
	return 1;
}
 
void itouch_w_phase (INT8U value)                                        
{                                                                       
	INT16U i;                                                             
	for (i=0;i<8;i++)                                                     
	{    
		if (value & 0x80)          
        {      
			itouch_gpio_output_high(iTouchSDA);				                  
        }
        else
        {
			itouch_gpio_output_low(iTouchSDA);				                 
        }        
		value <<= 1; 
		sw_timer_us_delay(TIME_DELAY);  
		itouch_gpio_output_high(iTouchSCL);	
		sw_timer_us_delay(TIME_DELAY);  
		itouch_gpio_output_low(iTouchSCL);
		sw_timer_us_delay(TIME_DELAY);
	}     
	itouch_read_i2c_ACK();
}                                                                       
                                                                        
INT8U itouch_r_phase (void)                                              
{                                                                       
	INT16U i;                                                             
	INT8U data;                                                          
	data = 0x00;                                                          
	for (i=0;i<8;i++)                                                     
	{   
		itouch_gpio_output_high(iTouchSCL);	
		data <<= 1; 		
		data |=(itouch_gpio_read(iTouchSDA));             
		sw_timer_us_delay(TIME_DELAY);	
        itouch_gpio_output_low(iTouchSCL);
        sw_timer_us_delay(TIME_DELAY);                                                  
	}                                                                     
    itouch_write_i2c_ACK();  
	return data;		                                                      
}   
  
void itouch_ad_int_init(void)
{
	adc_init();
	adc_user_line_in_en(ADC_LINE_0, TRUE);
    adc_tp_en(TRUE);
    gpio_init_io(0x56, GPIO_INPUT);
    gpio_write_io(0x56, DATA_HIGH);
    adc_tp_int_en(TRUE);
	tp_callback_set(itouch_isr);
}

void itouch_reset(void)
{
	gpio_init_io(ITOUCH_RESET, GPIO_OUTPUT);			 	
	gpio_write_io(ITOUCH_RESET, DATA_LOW);				
    sw_timer_us_delay(10);
	gpio_write_io(ITOUCH_RESET, DATA_HIGH);				
}

void itouch_init(void)
{
	itouch_ad_int_init();
    itouch_i2c_init(ITOUCH_SCL, ITOUCH_SDA);
	//itouch_reset();
}

INT8U itouch_version_get(void)
{
	INT8U version;
	itouch_read_nBytes(GPY0501C_ADDR_R,GPY0501C_VERSION_R,&version,1);
	//DBG_PRINT("ITOUCH SOFTWARE VERSION:%d\r\n",version);
	
	return version;
}

void itouch_sensitive_set(INT8U level)
{
	//DBG_PRINT("Set itouch sensitive to :%d\r\n",level);
	itouch_write_nBytes(GPY0501C_ADDR_R,GPY0501C_VERSION_R,&level,1);	
}

INT8U itouch_sensitive_get(void)
{
	INT8U sensitive;
	itouch_read_nBytes(GPY0501C_ADDR_R,GPY0501C_VERSION_R,&sensitive,1);
	//DBG_PRINT("Itouch sensitive:%d\r\n",sensitive);
	return sensitive;
}


INT32U itouch_sensor_result_get(void)
{
    INT32U ret=0;
    itouch_reduce_read_nBytes(GPY0501C_ADDR_R,(INT8U *)&ret,3);
    return ret;
}

void itouch_isr(void)
{
    INT32U get_data=0;
	#ifdef _PROJ_TYPE
	#if  _PROJ_TYPE == _PROJ_TURNKEY
	adc_tp_int_en(FALSE);
    it_msg_QSend(it_msgQId,it_msgId,&get_data,sizeof(INT32U),0);   
	adc_tp_int_en(TRUE);
	#endif
	#endif
}

#ifdef _PROJ_TYPE
void turnkey_itouch_resource_register(void* msg_qsend,void* msgq_id,INT32U msg_id)
{
	it_msg_QSend = (fp_msg_qsend) msg_qsend;
	it_msgQId = msgq_id;
	it_msgId = msg_id;
}
#endif

//=== This is for code configuration DON'T REMOVE or MODIFY it ===//
#endif //(defined _DRV_L2_ITOUCH) && (_DRV_L2_ITOUCH == 1)        //
//================================================================//


