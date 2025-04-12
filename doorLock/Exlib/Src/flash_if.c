/**
  ******************************************************************************
  * @file    IAP/IAP_Main/Src/flash_if.c 
  * @author  MCD Application Team
  * @brief   This file provides all the memory related operation functions.
  ******************************************************************************
  */ 

/** @addtogroup STM32F1xx_IAP / STM32F4xx_IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#if FLASH_IS_1024K
static uint32_t GetSector(uint32_t Address);
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{
  /* Unlock the Program memory */
	HAL_FLASH_Unlock();

#if FLASH_IS_32K
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
#elif FLASH_IS_64K
	/* Clear all FLASH flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
#elif FLASH_IS_1024K
  /* Clear pending flags (if any) */  
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                         FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
#endif
	
	/* Unlock the Program memory */
  HAL_FLASH_Lock();
}


bool FLASH_If_ErasePage(uint32_t PageAddress)
{
  /* Proceed to erase the page */
  SET_BIT(FLASH->CR, FLASH_CR_PER);
  WRITE_REG(FLASH->AR, PageAddress);
  SET_BIT(FLASH->CR, FLASH_CR_STRT);
  while (FLASH->SR & FLASH_SR_BSY) continue;
  
  return true;
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  destination: start address for target location
  * @param  p_source: pointer on buffer with data to write
  * @param  length: length of data buffer (unit is 32-bit word)
  * @retval uint32_t 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32_t FLASH_If_Write(uint32_t destination, uint32_t *p_source, uint32_t length)
{
  uint32_t i = 0;
  /* Unlock the Flash to enable the flash control register access *************/

	HAL_FLASH_Unlock();
	if(READ_BIT(FLASH->CR, FLASH_CR_LOCK) != RESET)
	{
		//printf("Unlock Fail!\n\r");
	}
	FLASH->CR = FLASH_CR_PG;
	//printf("length : %d\r\n",length);
	//printf("destination : %08x,END_ADDRESS:%08x\r\n",destination,(USER_FLASH_END_ADDRESS-4));
  for (i = 0; (i < length) && (destination <= (USER_FLASH_END_ADDRESS-4)); i++)
  {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */ 
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, destination, *(uint32_t*)(p_source+i)) == HAL_OK) 
    {
     /* Check the written value */
			if (*(uint32_t*)destination != *(uint32_t*)(p_source+i))
      {
        /* Flash content doesn't match SRAM content */
        return(FLASHIF_WRITINGCTRL_ERROR);
      }
      /* Increment FLASH destination address */
      destination += 4;
    }
    else
    {
      /* Error occurred while writing data in Flash memory */
			//printf("FLASHIF_WRITING_ERROR\r\n");
      return (FLASHIF_WRITING_ERROR);
    }
  }
	if (FLASH->SR & (FLASH_SR_PGERR | FLASH_SR_WRPERR)) return FLASHIF_WRITING_ERROR;
  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  return (FLASHIF_OK);
}

/**
  * @brief  Returns the write protection status of application flash area.
  * @param  None
  * @retval 0: No write protected sectors inside the application flash area
  *         1: Some sectors inside the application flash area are write protected
  */
uint16_t FLASH_If_GetWriteProtectionStatus(void)
{
  uint32_t ProtectedStatus = FLASHIF_PROTECTION_NONE;
  FLASH_OBProgramInitTypeDef OptionsBytesStruct;

  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();

  /* Check if there are write protected sectors inside the user flash area ****/
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();

  /* Get pages already write protected ****************************************/
#if FLASH_IS_32K
	ProtectedStatus = ~(OptionsBytesStruct.WRPPage) & FLASH_PAGE_TO_BE_PROTECTED;
#elif FLASH_IS_64K
	ProtectedStatus = ~(OptionsBytesStruct.WRPPage) & FLASH_PAGE_TO_BE_PROTECTED;
#elif FLASH_IS_1024K
  ProtectedStatus = ~(OptionsBytesStruct.WRPSector) & FLASH_SECTOR_TO_BE_PROTECTED;
#endif
	
  /* Check if desired pages are already write protected ***********************/
  if(ProtectedStatus != 0)
  {
    /* Some sectors inside the user flash area are write protected */
    return FLASHIF_PROTECTION_WRPENABLED;
  }
  else
  { 
    /* No write protected sectors inside the user flash area */
    return FLASHIF_PROTECTION_NONE;
  }
}

#if FLASH_IS_1024K
/**
  * @brief  Gets the sector of a given address
  * @param  Address: Flash address
  * @retval The sector of a given address
  */
static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/
  {
    sector = FLASH_SECTOR_11;  
  }
  return sector;
}
#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
