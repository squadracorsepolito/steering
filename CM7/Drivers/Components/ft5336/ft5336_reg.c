#include "ft5336_reg.h"
#include "i2c.h"

/**
 * @brief  Read FT5336 registers.
 * @param  reg Component reg to read from
 * @param  pdata pointer to data to be read
 * @param  length Length of data to read
 * @retval Component status
 */
int32_t ft5336_read_reg(uint8_t reg, uint8_t *pdata, uint16_t length)
{
  return HAL_I2C_Mem_Read(&hi2c4, FT5336_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, pdata, length, HAL_MAX_DELAY);
}

/**
 * @brief  Write FT5336 registers.
 * @param  reg Component reg to write to
 * @param  pdata pointer to data to be written
 * @param  length Length of data to write
 * @retval Component status
 */
int32_t ft5336_write_reg(uint8_t reg, uint8_t *pdata, uint16_t length)
{
  return HAL_I2C_Mem_Write(&hi2c4, FT5336_I2C_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, pdata, length, HAL_MAX_DELAY);
}

/**
 * @brief  Set FT5336 working mode
 * @param  value Value to write to FT5336_DEV_MODE_REG register
 * @retval Component status
 */
int32_t ft5336_dev_mode_w(uint8_t value)
{
  int32_t ret;
  uint8_t tmp;

  ret = ft5336_read_reg(FT5336_DEV_MODE_REG, &tmp, 1);

  if (ret == 0)
  {
    tmp &= ~FT5336_DEV_MODE_BIT_MASK;
    tmp |= value << FT5336_DEV_MODE_BIT_POSITION;

    ret = ft5336_write_reg(FT5336_DEV_MODE_REG, &tmp, 1);
  }

  return ret;
}

/**
 * @brief  Get FT5336 working mode
 * @param  value pointer to the value of FT5336_DEV_MODE_REG register
 * @retval Component status
 */
