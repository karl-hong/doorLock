/**
  ******************************************************************************
  * @file    IAP_Main/Inc/flash_if.h 
  * @author  MCD Application Team
  * @version 1.0.0
  * @date    8-April-2015
  * @brief   This file provides all the headers of the flash_if functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_IF_H
#define __FLASH_IF_H
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
#define VECTOR_SIZE (47*4)//STM32F0不同型号可能该值不同，查看startup_stm32f051.s，每一个DCD都代表一个中断向量（中断向量即某个中断服务程序的入口地址），一共有45个，每个地址大小是占用4字节，故中断向量表的总大小为45*4
#define UPGRADE_FLAG_DEFAULT_VALUE	0xffffffff
/* Exported constants --------------------------------------------------------*/

#if FLASH_IS_64K
/* Base address of the Flash sectors */
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 1 Kbytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x08000400) /* Base @ of Page 1, 1 Kbytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x08000800) /* Base @ of Page 2, 1 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x08000C00) /* Base @ of Page 3, 1 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x08001000) /* Base @ of Page 4, 1 Kbytes */
#define ADDR_FLASH_PAGE_5     ((uint32_t)0x08001400) /* Base @ of Page 5, 1 Kbytes */
#define ADDR_FLASH_PAGE_6     ((uint32_t)0x08001800) /* Base @ of Page 6, 1 Kbytes */
#define ADDR_FLASH_PAGE_7     ((uint32_t)0x08001C00) /* Base @ of Page 7, 1 Kbytes */
#define ADDR_FLASH_PAGE_8     ((uint32_t)0x08002000) /* Base @ of Page 8, 1 Kbytes */
#define ADDR_FLASH_PAGE_9     ((uint32_t)0x08002400) /* Base @ of Page 9, 1 Kbytes */
#define ADDR_FLASH_PAGE_10    ((uint32_t)0x08002800) /* Base @ of Page 10, 1 Kbytes */
#define ADDR_FLASH_PAGE_11    ((uint32_t)0x08002C00) /* Base @ of Page 11, 1 Kbytes */
#define ADDR_FLASH_PAGE_12    ((uint32_t)0x08003000) /* Base @ of Page 12, 1 Kbytes */
#define ADDR_FLASH_PAGE_13    ((uint32_t)0x08003400) /* Base @ of Page 13, 1 Kbytes */
#define ADDR_FLASH_PAGE_14    ((uint32_t)0x08003800) /* Base @ of Page 14, 1 Kbytes */
#define ADDR_FLASH_PAGE_15    ((uint32_t)0x08003C00) /* Base @ of Page 15, 1 Kbytes */
#define ADDR_FLASH_PAGE_16    ((uint32_t)0x08004000) /* Base @ of Page 16, 1 Kbytes */
#define ADDR_FLASH_PAGE_17    ((uint32_t)0x08004400) /* Base @ of Page 17, 1 Kbytes */
#define ADDR_FLASH_PAGE_18    ((uint32_t)0x08004800) /* Base @ of Page 18, 1 Kbytes */
#define ADDR_FLASH_PAGE_19    ((uint32_t)0x08004C00) /* Base @ of Page 19, 1 Kbytes */
#define ADDR_FLASH_PAGE_20    ((uint32_t)0x08005000) /* Base @ of Page 20, 1 Kbytes */
#define ADDR_FLASH_PAGE_21    ((uint32_t)0x08005400) /* Base @ of Page 21, 1 Kbytes */
#define ADDR_FLASH_PAGE_22    ((uint32_t)0x08005800) /* Base @ of Page 22, 1 Kbytes */
#define ADDR_FLASH_PAGE_23    ((uint32_t)0x08005C00) /* Base @ of Page 23, 1 Kbytes */
#define ADDR_FLASH_PAGE_24    ((uint32_t)0x08006000) /* Base @ of Page 24, 1 Kbytes */
#define ADDR_FLASH_PAGE_25    ((uint32_t)0x08006400) /* Base @ of Page 25, 1 Kbytes */
#define ADDR_FLASH_PAGE_26    ((uint32_t)0x08006800) /* Base @ of Page 26, 1 Kbytes */
#define ADDR_FLASH_PAGE_27    ((uint32_t)0x08006C00) /* Base @ of Page 27, 1 Kbytes */
#define ADDR_FLASH_PAGE_28    ((uint32_t)0x08007000) /* Base @ of Page 28, 1 Kbytes */
#define ADDR_FLASH_PAGE_29    ((uint32_t)0x08007400) /* Base @ of Page 29, 1 Kbytes */
#define ADDR_FLASH_PAGE_30    ((uint32_t)0x08007800) /* Base @ of Page 30, 1 Kbytes */
#define ADDR_FLASH_PAGE_31    ((uint32_t)0x08007C00) /* Base @ of Page 31, 1 Kbytes */
#define ADDR_FLASH_PAGE_32    ((uint32_t)0x08008000) /* Base @ of Page 32, 1 Kbytes */
#define ADDR_FLASH_PAGE_33    ((uint32_t)0x08008400) /* Base @ of Page 33, 1 Kbytes */
#define ADDR_FLASH_PAGE_34    ((uint32_t)0x08008800) /* Base @ of Page 34, 1 Kbytes */
#define ADDR_FLASH_PAGE_35    ((uint32_t)0x08008C00) /* Base @ of Page 35, 1 Kbytes */
#define ADDR_FLASH_PAGE_36    ((uint32_t)0x08009000) /* Base @ of Page 36, 1 Kbytes */
#define ADDR_FLASH_PAGE_37    ((uint32_t)0x08009400) /* Base @ of Page 37, 1 Kbytes */
#define ADDR_FLASH_PAGE_38    ((uint32_t)0x08009800) /* Base @ of Page 38, 1 Kbytes */
#define ADDR_FLASH_PAGE_39    ((uint32_t)0x08009C00) /* Base @ of Page 39, 1 Kbytes */
#define ADDR_FLASH_PAGE_40    ((uint32_t)0x0800A000) /* Base @ of Page 40, 1 Kbytes */
#define ADDR_FLASH_PAGE_41    ((uint32_t)0x0800A400) /* Base @ of Page 41, 1 Kbytes */
#define ADDR_FLASH_PAGE_42    ((uint32_t)0x0800A800) /* Base @ of Page 42, 1 Kbytes */
#define ADDR_FLASH_PAGE_43    ((uint32_t)0x0800AC00) /* Base @ of Page 43, 1 Kbytes */
#define ADDR_FLASH_PAGE_44    ((uint32_t)0x0800B000) /* Base @ of Page 44, 1 Kbytes */
#define ADDR_FLASH_PAGE_45    ((uint32_t)0x0800B400) /* Base @ of Page 45, 1 Kbytes */
#define ADDR_FLASH_PAGE_46    ((uint32_t)0x0800B800) /* Base @ of Page 46, 1 Kbytes */
#define ADDR_FLASH_PAGE_47    ((uint32_t)0x0800BC00) /* Base @ of Page 47, 1 Kbytes */
#define ADDR_FLASH_PAGE_48    ((uint32_t)0x0800C000) /* Base @ of Page 48, 1 Kbytes */
#define ADDR_FLASH_PAGE_49    ((uint32_t)0x0800C400) /* Base @ of Page 49, 1 Kbytes */
#define ADDR_FLASH_PAGE_50    ((uint32_t)0x0800C800) /* Base @ of Page 50, 1 Kbytes */
#define ADDR_FLASH_PAGE_51    ((uint32_t)0x0800CC00) /* Base @ of Page 51, 1 Kbytes */
#define ADDR_FLASH_PAGE_52    ((uint32_t)0x0800D000) /* Base @ of Page 52, 1 Kbytes */
#define ADDR_FLASH_PAGE_53    ((uint32_t)0x0800D400) /* Base @ of Page 53, 1 Kbytes */
#define ADDR_FLASH_PAGE_54    ((uint32_t)0x0800D800) /* Base @ of Page 54, 1 Kbytes */
#define ADDR_FLASH_PAGE_55    ((uint32_t)0x0800DC00) /* Base @ of Page 55, 1 Kbytes */
#define ADDR_FLASH_PAGE_56    ((uint32_t)0x0800E000) /* Base @ of Page 56, 1 Kbytes */
#define ADDR_FLASH_PAGE_57    ((uint32_t)0x0800E400) /* Base @ of Page 57, 1 Kbytes */
#define ADDR_FLASH_PAGE_58    ((uint32_t)0x0800E800) /* Base @ of Page 58, 1 Kbytes */
#define ADDR_FLASH_PAGE_59    ((uint32_t)0x0800EC00) /* Base @ of Page 59, 1 Kbytes */
#define ADDR_FLASH_PAGE_60    ((uint32_t)0x0800F000) /* Base @ of Page 60, 1 Kbytes */
#define ADDR_FLASH_PAGE_61    ((uint32_t)0x0800F400) /* Base @ of Page 61, 1 Kbytes */
#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0800F800) /* Base @ of Page 62, 1 Kbytes */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0800FC00) /* Base @ of Page 63, 1 Kbytes */
#elif FLASH_IS_1024K
/* Base address of the Flash sectors Bank 1 */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */
#elif FLASH_IS_32K
/* Base address of the Flash sectors */
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 1 Kbytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x08000400) /* Base @ of Page 1, 1 Kbytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x08000800) /* Base @ of Page 2, 1 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x08000C00) /* Base @ of Page 3, 1 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x08001000) /* Base @ of Page 4, 1 Kbytes */
#define ADDR_FLASH_PAGE_5     ((uint32_t)0x08001400) /* Base @ of Page 5, 1 Kbytes */
#define ADDR_FLASH_PAGE_6     ((uint32_t)0x08001800) /* Base @ of Page 6, 1 Kbytes */
#define ADDR_FLASH_PAGE_7     ((uint32_t)0x08001C00) /* Base @ of Page 7, 1 Kbytes */
#define ADDR_FLASH_PAGE_8     ((uint32_t)0x08002000) /* Base @ of Page 8, 1 Kbytes */
#define ADDR_FLASH_PAGE_9     ((uint32_t)0x08002400) /* Base @ of Page 9, 1 Kbytes */
#define ADDR_FLASH_PAGE_10    ((uint32_t)0x08002800) /* Base @ of Page 10, 1 Kbytes */
#define ADDR_FLASH_PAGE_11    ((uint32_t)0x08002C00) /* Base @ of Page 11, 1 Kbytes */
#define ADDR_FLASH_PAGE_12    ((uint32_t)0x08003000) /* Base @ of Page 12, 1 Kbytes */
#define ADDR_FLASH_PAGE_13    ((uint32_t)0x08003400) /* Base @ of Page 13, 1 Kbytes */
#define ADDR_FLASH_PAGE_14    ((uint32_t)0x08003800) /* Base @ of Page 14, 1 Kbytes */
#define ADDR_FLASH_PAGE_15    ((uint32_t)0x08003C00) /* Base @ of Page 15, 1 Kbytes */
#define ADDR_FLASH_PAGE_16    ((uint32_t)0x08004000) /* Base @ of Page 16, 1 Kbytes */
#define ADDR_FLASH_PAGE_17    ((uint32_t)0x08004400) /* Base @ of Page 17, 1 Kbytes */
#define ADDR_FLASH_PAGE_18    ((uint32_t)0x08004800) /* Base @ of Page 18, 1 Kbytes */
#define ADDR_FLASH_PAGE_19    ((uint32_t)0x08004C00) /* Base @ of Page 19, 1 Kbytes */
#define ADDR_FLASH_PAGE_20    ((uint32_t)0x08005000) /* Base @ of Page 20, 1 Kbytes */
#define ADDR_FLASH_PAGE_21    ((uint32_t)0x08005400) /* Base @ of Page 21, 1 Kbytes */
#define ADDR_FLASH_PAGE_22    ((uint32_t)0x08005800) /* Base @ of Page 22, 1 Kbytes */
#define ADDR_FLASH_PAGE_23    ((uint32_t)0x08005C00) /* Base @ of Page 23, 1 Kbytes */
#define ADDR_FLASH_PAGE_24    ((uint32_t)0x08006000) /* Base @ of Page 24, 1 Kbytes */
#define ADDR_FLASH_PAGE_25    ((uint32_t)0x08006400) /* Base @ of Page 25, 1 Kbytes */
#define ADDR_FLASH_PAGE_26    ((uint32_t)0x08006800) /* Base @ of Page 26, 1 Kbytes */
#define ADDR_FLASH_PAGE_27    ((uint32_t)0x08006C00) /* Base @ of Page 27, 1 Kbytes */
#define ADDR_FLASH_PAGE_28    ((uint32_t)0x08007000) /* Base @ of Page 28, 1 Kbytes */
#define ADDR_FLASH_PAGE_29    ((uint32_t)0x08007400) /* Base @ of Page 29, 1 Kbytes */
#define ADDR_FLASH_PAGE_30    ((uint32_t)0x08007800) /* Base @ of Page 30, 1 Kbytes */
#define ADDR_FLASH_PAGE_31    ((uint32_t)0x08007C00) /* Base @ of Page 31, 1 Kbytes */

