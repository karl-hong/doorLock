/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user_protocol.h"
#include "common.h"
#include "user_data.h"
#include "flash_if.h"

#include "led.h"
#include "motor.h"
#include "sc7a20.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
lock_ctrl_t lock;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
 * 
 * @brief  设置RDP Level 1,禁止通过SWD烧录口读写Flash
 * 开启后，只能通过cubemxprammer或者其他烧录器来解除保护，解除保护会擦掉flash
 * 开启RDP Level 1保护，禁止通过SWD烧录口读写Flash。调试时关掉读写保护
 * 开启后，SWD口硬件线路未与stlink断开，且PC端在操作STlink时，会导致MCU程序卡死
 */


// #define OPEN_RDPLEVEL_1   // 打开读保护 Level 1 

FLASH_OBProgramInitTypeDef obInit;

void InterruptRemap(void)
{
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	memcpy((void*)RAM_ADDRESS_START, (void*)APPLICATION_ADDRESS, VECTOR_SIZE);
	__HAL_SYSCFG_REMAPMEMORY_SRAM();
  __enable_irq();//如果bootloader关闭中断，这里必须打开中断，否则会导致中断无法响应
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
void Set_RDP_Level1(void)
{
    HAL_FLASH_Unlock();         // 解锁主 Flash
    HAL_FLASH_OB_Unlock();      // 解锁 Option Bytes
    // 读取当前 Option Bytes 设置
    HAL_FLASHEx_OBGetConfig(&obInit);
    // 判断是否已设置为 Level 1，避免重复操作
    if (obInit.RDPLevel != OB_RDP_LEVEL_1)
    {
        obInit.OptionType = OPTIONBYTE_RDP;
        obInit.RDPLevel = OB_RDP_LEVEL_1;

        // 配置 RDP Level 1
        if (HAL_FLASHEx_OBProgram(&obInit) != HAL_OK)
        {
            printf("set RDP Level 1 fail!\r\n");// 可以继续执行其他初始化
            return;        
        }

        // 启动 Option Bytes 加载，MCU 会复位
        HAL_FLASH_OB_Launch();
    }

    HAL_FLASH_OB_Lock();   // 锁定 Option Bytes
    HAL_FLASH_Lock();      // 锁定 Flash
}

  int main(void)
{
  /* USER CODE BEGIN 1 */
	InterruptRemap();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  
  /* Configure the system clock */
  SystemClock_Config();
  user_database_init();
  #ifdef OPEN_RDPLEVEL_1
  Set_RDP_Level1();
  #endif

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  MX_DMA_Init();
  MX_I2C2_Init();
  MX_IWDG_Init();

  MX_TIM14_Init();

  /* USER CODE BEGIN 2 */
  
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
<<<<<<< HEAD
  
  

  /* USER CODE BEGIN 2 */
  user_database_init();
=======
>>>>>>> eabcd00728ee50d796692c56617fd15ffa27cd53
  printSetting();
  sync_boot_env();
  lock_state_init();
  lock_stop_detect();
  sc7a20_init(&sc7a20_misc_data);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    MX_IWDG_Refresh();
		led_task();
    motor_task();
    user_protocol_handle();
    user_reply_handle();
    sc7a20_task();
    autolock_task();
		user_huart_error_check();
    check_door_detect1_status();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