int32_t ft5336_dev_mode_r(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_DEV_MODE_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_DEV_MODE_BIT_MASK;
    *value = *value >> FT5336_DEV_MODE_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 gesture ID
 * @param  value pointer to the value of FT5336_GEST_ID_REG register
 * @retval Component status
 */
int32_t ft5336_gest_id(uint8_t *value)
{
  return ft5336_read_reg(FT5336_GEST_ID_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 Touch Data Status
 * @param  value pointer to the value of Touch Data Status register
 * @retval Component status
 */
int32_t ft5336_td_status(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_TD_STAT_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_TD_STATUS_BIT_MASK;
    *value = *value >> FT5336_TD_STATUS_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 First X Event Flag
 * @param  value pointer to the value of FT5336_P1_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p1_xh_ef(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P1_XH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P1_XH_EF_BIT_MASK;
    *value = *value >> FT5336_P1_XH_EF_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 First X High Touch Position
 * @param  value pointer to the value of FT5336_P1_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p1_xh_tp(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P1_XH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P1_XH_TP_BIT_MASK;
    *value = *value >> FT5336_P1_XH_TP_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 First X Low Touch Position
 * @param  value pointer to the value of FT5336_P1_XL_REG register
 * @retval Component status
 */
int32_t ft5336_p1_xl_tp(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P1_XL_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 First Touch ID
 * @param  value pointer to the value of FT5336_P1_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p1_yh_tid(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P1_YH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P1_YH_TID_BIT_MASK;
    *value = *value >> FT5336_P1_YH_TID_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 First Y High Touch Position
 * @param  value pointer to the value of FT5336_P1_YH_REG register
 * @retval Component status
 */
int32_t ft5336_p1_yh_tp(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P1_YH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P1_YH_TP_BIT_MASK;
    *value = *value >> FT5336_P1_YH_TP_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 First Y Low Touch Position
 * @param  value pointer to the value of FT5336_P1_YL_REG register
 * @retval Component status
 */
int32_t ft5336_p1_yl_tp(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P1_YL_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 First Touch pressure
 * @param  value pointer to the value of FT5336_P1_WEIGHT_REG register
 * @retval Component status
 */
int32_t ft5336_p1_weight(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P1_WEIGHT_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 First Touch area
 * @param  value pointer to the value of FT5336_P1_MISC_REG register
 * @retval Component status
 */
int32_t ft5336_p1_misc(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P1_MISC_REG, (uint8_t *)value, 1);
  if (ret == 0)
  {
    *value &= FT5336_P1_MISC_BIT_MASK;
    *value = *value >> FT5336_P1_MISC_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Second X Event Flag
 * @param  value pointer to the value of FT5336_P2_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p2_xh_ef(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P2_XH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P2_XH_EF_BIT_MASK;
    *value = *value >> FT5336_P2_XH_EF_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Second X High Touch Position
 * @param  value pointer to the value of FT5336_P2_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p2_xh_tp(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P2_XH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P2_XH_TP_BIT_MASK;
    *value = *value >> FT5336_P2_XH_TP_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Second X Low Touch Position
 * @param  value pointer to the value of FT5336_P2_XL_REG register
 * @retval Component status
 */
int32_t ft5336_p2_xl_tp(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P2_XL_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 Second Touch ID
 * @param  value pointer to the value of FT5336_P2_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p2_yh_tid(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P2_YH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P2_YH_TID_BIT_MASK;
    *value = *value >> FT5336_P2_YH_TID_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Second Y High Touch Position
 * @param  value pointer to the value of FT5336_P2_YH_REG register
 * @retval Component status
 */
int32_t ft5336_p2_yh_tp(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P2_YH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P2_YH_TP_BIT_MASK;
    *value = *value >> FT5336_P2_YH_TP_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Second Y Low Touch Position
 * @param  value pointer to the value of FT5336_P2_YL_REG register
 * @retval Component status
 */
int32_t ft5336_p2_yl_tp(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P2_YL_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 Second Touch pressure
 * @param  value pointer to the value of FT5336_P2_WEIGHT_REG register
 * @retval Component status
 */
int32_t ft5336_p2_weight(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P2_WEIGHT_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 Second Touch area
 * @param  value pointer to the value of FT5336_P2_MISC_REG register
 * @retval Component status
 */
int32_t ft5336_p2_misc(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P2_MISC_REG, (uint8_t *)value, 1);
  if (ret == 0)
  {
    *value &= FT5336_P2_MISC_BIT_MASK;
    *value = *value >> FT5336_P2_MISC_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Third X Event Flag
 * @param  value pointer to the value of FT5336_P3_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p3_xh_ef(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P3_XH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P3_XH_EF_BIT_MASK;
    *value = *value >> FT5336_P3_XH_EF_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Third X High Touch Position
 * @param  value pointer to the value of FT5336_P3_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p3_xh_tp(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P3_XH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P3_XH_TP_BIT_MASK;
    *value = *value >> FT5336_P3_XH_TP_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Third X Low Touch Position
 * @param  value pointer to the value of FT5336_P3_XL_REG register
 * @retval Component status
 */
int32_t ft5336_p3_xl_tp(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P3_XL_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 Third Touch ID
 * @param  value pointer to the value of FT5336_P3_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p3_yh_tid(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P3_YH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P3_YH_TID_BIT_MASK;
    *value = *value >> FT5336_P3_YH_TID_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Third Y High Touch Position
 * @param  value pointer to the value of FT5336_P3_YH_REG register
 * @retval Component status
 */
int32_t ft5336_p3_yh_tp(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P3_YH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P3_YH_TP_BIT_MASK;
    *value = *value >> FT5336_P3_YH_TP_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Third Y Low Touch Position
 * @param  value pointer to the value of FT5336_P3_YL_REG register
 * @retval Component status
 */
int32_t ft5336_p3_yl_tp(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P3_YL_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 Third Touch pressure
 * @param  value pointer to the value of FT5336_P3_WEIGHT_REG register
 * @retval Component status
 */
int32_t ft5336_p3_weight(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P3_WEIGHT_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 Third Touch area
 * @param  value pointer to the value of FT5336_P3_MISC_REG register
 * @retval Component status
 */
int32_t ft5336_p3_misc(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P3_MISC_REG, (uint8_t *)value, 1);
  if (ret == 0)
  {
    *value &= FT5336_P3_MISC_BIT_MASK;
    *value = *value >> FT5336_P3_MISC_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Fourth X Event Flag
 * @param  value pointer to the value of FT5336_P4_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p4_xh_ef(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P4_XH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P4_XH_EF_BIT_MASK;
    *value = *value >> FT5336_P4_XH_EF_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Fourth X High Touch Position
 * @param  value pointer to the value of FT5336_P4_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p4_xh_tp(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P4_XH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P4_XH_TP_BIT_MASK;
    *value = *value >> FT5336_P4_XH_TP_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Fourth X Low Touch Position
 * @param  value pointer to the value of FT5336_P4_XL_REG register
 * @retval Component status
 */
int32_t ft5336_p4_xl_tp(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P4_XL_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 Fourth Touch ID
 * @param  value pointer to the value of FT5336_P4_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p4_yh_tid(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P4_YH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P4_YH_TID_BIT_MASK;
    *value = *value >> FT5336_P4_YH_TID_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Fourth Y High Touch Position
 * @param  value pointer to the value of FT5336_P4_YH_REG register
 * @retval Component status
 */
int32_t ft5336_p4_yh_tp(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P4_YH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P4_YH_TP_BIT_MASK;
    *value = *value >> FT5336_P4_YH_TP_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Fourth Y Low Touch Position
 * @param  value pointer to the value of FT5336_P4_YL_REG register
 * @retval Component status
 */
int32_t ft5336_p4_yl_tp(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P4_YL_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 Fourth Touch pressure
 * @param  value pointer to the value of FT5336_P4_WEIGHT_REG register
 * @retval Component status
 */
int32_t ft5336_p4_weight(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P4_WEIGHT_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 Fourth Touch area
 * @param  value pointer to the value of FT5336_P4_MISC_REG register
 * @retval Component status
 */
int32_t ft5336_p4_misc(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P4_MISC_REG, (uint8_t *)value, 1);
  if (ret == 0)
  {
    *value &= FT5336_P4_MISC_BIT_MASK;
    *value = *value >> FT5336_P4_MISC_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Fifth X Event Flag
 * @param  value pointer to the value of FT5336_P5_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p5_xh_ef(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P5_XH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P5_XH_EF_BIT_MASK;
    *value = *value >> FT5336_P5_XH_EF_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Fifth X High Touch Position
 * @param  value pointer to the value of FT5336_P5_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p5_xh_tp(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P5_XH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P5_XH_TP_BIT_MASK;
    *value = *value >> FT5336_P5_XH_TP_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Fifth X Low Touch Position
 * @param  value pointer to the value of FT5336_P5_XL_REG register
 * @retval Component status
 */
int32_t ft5336_p5_xl_tp(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P5_XL_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 Fifth Touch ID
 * @param  value pointer to the value of FT5336_P5_XH_REG register
 * @retval Component status
 */
int32_t ft5336_p5_yh_tid(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P5_YH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P5_YH_TID_BIT_MASK;
    *value = *value >> FT5336_P5_YH_TID_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Fifth Y High Touch Position
 * @param  value pointer to the value of FT5336_P5_YH_REG register
 * @retval Component status
 */
int32_t ft5336_p5_yh_tp(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P5_YH_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_P5_YH_TP_BIT_MASK;
    *value = *value >> FT5336_P5_YH_TP_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Get FT5336 Fifth Y Low Touch Position
 * @param  value pointer to the value of FT5336_P5_YL_REG register
 * @retval Component status
 */
int32_t ft5336_p5_yl_tp(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P5_YL_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 Fifth Touch pressure
 * @param  value pointer to the value of FT5336_P5_WEIGHT_REG register
 * @retval Component status
 */
int32_t ft5336_p5_weight(uint8_t *value)
{
  return ft5336_read_reg(FT5336_P5_WEIGHT_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Get FT5336 Fifth Touch area
 * @param  value pointer to the value of FT5336_P5_MISC_REG register
 * @retval Component status
 */
int32_t ft5336_p5_misc(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_P5_MISC_REG, (uint8_t *)value, 1);
  if (ret == 0)
  {
    *value &= FT5336_P5_MISC_BIT_MASK;
    *value = *value >> FT5336_P5_MISC_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Set FT5336 Threshold for touch detection
 * @param  value Value to write to FT5336_TH_GROUP_REG register
 * @retval Component status
 */
int32_t ft5336_th_group(uint8_t value)
{
  return ft5336_write_reg(FT5336_TH_GROUP_REG, &value, 1);
}

/**
 * @brief  Set FT5336 Filter function coefficient
 * @param  value Value to write to FT5336_TH_DIFF_REG register
 * @retval Component status
 */
int32_t ft5336_th_diff(uint8_t value)
{
  return ft5336_write_reg(FT5336_TH_DIFF_REG, &value, 1);
}

/**
 * @brief  Control the Switch between Active and Monitoring Mode
 * @param  value Value to write to FT5336_CTRL_REG register
 * @retval Component status
 */
int32_t ft5336_ctrl(uint8_t value)
{
  return ft5336_write_reg(FT5336_CTRL_REG, &value, 1);
}

/**
 * @brief  Set the time period of switching from Active mode to Monitor
 * @param  value Value to write to FT5336_TIMEENTERMONITOR_REG register
 * @retval Component status
 */
int32_t ft5336_time_enter_monitor(uint8_t value)
{
  return ft5336_write_reg(FT5336_TIMEENTERMONITOR_REG, &value, 1);
}

/**
 * @brief  Set rate in Active mode
 * @param  value Value to write to FT5336_PERIODACTIVE_REG register
 * @retval Component status
 */
int32_t ft5336_period_active(uint8_t value)
{
  return ft5336_write_reg(FT5336_PERIODACTIVE_REG, &value, 1);
}

/**
 * @brief  Set rate in Monitor mode
 * @param  value Value to write to FT5336_PERIODMONITOR_REG register
 * @retval Component status
 */
int32_t ft5336_period_monitor(uint8_t value)
{
  return ft5336_write_reg(FT5336_PERIODMONITOR_REG, &value, 1);
}

/**
 * @brief  Set the value of the minimum allowed angle while Rotating
 * @param  value Value to write to FT5336_RADIAN_VALUE_REG register
 * @retval Component status
 */
int32_t ft5336_radian_value(uint8_t value)
{
  return ft5336_write_reg(FT5336_RADIAN_VALUE_REG, &value, 1);
}

/**
 * @brief  Set Maximum offset while Moving Left and Moving Right gesture
 * @param  value Value to write to FT5336_OFFSET_LR_REG register
 * @retval Component status
 */
int32_t ft5336_offset_left_right(uint8_t value)
{
  return ft5336_write_reg(FT5336_OFFSET_LR_REG, &value, 1);
}

/**
 * @brief  Set Maximum offset while Moving Up and Moving Down gesture
 * @param  value Value to write to FT5336_OFFSET_UD_REG register
 * @retval Component status
 */
int32_t ft5336_offset_up_down(uint8_t value)
{
  return ft5336_write_reg(FT5336_OFFSET_UD_REG, &value, 1);
}

/**
 * @brief  Set Minimum distance while Moving Left and Moving Right gesture
 * @param  value Value to write to FT5336_DISTANCE_LR_REG register
 * @retval Component status
 */
int32_t ft5336_disatnce_left_right(uint8_t value)
{
  return ft5336_write_reg(FT5336_DISTANCE_LR_REG, &value, 1);
}

/**
 * @brief  Set Minimum distance while Moving Up and Moving Down gesture
 * @param  value Value to write to FT5336_DISTANCE_UD_REG register
 * @retval Component status
 */
int32_t ft5336_distance_up_down(uint8_t value)
{
  return ft5336_write_reg(FT5336_DISTANCE_UD_REG, &value, 1);
}

/**
 * @brief  Set Maximum distance while Zoom In and Zoom Out gesture
 * @param  value Value to write to FT5336_DISTANCE_ZOOM_REG register
 * @retval Component status
 */
int32_t ft5336_distance_zoom(uint8_t value)
{
  return ft5336_write_reg(FT5336_DISTANCE_ZOOM_REG, &value, 1);
}

/**
 * @brief  Read High 8-bit of LIB Version info
 * @param  value Pointer to FT5336_LIB_VER_H_REG register value
 * @retval Component status
 */
int32_t ft5336_lib_ver_high(uint8_t *value)
{
  return ft5336_read_reg(FT5336_LIB_VER_H_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Read Low 8-bit of LIB Version info
 * @param  value Pointer to FT5336_LIB_VER_L_REG register value
 * @retval Component status
 */
int32_t ft5336_lib_ver_low(uint8_t *value)
{
  return ft5336_read_reg(FT5336_LIB_VER_L_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Read status of cipher register
 * @param  value Pointer to FT5336_CIPHER_REG register value
 * @retval Component status
 */
int32_t ft5336_cipher(uint8_t *value)
{
  return ft5336_read_reg(FT5336_CIPHER_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Select Interrupt (polling or trigger) mode
 * @param  value Value to write to FT5336_GMODE_REG register
 * @retval Component status
 */
int32_t ft5336_g_mode(uint8_t value)
{
  return ft5336_write_reg(FT5336_GMODE_REG, &value, 1);
}

/**
 * @brief  Select Current power mode
 * @param  value Value to write to FT5336_PWR_MODE_REG register
 * @retval Component status
 */
int32_t ft5336_pwr_mode(uint8_t value)
{
  return ft5336_write_reg(FT5336_PWR_MODE_REG, &value, 1);
}

/**
 * @brief  Read the Firmware Version
 * @param  value Pointer to FT5336_FIRMID_REG register value
 * @retval Component status
 */
int32_t ft5336_firm_id(uint8_t *value)
{
  return ft5336_read_reg(FT5336_FIRMID_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Read the FocalTech's Panel ID
 * @param  value Pointer to FT5336_CHIP_ID_REG register value
 * @retval Component status
 */
int32_t ft5336_chip_id(uint8_t *value)
{
  int32_t ret;

  ret = ft5336_read_reg(FT5336_CHIP_ID_REG, (uint8_t *)value, 1);

  if (ret == 0)
  {
    *value &= FT5336_CHIP_ID_BIT_MASK;
    *value = *value >> FT5336_CHIP_ID_BIT_POSITION;
  }

  return ret;
}

/**
 * @brief  Read the Release code version
 * @param  value Pointer to FT5336_RELEASE_CODE_ID_REG register value
 * @retval Component status
 */
int32_t ft5336_release_code_id(uint8_t *value)
{
  return ft5336_read_reg(FT5336_RELEASE_CODE_ID_REG, (uint8_t *)value, 1);
}

/**
 * @brief  Select Current Operating mode
 * @param  value Value to write to FT5336_STATE_REG register
 * @retval Component status
 */
int32_t ft5336_state(uint8_t value)
{
  return ft5336_write_reg(FT5336_STATE_REG, &value, 1);
}