#endif

#define RAM_ADDRESS_START 0x20000000
#define APP_UPGREQ_IS_VALID			0x214D4478		/* APP??????:!MDx */

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#if FLASH_IS_32K
/* APP 头信息 */
#define APPHEADER_ADDRESS				ADDR_FLASH_PAGE_11
#define APPHEADER_SIZE					(APPLICATION_ADDRESS - APPHEADER_ADDRESS)				/* Size of page : 1 Kbytes */
/* Define the address from where user application will be loaded. */
#define APPLICATION_ADDRESS     ADDR_FLASH_PAGE_12			/* Start user code address: ADDR_FLASH_PAGE_12 */
/* 计算APP区每页/扇区的大小 */
#define APP_FLASH_STEP          (ADDR_FLASH_PAGE_12 - ADDR_FLASH_PAGE_11)				/* Size of page : 1 Kbytes */
/* End of the Flash address */
#define USER_FLASH_END_ADDRESS     (ADDR_FLASH_PAGE_31 + APP_FLASH_STEP - 1)

#elif FLASH_IS_64K
/*
       Flash分区表如下
 _________________________
/													\ADDR_FLASH_PAGE_63
|				    App 					|
|												 	|
\_________________________/ADDR_FLASH_PAGE_16
/													\
|				Upgrade Flag	    |
|												 	|
\_________________________/ADDR_FLASH_PAGE_15
/													\ADDR_FLASH_PAGE_14
|					 Boot				  	|
|												 	|
\_________________________/ADDR_FLASH_PAGE_0

*/
// #define APP_CONFIG_ADDRESS				ADDR_FLASH_PAGE_11//配置区域地址
#define APP_UPGRADE_FLAG_ADDRESS	ADDR_FLASH_PAGE_15//升级标志区域地址
/* Define the address from where user application will be loaded. */
#define APPLICATION_ADDRESS     	ADDR_FLASH_PAGE_16			/* Start user code address: ADDR_FLASH_PAGE_13 */
/* 计算APP区每页/扇区的大小 */
#define APP_FLASH_STEP          	(ADDR_FLASH_PAGE_1 - ADDR_FLASH_PAGE_0)				/* Size of page : 1 Kbytes */
/* End of the Flash address */
#define USER_FLASH_END_ADDRESS    (ADDR_FLASH_PAGE_63 + APP_FLASH_STEP - 1)
#elif FLASH_IS_1024K
/* APP 头信息 */
#define APPHEADER_ADDRESS       ADDR_FLASH_SECTOR_4
#define APPHEADER_SIZE					(APPLICATION_ADDRESS - APPHEADER_ADDRESS)           /* Size of page : 64 Kbytes */
/* Define the address from where user application will be loaded. */
#define APPLICATION_ADDRESS			ADDR_FLASH_SECTOR_5
/* 计算APP区每页/扇区的大小 */
#define APP_FLASH_STEP          (ADDR_FLASH_SECTOR_6 - ADDR_FLASH_SECTOR_5)				/* Size of page : 128 Kbytes */
/* End of the Flash address */
#define USER_FLASH_END_ADDRESS        (ADDR_FLASH_SECTOR_11 + APP_FLASH_STEP - 1)	/* End @ of user Flash area : sector start address + sector size -1 */
#endif



