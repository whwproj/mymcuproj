/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../common.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
BYTE send_buf[512];
HAL_SD_CardInfoTypeDef SDCardInfo;
DRESULT ret;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
TaskHandle_t myTestTaskHandle;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;


/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void printf_sdcard_info(HAL_SD_CardInfoTypeDef SDCardInfo) {
	uint64_t CardCap; //SD卡容量
	HAL_SD_CardCIDTypeDef SDCard_CID;
	HAL_SD_GetCardCID(&hsd,&SDCard_CID); //获取CID
	HAL_SD_GetCardInfo(&hsd,&SDCardInfo); //获取SD卡信息
	CardCap=(uint64_t)(SDCardInfo.LogBlockNbr)*(uint64_t)(SDCardInfo.LogBlockSize); //计算SD卡容量
	switch(SDCardInfo.CardType) {
		case CARD_SDSC:
		{
			if(SDCardInfo.CardVersion == CARD_V1_X)
				printf("Card Type:SDSC V1\r\n");
			else if(SDCardInfo.CardVersion == CARD_V2_X)
				printf("Card Type:SDSC V2\r\n");
		} break;
		case CARD_SDHC_SDXC:printf("Card Type:SDHC\r\n");
			break;
		default:break;
	}
	printf("Card ManufacturerID: %d \r\n",SDCard_CID.ManufacturerID); //制造商ID
	printf("CardVersion: %d \r\n",(uint32_t)(SDCardInfo.CardVersion)); //卡版本号
	printf("Class: %d \r\n",(uint32_t)(SDCardInfo.Class)); //SD卡类别
	printf("Card RCA(RelCardAdd):%d \r\n",SDCardInfo.RelCardAdd); //卡相对地址
	printf("Card BlockNbr: %d \r\n",SDCardInfo.BlockNbr); //块数量
	printf("Card BlockSize: %d \r\n",SDCardInfo.BlockSize); //块大小
	printf("LogBlockNbr: %d \r\n",(uint32_t)(SDCardInfo.LogBlockNbr)); //逻辑块数量
	printf("LogBlockSize: %d \r\n",(uint32_t)(SDCardInfo.LogBlockSize)); //逻辑块大小
	printf("Card Capacity: %d MB\r\n",(uint32_t)(CardCap>>20)); //卡容量
}

void myTestTaskFun(void const * argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
//  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 1024);
//  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  osThreadDef(myTestTask, myTestTaskFun, osPriorityNormal, 0, 128);
  myTestTaskHandle = osThreadCreate(osThread(myTestTask), NULL);
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	printf("init...\r\n");
	
	SD_Driver.disk_initialize(0);
	printf_sdcard_info(SDCardInfo);
	printf("\r\n\r\n********** 英文读写测试 **********\r\n");
	ret = SD_Driver.disk_write(0, (BYTE *)"L is too short to spend 阿time",20,2);
	printf("sd write result:%d\r\n", ret);
	ret = SD_Driver.disk_read(0, send_buf, 20, 2);
	printf("sd reak result:%d\r\n", ret);
	printf("sd read content:\r\n%s\r\n", send_buf);
  /* Infinite loop */
  for(;;)
  {
	printf("test \r\n");
	osDelay(1000);
	printf("test ok\r\n");
//	printf("\r\n\r\n********** 中文读写测试 **********\r\n");
//	ret = SD_Driver.disk_write(0,(BYTE *)"1.wifi模块", 22, 2);
//	printf("sd write result:%d\r\n", ret);
//	ret = SD_Driver.disk_read(0, send_buf, 23, 2);
//	printf("sd reak result:%d\r\n", ret);
//	printf("sd read content:\r\n%s\r\n", send_buf);
	
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void myTestTaskFun(void const * argument) {
	for(;;) {
		osDelay(2000);
		printf("...\r\n");
	}
}
/* USER CODE END Application */

