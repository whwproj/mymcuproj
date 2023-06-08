#include "../Bsp/bsp_wkup.h"


void Enter_Standby_Mode()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能pwr外设时钟
    PWR_WakeUpPinCmd(ENABLE);
    
    PWR_ClearFlag(PWR_FLAG_WU);
    PWR_EnterSTANDBYMode();
}