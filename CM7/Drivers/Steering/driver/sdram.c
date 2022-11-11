#include "sdram.h"

/**
 * @brief  Initializes the SDRAM device.
 * @retval BSP status
 */
int32_t BSP_SDRAM_Init()
{
	int32_t ret = BSP_ERROR_NONE;
	static MT48LC4M32B2_Context_t pRegMode;

	/* External memory mode register configuration */
	pRegMode.TargetBank = FMC_SDRAM_CMD_TARGET_BANK2;
	pRegMode.RefreshMode = MT48LC4M32B2_AUTOREFRESH_MODE_CMD;
	pRegMode.RefreshRate = REFRESH_COUNT;
	pRegMode.BurstLength = MT48LC4M32B2_BURST_LENGTH_1;
	pRegMode.BurstType = MT48LC4M32B2_BURST_TYPE_SEQUENTIAL;
	pRegMode.CASLatency = MT48LC4M32B2_CAS_LATENCY_3;
	pRegMode.OperationMode = MT48LC4M32B2_OPERATING_MODE_STANDARD;
	pRegMode.WriteBurstMode = MT48LC4M32B2_WRITEBURST_MODE_SINGLE;

	/* SDRAM initialization sequence */
	if (MT48LC4M32B2_Init(&hsdram1, &pRegMode) != MT48LC4M32B2_OK) {
		ret = BSP_ERROR_COMPONENT_FAILURE;
	}

	return ret;
}

/**
 * @brief  DeInitializes the SDRAM device.
 * @retval BSP status
 */
int32_t BSP_SDRAM_DeInit()
{
	HAL_SDRAM_DeInit(&hsdram1);

	return BSP_ERROR_NONE;
}

/**
 * @brief  Sends command to the SDRAM bank.
 * @param  SdramCmd  Pointer to SDRAM command structure
 * @retval BSP status
 */
int32_t BSP_SDRAM_SendCmd(FMC_SDRAM_CommandTypeDef *SdramCmd)
{
	if (MT48LC4M32B2_Sendcmd(&hsdram1, SdramCmd) != MT48LC4M32B2_OK) {
		return BSP_ERROR_PERIPH_FAILURE;
	}

	return BSP_ERROR_NONE;
}
