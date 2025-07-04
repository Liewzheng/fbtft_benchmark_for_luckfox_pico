/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   Luckfox team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V2.0
* | Date        :   2019-07-08
* | Info        :   Basic version
*
******************************************************************************/
#include "DEV_Config.h"

#if USE_DEV_LIB

pthread_t t1;
UWORD pwm_dule=100;
struct sched_param param;
pthread_attr_t attr;
void *BL_PWM(void *arg){
	(void)arg; // 忽略未使用的参数警告
	
	UWORD i=0;
	for(i=0;;i++){
		if(i>64)i=0;
		if(i<(pwm_dule/16))LCD_BL_1;
		else LCD_BL_0;
	}	
	
}
#endif
void DEV_SetBacklight(UWORD Value)
{
#ifdef USE_DEV_LIB
	pwm_dule=Value;
#endif
}

/*****************************************
                GPIO
*****************************************/
void DEV_Digital_Write(UWORD Pin, UBYTE Value)
{
#ifdef USE_DEV_LIB
    SYSFS_GPIO_Write(Pin, Value);
#endif
}

UBYTE DEV_Digital_Read(UWORD Pin)
{
    UBYTE Read_value = 0;
#ifdef USE_DEV_LIB
    Read_value = SYSFS_GPIO_Read(Pin);
#endif
    return Read_value;
}

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode)
{
#ifdef USE_DEV_LIB
    SYSFS_GPIO_Export(Pin);
    if(Mode == 0 || Mode == SYSFS_GPIO_IN){
        SYSFS_GPIO_Direction(Pin, SYSFS_GPIO_IN);
        printf("IN Pin = %d\r\n",Pin);
    }else{
        SYSFS_GPIO_Direction(Pin, SYSFS_GPIO_OUT);
        printf("OUT Pin = %d\r\n",Pin);
    }
#endif   
}

/**
 * delay x ms
**/
void DEV_Delay_ms(UDOUBLE xms)
{
#ifdef  USE_DEV_LIB
    UDOUBLE i;
    for(i=0; i < xms; i++){
        usleep(1000);
    }
#endif
}

static void DEV_GPIO_Init(void)
{
    DEV_GPIO_Mode(LCD_CS, 1);
    DEV_GPIO_Mode(LCD_RST, 1);
    DEV_GPIO_Mode(LCD_DC, 1);
    DEV_GPIO_Mode(LCD_BL, 1);
    
    DEV_GPIO_Mode(KEY_UP_PIN, 0);
    DEV_GPIO_Mode(KEY_DOWN_PIN, 0);
    DEV_GPIO_Mode(KEY_LEFT_PIN, 0);
    DEV_GPIO_Mode(KEY_RIGHT_PIN, 0);
    DEV_GPIO_Mode(KEY_PRESS_PIN, 0);
    DEV_GPIO_Mode(KEY1_PIN, 0);
    DEV_GPIO_Mode(KEY2_PIN, 0);
    DEV_GPIO_Mode(KEY3_PIN, 0);
    DEV_GPIO_Mode(KEYA_PIN, 0);
    DEV_GPIO_Mode(KEYB_PIN, 0);
    DEV_GPIO_Mode(KEYX_PIN, 0);
    DEV_GPIO_Mode(KEYY_PIN, 0);
    LCD_CS_1;
	LCD_BL_1;
}

/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
UBYTE DEV_ModuleInit(void)
{
 #ifdef USE_DEV_LIB

    DEV_GPIO_Init();
    DEV_HARDWARE_SPI_begin("/dev/spidev0.0");
	
	#ifdef USE_DEV_LIB_PWM
		pthread_attr_init(&attr);	
		param.sched_priority = 98;
		pthread_attr_setschedpolicy(&attr,SCHED_RR);
		pthread_attr_setschedparam(&attr,&param);
		pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);
		pthread_create(&t1,&attr,BL_PWM,NULL);
		// pthread_join(&t1,NULL);
    #else

	#endif

#endif
    return 0;
}

void DEV_SPI_WriteByte(uint8_t Value)
{
#ifdef  USE_DEV_LIB
    DEV_HARDWARE_SPI_TransferByte(Value);
#endif
}

void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len)
{
#ifdef  USE_DEV_LIB
    DEV_HARDWARE_SPI_Transfer(pData, Len);
#endif
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_ModuleExit(void)
{
#ifdef USE_DEV_LIB
    DEV_HARDWARE_SPI_end();
	#ifdef USE_DEV_LIB_PWM
		pthread_cancel(t1);
	#endif
	LCD_BL_1;
#endif
}