/* Error code */
typedef enum 
{
  FLASHIF_OK = 0,
  FLASHIF_ERASEKO,
  FLASHIF_WRITINGCTRL_ERROR,
  FLASHIF_WRITING_ERROR,
  FLASHIF_PROTECTION_ERRROR
}FLASHIF_RES;

/* protection type */  
enum{
  FLASHIF_PROTECTION_NONE         = 0,
  FLASHIF_PROTECTION_PCROPENABLED = 0x1,
  FLASHIF_PROTECTION_WRPENABLED   = 0x2,
  FLASHIF_PROTECTION_RDPENABLED   = 0x4,
};


#if FLASH_IS_32K

/* protection update */
enum {
	FLASHIF_WRP_ENABLE,
	FLASHIF_WRP_DISABLE
};

/* Define bitmap representing user flash area that could be write protected (check restricted to pages 8-39). */
#define FLASH_PAGE_TO_BE_PROTECTED (OB_WRP_PAGES16TO19 | OB_WRP_PAGES20TO23 | OB_WRP_PAGES24TO27 | OB_WRP_PAGES28TO31 )//|OB_WRP_PAGES12TO15

#elif FLASH_IS_64K

/* protection update */
enum {
	FLASHIF_WRP_ENABLE,
	FLASHIF_WRP_DISABLE
};

