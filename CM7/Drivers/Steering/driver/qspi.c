#include "qspi.h"

BSP_QSPI_Ctx_t QSPI_Ctx;

static int32_t QSPI_ResetMemory();
static int32_t QSPI_DummyCyclesCfg();

/**
 * @brief  Initializes the QSPI interface.
 * @param  Init       QSPI Init structure
 * @retval BSP status
 */
int32_t BSP_QSPI_Init(BSP_QSPI_Init_t *Init)
{
	int32_t ret = BSP_ERROR_NONE;

	/* Check if instance is already initialized */
	if (QSPI_Ctx.IsInitialized == QSPI_ACCESS_NONE) {
		QSPI_Ctx.InterfaceMode = Init->InterfaceMode;
		QSPI_Ctx.TransferRate = Init->TransferRate;
		QSPI_Ctx.DualFlashMode = Init->DualFlashMode;
		if (QSPI_ResetMemory() != BSP_ERROR_NONE) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}/* Force Flash enter 4 Byte address mode */
		else if (MT25TL01G_AutoPollingMemReady(&hqspi,
				QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		} else if (MT25TL01G_Enter4BytesAddressMode(&hqspi,
				QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}/* Configuration of the dummy cycles on QSPI memory side */
		else if (QSPI_DummyCyclesCfg() != BSP_ERROR_NONE) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		} else {
			/* Configure Flash to desired mode */
			if (BSP_QSPI_ConfigFlash(Init->InterfaceMode,
					Init->TransferRate) != BSP_ERROR_NONE) {
				ret = BSP_ERROR_COMPONENT_FAILURE;
			}
		}
	}

	/* Return BSP status */
	return ret;
}

/**
 * @brief  De-Initializes the QSPI interface.
 * @retval BSP status
 */
int32_t BSP_QSPI_DeInit()
{
	int32_t ret = BSP_ERROR_NONE;

	if (QSPI_Ctx.IsInitialized == QSPI_ACCESS_MMP) {
		if (BSP_QSPI_DisableMemoryMappedMode() != BSP_ERROR_NONE) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}
	}

	if (ret == BSP_ERROR_NONE) {
		/* Set default QSPI_Ctx values */
		QSPI_Ctx.IsInitialized = QSPI_ACCESS_NONE;
		QSPI_Ctx.InterfaceMode = BSP_QSPI_SPI_MODE;
		QSPI_Ctx.TransferRate = BSP_QSPI_STR_TRANSFER;
		QSPI_Ctx.DualFlashMode = QSPI_DUALFLASH_ENABLE;

		/* Call the DeInit function to reset the driver */
		if (HAL_QSPI_DeInit(&hqspi) != HAL_OK) {
			ret = BSP_ERROR_PERIPH_FAILURE;
		}
	}

	/* Return BSP status */
	return ret;
}

/**
 * @brief  Reads an amount of data from the QSPI memory.
 * @param  pData     Pointer to data to be read
 * @param  ReadAddr  Read start address
 * @param  Size      Size of data to read
 * @retval BSP status
 */
int32_t BSP_QSPI_Read(uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
	int32_t ret = BSP_ERROR_NONE;

	if (QSPI_Ctx.TransferRate == BSP_QSPI_STR_TRANSFER) {
		if (MT25TL01G_ReadSTR(&hqspi, QSPI_Ctx.InterfaceMode, pData, ReadAddr,
				Size) != MT25TL01G_OK) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}
	} else {
		if (MT25TL01G_ReadDTR(&hqspi, QSPI_Ctx.InterfaceMode, pData, ReadAddr,
				Size) != MT25TL01G_OK) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}
	}

	/* Return BSP status */
	return ret;
}

/**
 * @brief  Writes an amount of data to the QSPI memory.
 * @param  pData      Pointer to data to be written
 * @param  WriteAddr  Write start address
 * @param  Size       Size of data to write
 * @retval BSP status
 */
