#include "mt48lc4m32b2.h"

static FMC_SDRAM_CommandTypeDef Command;

/**
 * @brief  Initializes the MT48LC4M32B2 SDRAM memory
 * @param  Ctx : Component object pointer
 * @param  pRegMode : Pointer to Register Mode stucture
 * @retval error status
 */
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_Init(SDRAM_HandleTypeDef *Ctx, MT48LC4M32B2_ContextTypeDef *pRegMode)
{
    int32_t ret = MT48LC4M32B2_ERROR;

    /* Step 1: Configure a clock configuration enable command */
    if (MT48LC4M32B2_ClockEnable(Ctx, pRegMode->TargetBank) == MT48LC4M32B2_OK)
    {
        /* Step 2: Insert 100 us minimum delay */
        /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
        HAL_Delay(1);

        /* Step 3: Configure a PALL (precharge all) command */
        if (MT48LC4M32B2_Precharge(Ctx, pRegMode->TargetBank) == MT48LC4M32B2_OK)
        {
            /* Step 4: Configure a Refresh command */
            if (MT48LC4M32B2_RefreshMode(Ctx, pRegMode->TargetBank, pRegMode->RefreshMode) == MT48LC4M32B2_OK)
            {
                /* Step 5: Program the external memory mode register */
                if (MT48LC4M32B2_ModeRegConfig(Ctx, pRegMode) == MT48LC4M32B2_OK)
                {
                    /* Step 6: Set the refresh rate counter */
                    if (MT48LC4M32B2_RefreshRate(Ctx, pRegMode->RefreshRate) == MT48LC4M32B2_OK)
                    {
                        ret = MT48LC4M32B2_OK;
                    }
                }
            }
        }
    }
    return ret;
}

/**
 * @brief  Enable SDRAM clock
 * @param  Ctx : Component object pointer
 * @param  Interface : Could be FMC_SDRAM_CMD_TARGET_BANK1 or
 * FMC_SDRAM_CMD_TARGET_BANK2
 * @retval error status
 */
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_ClockEnable(SDRAM_HandleTypeDef *Ctx, uint32_t Interface)
{
    Command.CommandMode = MT48LC4M32B2_CLK_ENABLE_CMD;
    Command.CommandTarget = Interface;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    if (HAL_SDRAM_SendCommand(Ctx, &Command, MT48LC4M32B2_TIMEOUT) != HAL_OK)
    {
        return MT48LC4M32B2_ERROR;
    }
    else
    {
        return MT48LC4M32B2_OK;
    }
}

/**
 * @brief  Precharge all sdram banks
 * @param  Ctx : Component object pointer
 * @param  Interface : Could be FMC_SDRAM_CMD_TARGET_BANK1 or
 * FMC_SDRAM_CMD_TARGET_BANK2
 * @retval error status
 */
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_Precharge(SDRAM_HandleTypeDef *Ctx, uint32_t Interface)
{
    Command.CommandMode = MT48LC4M32B2_PALL_CMD;
    Command.CommandTarget = Interface;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    if (HAL_SDRAM_SendCommand(Ctx, &Command, MT48LC4M32B2_TIMEOUT) != HAL_OK)
    {
        return MT48LC4M32B2_ERROR;
    }
    else
    {
        return MT48LC4M32B2_OK;
    }
}

/**
 * @brief  Program the external memory mode register
 * @param  Ctx : Component object pointer
 * @param  pRegMode : Pointer to Register Mode stucture
 * @retval error status
 */
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_ModeRegConfig(SDRAM_HandleTypeDef *Ctx, MT48LC4M32B2_ContextTypeDef *pRegMode)
{
    uint32_t tmpmrd;

    /* Program the external memory mode register */
    tmpmrd = (uint32_t)pRegMode->BurstLength | pRegMode->BurstType | pRegMode->CASLatency | pRegMode->OperationMode |
             pRegMode->WriteBurstMode;

    Command.CommandMode = MT48LC4M32B2_LOAD_MODE_CMD;
    Command.CommandTarget = pRegMode->TargetBank;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = tmpmrd;

    /* Send the command */
    if (HAL_SDRAM_SendCommand(Ctx, &Command, MT48LC4M32B2_TIMEOUT) != HAL_OK)
    {
        return MT48LC4M32B2_ERROR;
    }
    else
    {
        return MT48LC4M32B2_OK;
    }
}