/* Define bitmap representing user flash area that could be write protected (check restricted to pages 8-39). */
#define FLASH_PAGE_TO_BE_PROTECTED (OB_WRP_PAGES16TO19 | OB_WRP_PAGES20TO23 | OB_WRP_PAGES24TO27 | OB_WRP_PAGES28TO31 | \
                                    OB_WRP_PAGES32TO35 | OB_WRP_PAGES36TO39 | OB_WRP_PAGES40TO43 | OB_WRP_PAGES44TO47 | \
                                    OB_WRP_PAGES48TO51 | OB_WRP_PAGES52TO57 | OB_WRP_PAGES56TO59 | OB_WRP_PAGES60TO63)  
#elif FLASH_IS_1024K
/* Define bitmap representing user flash area that could be write protected */
#define FLASH_SECTOR_TO_BE_PROTECTED (OB_WRP_SECTOR_1 | OB_WRP_SECTOR_2 | OB_WRP_SECTOR_3 | OB_WRP_SECTOR_4 | OB_WRP_SECTOR_5 | OB_WRP_SECTOR_6 |\
                                      OB_WRP_SECTOR_7 | OB_WRP_SECTOR_8 | OB_WRP_SECTOR_9 | OB_WRP_SECTOR_10 | OB_WRP_SECTOR_11)
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void FLASH_If_Init(void);
bool FLASH_If_ErasePage(uint32_t PageAddress);
uint32_t FLASH_If_Write(uint32_t destination, uint32_t *p_source, uint32_t length);
uint16_t FLASH_If_GetWriteProtectionStatus(void);
uint32_t FLASH_If_WriteProtectionConfig(uint32_t modifier);
#endif  /* __FLASH_IF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