int32_t BSP_QSPI_Write(uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
	int32_t ret = BSP_ERROR_NONE;
	uint32_t end_addr, current_size, current_addr;
	uint8_t *write_data;

	/* Calculation of the size between the write address and the end of the page */
	current_size = MT25TL01G_PAGE_SIZE - (WriteAddr % MT25TL01G_PAGE_SIZE);

	/* Check if the size of the data is less than the remaining place in the page */
	if (current_size > Size) {
		current_size = Size;
	}

	/* Initialize the address variables */
	current_addr = WriteAddr;
	end_addr = WriteAddr + Size;
	write_data = pData;

	/* Perform the write page by page */
	do {
		/* Check if Flash busy ? */
		if (MT25TL01G_AutoPollingMemReady(&hqspi,
				QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}/* Enable write operations */
		else if (MT25TL01G_WriteEnable(&hqspi,
				QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}/* Issue page program command */
		else if (MT25TL01G_PageProgram(&hqspi, QSPI_Ctx.InterfaceMode, write_data,
				current_addr, current_size) != MT25TL01G_OK) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}/* Configure automatic polling mode to wait for end of program */
		else if (MT25TL01G_AutoPollingMemReady(&hqspi,
				QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		} else {
			/* Update the address and size variables for next page programming */
			current_addr += current_size;
			write_data += current_size;
			current_size =
					((current_addr + MT25TL01G_PAGE_SIZE) > end_addr) ?
							(end_addr - current_addr) : MT25TL01G_PAGE_SIZE;
		}
	} while ((current_addr < end_addr) && (ret == BSP_ERROR_NONE));

	/* Return BSP status */
	return ret;
}

/**
 * @brief  Erases the specified block of the QSPI memory.
 *         MT25TL01G support 4K, 32K, 64K size block erase commands for each Die.
 *           i.e 8K, 64K, 128K at BSP level (see BSP_QSPI_Erase_t type definition)
 * @param  BlockAddress Block address to erase
 * @param  BlockSize    Erase Block size
 * @retval BSP status
 */
int32_t BSP_QSPI_EraseBlock(uint32_t BlockAddress, BSP_QSPI_Erase_t BlockSize)
{
	int32_t ret = BSP_ERROR_NONE;

	/* Check Flash busy ? */
	if (MT25TL01G_AutoPollingMemReady(&hqspi,
			QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
		ret = BSP_ERROR_COMPONENT_FAILURE;
	}/* Enable write operations */
	else if (MT25TL01G_WriteEnable(&hqspi, QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
		ret = BSP_ERROR_COMPONENT_FAILURE;
	} else {
		/* Issue Block Erase command */
		if (MT25TL01G_BlockErase(&hqspi, QSPI_Ctx.InterfaceMode, BlockAddress,
				(MT25TL01G_EraseTypeDef) BlockSize) != MT25TL01G_OK) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}
	}

	/* Return BSP status */
	return ret;
}

/**
 * @brief  Erases the entire QSPI memory.
 * @retval BSP status
 */
int32_t BSP_QSPI_EraseChip()
{
	int32_t ret = BSP_ERROR_NONE;

	/* Check Flash busy ? */
	if (MT25TL01G_AutoPollingMemReady(&hqspi,
			QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
		ret = BSP_ERROR_COMPONENT_FAILURE;
	}/* Enable write operations */
	else if (MT25TL01G_WriteEnable(&hqspi, QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
		ret = BSP_ERROR_COMPONENT_FAILURE;
	} else {
		/* Issue Chip erase command */
		if (MT25TL01G_ChipErase(&hqspi, QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}
	}

	/* Return BSP status */
	return ret;
}

/**
 * @brief  Reads current status of the QSPI memory.
 *         If WIP != 0 then return busy.
 * @retval QSPI memory status: whether busy or not
 */
int32_t BSP_QSPI_GetStatus()
{
	int32_t ret = BSP_ERROR_NONE;
	uint8_t reg;

	if (MT25TL01G_ReadStatusRegister(&hqspi, QSPI_Ctx.InterfaceMode,
			&reg) != MT25TL01G_OK) {
		ret = BSP_ERROR_COMPONENT_FAILURE;
	} else {
		/* Check the value of the register */
		if ((reg & MT25TL01G_SR_WIP) != 0U) {
			ret = BSP_ERROR_BUSY;
		}
	}

	/* Return BSP status */
	return ret;
}

/**
 * @brief  Return the configuration of the QSPI memory.
 * @param  pInfo     pointer on the configuration structure
 * @retval BSP status
 */
int32_t BSP_QSPI_GetInfo(BSP_QSPI_Info_t *pInfo)
{
	int32_t ret = BSP_ERROR_NONE;

	(void) MT25TL01G_GetFlashInfo(pInfo);

	/* Return BSP status */
	return ret;
}

/**
 * @brief  Configure the QSPI in memory-mapped mode
 *         Only 1 Instance can running MMP mode. And it will lock system at this mode.
 * @retval BSP status
 */
int32_t BSP_QSPI_EnableMemoryMappedMode()
{
	int32_t ret = BSP_ERROR_NONE;

	if (QSPI_Ctx.TransferRate == BSP_QSPI_STR_TRANSFER) {
		if (MT25TL01G_EnableMemoryMappedModeSTR(&hqspi,
				QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		} else /* Update QSPI context if all operations are well done */
		{
			QSPI_Ctx.IsInitialized = QSPI_ACCESS_MMP;
		}
	} else {
		if (MT25TL01G_EnableMemoryMappedModeDTR(&hqspi,
				QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		} else /* Update QSPI context if all operations are well done */
		{
			QSPI_Ctx.IsInitialized = QSPI_ACCESS_MMP;
		}
	}

	/* Return BSP status */
	return ret;
}

/**
 * @brief  Exit form memory-mapped mode
 *         Only 1 Instance can running MMP mode. And it will lock system at this mode.
 * @retval BSP status
 */
int32_t BSP_QSPI_DisableMemoryMappedMode()
{
	uint8_t Dummy;
	int32_t ret = BSP_ERROR_NONE;

	if (QSPI_Ctx.IsInitialized != QSPI_ACCESS_MMP) {
		ret = BSP_ERROR_QSPI_MMP_UNLOCK_FAILURE;
	}/* Abort MMP back to indirect mode */
	else if (HAL_QSPI_Abort(&hqspi) != HAL_OK) {
		ret = BSP_ERROR_PERIPH_FAILURE;
	} else {
		/* Force QSPI interface Sampling Shift to half cycle */
		hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;

		if (HAL_QSPI_Init(&hqspi) != HAL_OK) {
			ret = BSP_ERROR_PERIPH_FAILURE;
		}
		/* Dummy read for exit from Performance Enhance mode */
		else if (MT25TL01G_ReadSTR(&hqspi, QSPI_Ctx.InterfaceMode, &Dummy, 0,
				1) != MT25TL01G_OK) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		} else /* Update QSPI context if all operations are well done */
		{
			QSPI_Ctx.IsInitialized = QSPI_ACCESS_INDIRECT;
		}
	}
	/* Return BSP status */
	return ret;
}

/**
 * @brief  Get flash ID, 3 Byte
 *         Manufacturer ID, Memory type, Memory density
 * @param  Id QSPI Identifier
 * @retval BSP status
 */
int32_t BSP_QSPI_ReadID(uint8_t *Id)
{
	int32_t ret = BSP_ERROR_NONE;

	if (MT25TL01G_ReadID(&hqspi, QSPI_Ctx.InterfaceMode, Id) != MT25TL01G_OK) {
		ret = BSP_ERROR_COMPONENT_FAILURE;
	}

	/* Return BSP status */
	return ret;
}

/**
 * @brief  Set Flash to desired Interface mode. And this instance becomes current instance.
 *         If current instance running at MMP mode then this function isn't work.
 *         Indirect -> Indirect
 * @param  Mode      QSPI mode
 * @param  Rate      QSPI transfer rate
 * @retval BSP status
 */
int32_t BSP_QSPI_ConfigFlash(BSP_QSPI_Interface_t Mode,
		BSP_QSPI_Transfer_t Rate)
{
	int32_t ret = BSP_ERROR_NONE;

	/* Check if MMP mode locked ************************************************/
	if (QSPI_Ctx.IsInitialized == QSPI_ACCESS_MMP) {
		ret = BSP_ERROR_QSPI_MMP_LOCK_FAILURE;
	} else {
		/* Setup MCU transfer rate setting ***************************************************/
		hqspi.Init.SampleShifting =
				(Rate == BSP_QSPI_STR_TRANSFER ) ?
						QSPI_SAMPLE_SHIFTING_HALFCYCLE : QSPI_SAMPLE_SHIFTING_NONE;

		if (HAL_QSPI_Init(&hqspi) != HAL_OK) {
			ret = BSP_ERROR_PERIPH_FAILURE;
		} else {
			/* Setup Flash interface ***************************************************/
			switch (QSPI_Ctx.InterfaceMode)
			{
			case MT25TL01G_QPI_MODE: /* 4-4-4 commands */
				if (Mode != MT25TL01G_QPI_MODE) {
					if (MT25TL01G_ExitQPIMode(&hqspi) != MT25TL01G_OK) {
						ret = BSP_ERROR_COMPONENT_FAILURE;
					}
				}
				break;

			case BSP_QSPI_SPI_MODE : /* 1-1-1 commands, Power on H/W default setting */
			case BSP_QSPI_SPI_2IO_MODE : /* 1-2-2 read commands */
			case BSP_QSPI_SPI_4IO_MODE : /* 1-4-4 read commands */
			default:
				if (Mode == MT25TL01G_QPI_MODE) {
					if (MT25TL01G_EnterQPIMode(&hqspi) != MT25TL01G_OK) {
						ret = BSP_ERROR_COMPONENT_FAILURE;
					}
				}
				break;
			}

			/* Update QSPI context if all operations are well done */
			if (ret == BSP_ERROR_NONE) {
				/* Update current status parameter *****************************************/
				QSPI_Ctx.IsInitialized = QSPI_ACCESS_INDIRECT;
				QSPI_Ctx.InterfaceMode = Mode;
				QSPI_Ctx.TransferRate = Rate;
			}
		}
	}

	/* Return BSP status */
	return ret;
}

/**
 * @brief  This function reset the QSPI Flash memory.
 *         Fore QPI+SPI reset to avoid system come from unknown status.
 *         Flash accept 1-1-1, 1-1-2, 1-2-2 commands after reset.
 * @retval BSP status
 */
static int32_t QSPI_ResetMemory()
{
	int32_t ret = BSP_ERROR_NONE;

	/* Send RESET ENABLE command in QPI mode (QUAD I/Os, 4-4-4) */
	if (MT25TL01G_ResetEnable(&hqspi, MT25TL01G_QPI_MODE) != MT25TL01G_OK) {
		ret = BSP_ERROR_COMPONENT_FAILURE;
	}/* Send RESET memory command in QPI mode (QUAD I/Os, 4-4-4) */
	else if (MT25TL01G_ResetMemory(&hqspi, MT25TL01G_QPI_MODE) != MT25TL01G_OK) {
		ret = BSP_ERROR_COMPONENT_FAILURE;
	}/* Wait Flash ready */
	else if (MT25TL01G_AutoPollingMemReady(&hqspi,
			QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
		ret = BSP_ERROR_COMPONENT_FAILURE;
	}/* Send RESET ENABLE command in SPI mode (1-1-1) */
	else if (MT25TL01G_ResetEnable(&hqspi, BSP_QSPI_SPI_MODE) != MT25TL01G_OK) {
		ret = BSP_ERROR_COMPONENT_FAILURE;
	}/* Send RESET memory command in SPI mode (1-1-1) */
	else if (MT25TL01G_ResetMemory(&hqspi, BSP_QSPI_SPI_MODE) != MT25TL01G_OK) {
		ret = BSP_ERROR_COMPONENT_FAILURE;
	} else {
		QSPI_Ctx.IsInitialized = QSPI_ACCESS_INDIRECT; /* After reset S/W setting to indirect access   */
		QSPI_Ctx.InterfaceMode = BSP_QSPI_SPI_MODE; /* After reset H/W back to SPI mode by default  */
		QSPI_Ctx.TransferRate = BSP_QSPI_STR_TRANSFER; /* After reset S/W setting to STR mode          */

	}

	/* Return BSP status */
	return ret;
}

/**
 * @brief  This function configure the dummy cycles on memory side.
 *         Dummy cycle bit locate in Configuration Register[7:6]
 * @retval BSP status
 */
static int32_t QSPI_DummyCyclesCfg()
{
	int32_t ret = BSP_ERROR_NONE;
	QSPI_CommandTypeDef s_command;
	uint16_t reg = 0;

	/* Initialize the read volatile configuration register command */
	s_command.InstructionMode = QSPI_INSTRUCTION_4_LINES;
	s_command.Instruction = MT25TL01G_READ_VOL_CFG_REG_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode = QSPI_DATA_4_LINES;
	s_command.DummyCycles = 0;
	s_command.NbData = 2;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	/* Configure the command */
	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK) {
		return BSP_ERROR_COMPONENT_FAILURE;
	}

	/* Reception of the data */
	if (HAL_QSPI_Receive(&hqspi, (uint8_t*) (&reg),
			HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return BSP_ERROR_COMPONENT_FAILURE;
	}

	/* Enable write operations */
	if (MT25TL01G_WriteEnable(&hqspi, QSPI_Ctx.InterfaceMode) != MT25TL01G_OK) {
		return BSP_ERROR_COMPONENT_FAILURE;
	}

	/* Update volatile configuration register (with new dummy cycles) */
	s_command.Instruction = MT25TL01G_WRITE_VOL_CFG_REG_CMD;
	MODIFY_REG(reg, 0xF0F0,
			((MT25TL01G_DUMMY_CYCLES_READ_QUAD << 4) | (MT25TL01G_DUMMY_CYCLES_READ_QUAD << 12)));

	/* Configure the write volatile configuration register command */
	if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE)
			!= HAL_OK) {
		return BSP_ERROR_COMPONENT_FAILURE;
	}

	/* Transmission of the data */
	if (HAL_QSPI_Transmit(&hqspi, (uint8_t*) (&reg),
			HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
		return BSP_ERROR_COMPONENT_FAILURE;
	}

	/* Return BSP status */
	return ret;
}