/**
 * @brief  Program the SDRAM timing
 * @param  Ctx : Component object pointer
 * @param  pTiming : Pointer to SDRAM timing configuration stucture
 * @retval error status
 */
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_TimingConfig(SDRAM_HandleTypeDef *Ctx, FMC_SDRAM_TimingTypeDef *pTiming)
{
    /* Program the SDRAM timing */
    if (HAL_SDRAM_Init(Ctx, pTiming) != HAL_OK)
    {
        return MT48LC4M32B2_ERROR;
    }
    else
    {
        return MT48LC4M32B2_OK;
    }
}

/**
 * @brief  Configure Refresh mode
 * @param  Ctx : Component object pointer
 * @param  Interface : Could be FMC_SDRAM_CMD_TARGET_BANK1 or
 * FMC_SDRAM_CMD_TARGET_BANK2
 * @param  RefreshMode : Could be MT48LC4M32B2_CMD_AUTOREFRESH_MODE or
 *                      MT48LC4M32B2_CMD_SELFREFRESH_MODE
 * @retval error status
 */
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_RefreshMode(SDRAM_HandleTypeDef *Ctx, uint32_t Interface, uint32_t RefreshMode)
{
    Command.CommandMode = RefreshMode;
    Command.CommandTarget = Interface;
    Command.AutoRefreshNumber = 8;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    if (HAL_SDRAM_SendCommand(Ctx, &Command, MT48LC4M32B2_TIMEOUT) != HAL_OK)
    {
        return MT48LC4M32B2_ERROR;
    }
    else
    {
        return MT48LC4M32B2_OK;
    }
}

/**
 * @brief  Set the device refresh rate
 * @param  Ctx : Component object pointer
 * @param  RefreshCount : The refresh rate to be programmed
 * @retval error status
 */
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_RefreshRate(SDRAM_HandleTypeDef *Ctx, uint32_t RefreshCount)
{
    /* Set the device refresh rate */
    if (HAL_SDRAM_ProgramRefreshRate(Ctx, RefreshCount) != HAL_OK)
    {
        return MT48LC4M32B2_ERROR;
    }
    else
    {
        return MT48LC4M32B2_OK;
    }
}

/**
 * @brief  Enter Power mode
 * @param  Ctx : Component object pointer
 * @param  Interface : Could be FMC_SDRAM_CMD_TARGET_BANK1 or
 * FMC_SDRAM_CMD_TARGET_BANK2
 * @retval error status
 */
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_EnterPowerMode(SDRAM_HandleTypeDef *Ctx, uint32_t Interface)
{
    Command.CommandMode = MT48LC4M32B2_POWERDOWN_MODE_CMD;
    Command.CommandTarget = Interface;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    if (HAL_SDRAM_SendCommand(Ctx, &Command, MT48LC4M32B2_TIMEOUT) != HAL_OK)
    {
        return MT48LC4M32B2_ERROR;
    }
    else
    {
        return MT48LC4M32B2_OK;
    }
}

/**
 * @brief  Exit Power mode
 * @param  Ctx : Component object pointer
 * @param  Interface : Could be FMC_SDRAM_CMD_TARGET_BANK1 or
 * FMC_SDRAM_CMD_TARGET_BANK2
 * @retval error status
 */
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_ExitPowerMode(SDRAM_HandleTypeDef *Ctx, uint32_t Interface)
{
    Command.CommandMode = MT48LC4M32B2_NORMAL_MODE_CMD;
    Command.CommandTarget = Interface;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;

    /* Send the command */
    if (HAL_SDRAM_SendCommand(Ctx, &Command, MT48LC4M32B2_TIMEOUT) != HAL_OK)
    {
        return MT48LC4M32B2_ERROR;
    }
    else
    {
        return MT48LC4M32B2_OK;
    }
}

/**
 * @brief  Sends command to the SDRAM bank.
 * @param  Ctx : Component object pointer
 * @param  SdramCmd : Pointer to SDRAM command structure
 * @retval SDRAM status
 */
MT48LC4M32B2_StatusTypeDef MT48LC4M32B2_Sendcmd(SDRAM_HandleTypeDef *Ctx, FMC_SDRAM_CommandTypeDef *SdramCmd)
{
    if (HAL_SDRAM_SendCommand(Ctx, SdramCmd, MT48LC4M32B2_TIMEOUT) != HAL_OK)
    {
        return MT48LC4M32B2_ERROR;
    }
    else
    {
        return MT48LC4M32B2_OK;
    }
}
