#include "../Bsp/bsp_wkup.h"

uint8_t Wkup0_Check_Up_HAL( void ) {
	uint16_t time = 0;
	while ( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET ) {
		HAL_Delay( 50 );//3s = 3000ms = 50 * 60
		if ( time++ > 60 ) {//3s
			break;
		}
	}
	if ( time > 60 ) {
		return 1;
	} else {
		return 0;
	}
}

uint8_t Wkup0_Check_Up_OS( void ) {
	uint16_t time = 0;
	while ( HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET ) {
		osDelay( 50 );//3s = 3000ms = 50 * 60
		if ( time++ > 1 ) {//3s
			printf("大于3s\r\n");
			break;
		}
	}
	if ( time > 1 ) {
		enter_standbyMode();
		return 1;
	} else {
		printf("...%d\r\n", time);
		return 0;
	}
}

void enter_standbyMode( void ) {
	//HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
	printf("进入待机模式\r\n");
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
	HAL_PWR_EnterSTANDBYMode();
}

void judge_if_Rest( void ) {
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB)==SET)
	{
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
		printf("standby wakeUp");
	}
	else
		printf("normal reset");
}



