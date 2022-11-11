#ifndef QSPI_H
#define QSPI_H

#include "driver_conf.h"
#include "errno.h"
#include "mt25tl01g/mt25tl01g.h"

#define BSP_QSPI_Info_t                 MT25TL01G_Info_t
#define BSP_QSPI_Interface_t            MT25TL01G_Interface_t
#define BSP_QSPI_Transfer_t             MT25TL01G_Transfer_t
#define BSP_QSPI_DualFlash_t            MT25TL01G_DualFlash_t
#define BSP_QSPI_ODS_t                  MT25TL01G_ODS_t

typedef enum
{
	BSP_QSPI_ERASE_8K = MT25TL01G_ERASE_4K, /*!< 8K size Sector erase = 2 x 4K as Dual flash mode is used for this board   */
	BSP_QSPI_ERASE_64K = MT25TL01G_ERASE_32K, /*!< 64K size Sector erase = 2 x 32K as Dual flash mode is used for this board */
	BSP_QSPI_ERASE_128K = MT25TL01G_ERASE_64K, /*!< 128K size Sector erase = 2 x 64K as Dual mode is used for this board      */
	BSP_QSPI_ERASE_CHIP = MT25TL01G_ERASE_CHIP /*!< Whole chip erase */

} BSP_QSPI_Erase_t;

typedef enum
{
	QSPI_ACCESS_NONE = 0, /*!<  Instance not initialized,             */
	QSPI_ACCESS_INDIRECT, /*!<  Instance use indirect mode access     */
	QSPI_ACCESS_MMP /*!<  Instance use Memory Mapped Mode read  */
} BSP_QSPI_Access_t;

typedef struct
{
	BSP_QSPI_Access_t IsInitialized; /*!<  Instance access Flash method     */
	BSP_QSPI_Interface_t InterfaceMode; /*!<  Flash Interface mode of Instance */
	BSP_QSPI_Transfer_t TransferRate; /*!<  Flash Transfer mode of Instance  */
	uint32_t DualFlashMode; /*!<  Flash dual mode                  */
	uint32_t IsMspCallbacksValid;
} BSP_QSPI_Ctx_t;

typedef struct
{
	BSP_QSPI_Interface_t InterfaceMode; /*!<  Current Flash Interface mode */
	BSP_QSPI_Transfer_t TransferRate; /*!<  Current Flash Transfer mode  */
	BSP_QSPI_DualFlash_t DualFlashMode; /*!<  Dual Flash mode              */
} BSP_QSPI_Init_t;

typedef struct
{
	uint32_t FlashSize;
	uint32_t ClockPrescaler;
	uint32_t SampleShifting;
	uint32_t DualFlashMode;
} MX_QSPI_Init_t;

/* Definition for QSPI modes */
#define BSP_QSPI_SPI_MODE            (BSP_QSPI_Interface_t)MT25TL01G_SPI_MODE      /* 1 Cmd Line, 1 Address Line and 1 Data Line    */
#define BSP_QSPI_SPI_1I2O_MODE       (BSP_QSPI_Interface_t)MT25TL01G_SPI_1I2O_MODE /* 1 Cmd Line, 1 Address Line and 2 Data Lines   */
#define BSP_QSPI_SPI_2IO_MODE        (BSP_QSPI_Interface_t)MT25TL01G_SPI_2IO_MODE  /* 1 Cmd Line, 2 Address Lines and 2 Data Lines  */
#define BSP_QSPI_SPI_1I4O_MODE       (BSP_QSPI_Interface_t)MT25TL01G_SPI_1I4O_MODE /* 1 Cmd Line, 1 Address Line and 4 Data Lines   */
#define BSP_QSPI_SPI_4IO_MODE        (BSP_QSPI_Interface_t)MT25TL01G_SPI_4IO_MODE  /* 1 Cmd Line, 4 Address Lines and 4 Data Lines  */
#define BSP_QSPI_DPI_MODE            (BSP_QSPI_Interface_t)MT25TL01G_DPI_MODE      /* 2 Cmd Lines, 2 Address Lines and 2 Data Lines */
#define BSP_QSPI_QPI_MODE            (BSP_QSPI_Interface_t)MT25TL01G_QPI_MODE      /* 4 Cmd Lines, 4 Address Lines and 4 Data Lines */

/* Definition for QSPI transfer rates */
#define BSP_QSPI_STR_TRANSFER        (BSP_QSPI_Transfer_t)MT25TL01G_STR_TRANSFER /* Single Transfer Rate */
#define BSP_QSPI_DTR_TRANSFER        (BSP_QSPI_Transfer_t)MT25TL01G_DTR_TRANSFER /* Double Transfer Rate */

/* Definition for QSPI dual flash mode */
#define BSP_QSPI_DUALFLASH_DISABLE   (BSP_QSPI_DualFlash_t)MT25TL01G_DUALFLASH_DISABLE   /* Dual flash mode enabled  */
/* Definition for QSPI Flash ID */
#define BSP_QSPI_FLASH_ID            QSPI_FLASH_ID_1

/* QSPI block sizes for dual flash */
#define BSP_QSPI_BLOCK_8K            MT25TL01G_SECTOR_4K
#define BSP_QSPI_BLOCK_64K           MT25TL01G_BLOCK_32K
#define BSP_QSPI_BLOCK_128K          MT25TL01G_BLOCK_64K

/* MT25TL01G Micron memory */
/* Size of the flash */
#define QSPI_FLASH_SIZE            26     /* Address bus width to access whole memory space */
#define QSPI_PAGE_SIZE             256

/* QSPI Base Address */
#define QSPI_BASE_ADDRESS          0x90000000

extern QSPI_HandleTypeDef hqspi;
extern BSP_QSPI_Ctx_t QSPI_Ctx;

int32_t BSP_QSPI_Init(BSP_QSPI_Init_t *Init);
int32_t BSP_QSPI_DeInit();
int32_t BSP_QSPI_Read(uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
int32_t BSP_QSPI_Write(uint8_t *pData, uint32_t WriteAddr, uint32_t Size);
int32_t BSP_QSPI_EraseBlock(uint32_t BlockAddress, BSP_QSPI_Erase_t BlockSize);
int32_t BSP_QSPI_EraseChip();
int32_t BSP_QSPI_GetStatus();
int32_t BSP_QSPI_GetInfo(BSP_QSPI_Info_t *pInfo);
int32_t BSP_QSPI_EnableMemoryMappedMode();
int32_t BSP_QSPI_DisableMemoryMappedMode();
int32_t BSP_QSPI_ReadID(uint8_t *Id);
int32_t BSP_QSPI_ConfigFlash(BSP_QSPI_Interface_t Mode,
BSP_QSPI_Transfer_t Rate);

#endif /* QSPI_H */
