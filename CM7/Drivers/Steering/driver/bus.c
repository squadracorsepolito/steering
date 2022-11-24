#include "bus.h"

#ifndef I2C_VALID_TIMING_NBR
#define I2C_VALID_TIMING_NBR                 128U
#endif
#define I2C_SPEED_FREQ_STANDARD                0U    /* 100 kHz */
#define I2C_SPEED_FREQ_FAST                    1U    /* 400 kHz */
#define I2C_SPEED_FREQ_FAST_PLUS               2U    /* 1 MHz */
#define I2C_ANALOG_FILTER_DELAY_MIN            50U   /* ns */
#define I2C_ANALOG_FILTER_DELAY_MAX            260U  /* ns */
#define I2C_USE_ANALOG_FILTER                  1U
#define I2C_DIGITAL_FILTER_COEF                0U
#define I2C_PRESC_MAX                          16U
#define I2C_SCLDEL_MAX                         16U
#define I2C_SDADEL_MAX                         16U
#define I2C_SCLH_MAX                           256U
#define I2C_SCLL_MAX                           256U
#define SEC2NSEC                               1000000000UL

typedef struct
{
	uint32_t freq; /* Frequency in Hz */
	uint32_t freq_min; /* Minimum frequency in Hz */
	uint32_t freq_max; /* Maximum frequency in Hz */
	uint32_t hddat_min; /* Minimum data hold time in ns */
	uint32_t vddat_max; /* Maximum data valid time in ns */
	uint32_t sudat_min; /* Minimum data setup time in ns */
	uint32_t lscl_min; /* Minimum low period of the SCL clock in ns */
	uint32_t hscl_min; /* Minimum high period of SCL clock in ns */
	uint32_t trise; /* Rise time in ns */
	uint32_t tfall; /* Fall time in ns */
	uint32_t dnf; /* Digital noise filter coefficient */
} I2C_Charac_t;

typedef struct
{
	uint32_t presc; /* Timing prescaler */
	uint32_t tscldel; /* SCL delay */
	uint32_t tsdadel; /* SDA delay */
	uint32_t sclh; /* SCL high period */
	uint32_t scll; /* SCL low period */
} I2C_Timings_t;

static const I2C_Charac_t I2C_Charac[] =
{ [I2C_SPEED_FREQ_STANDARD] =
{ .freq = 100000, .freq_min = 80000, .freq_max = 120000, .hddat_min = 0,
		.vddat_max = 3450, .sudat_min = 250, .lscl_min = 4700, .hscl_min = 4000,
		.trise = 640, .tfall = 20, .dnf = I2C_DIGITAL_FILTER_COEF, },
		[I2C_SPEED_FREQ_FAST] =
		{ .freq = 400000, .freq_min = 320000, .freq_max = 480000, .hddat_min = 0,
				.vddat_max = 900, .sudat_min = 100, .lscl_min = 1300, .hscl_min = 600,
				.trise = 250, .tfall = 100, .dnf = I2C_DIGITAL_FILTER_COEF, },
		[I2C_SPEED_FREQ_FAST_PLUS] =
		{ .freq = 1000000, .freq_min = 800000, .freq_max = 1200000, .hddat_min = 0,
				.vddat_max = 450, .sudat_min = 50, .lscl_min = 500, .hscl_min = 260,
				.trise = 60, .tfall = 100, .dnf = I2C_DIGITAL_FILTER_COEF, }, };

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
static uint32_t IsI2c4MspCbValid = 0;
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
static uint32_t I2c4InitCounter = 0;
static I2C_Timings_t I2c_valid_timing[I2C_VALID_TIMING_NBR];
static uint32_t I2c_valid_timing_nbr = 0;
#if defined(BSP_USE_CMSIS_OS)
static osSemaphoreId BspI2cSemaphore = 0;
#endif

static int32_t I2C4_WriteReg(uint16_t DevAddr, uint16_t MemAddSize,
		uint16_t Reg, uint8_t *pData, uint16_t Length);
static int32_t I2C4_ReadReg(uint16_t DevAddr, uint16_t MemAddSize, uint16_t Reg,
		uint8_t *pData, uint16_t Length);
static uint32_t I2C_GetTiming(uint32_t clock_src_freq, uint32_t i2c_freq);
static uint32_t I2C_Compute_SCLL_SCLH(uint32_t clock_src_freq,
		uint32_t I2C_speed);
static void I2C_Compute_PRESC_SCLDEL_SDADEL(uint32_t clock_src_freq,
		uint32_t I2C_speed);

/**
 * @brief  Initializes I2C HAL.
 * @retval BSP status
 */
int32_t BSP_I2C4_Init(void)
{
	int32_t ret = BSP_ERROR_NONE;

	if (I2c4InitCounter == 0U) {
		I2c4InitCounter++;
	}

	return ret;
}

/**
 * @brief  DeInitializes I2C HAL.
 * @retval BSP status
 */
int32_t BSP_I2C4_DeInit(void)
{
	int32_t ret = BSP_ERROR_NONE;

	I2c4InitCounter--;

	if (I2c4InitCounter == 0U) {
		/* Init the I2C */
		if (HAL_I2C_DeInit(&hi2c4) != HAL_OK) {
			ret = BSP_ERROR_BUS_FAILURE;
		}
	}

	return ret;
}

/**
 * @brief  Write a 8bit value in a register of the device through BUS.
 * @param  DevAddr Device address on Bus.
 * @param  Reg    The target register address to write
 * @param  pData  The target register value to be written
 * @param  Length buffer size to be written
 * @retval BSP status
 */
int32_t BSP_I2C4_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData,
		uint16_t Length)
{
	int32_t ret;
#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif
	if (I2C4_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length) == 0) {
		ret = BSP_ERROR_NONE;
	} else {
		if (HAL_I2C_GetError(&hi2c4) == HAL_I2C_ERROR_AF) {
			ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
		} else {
			ret = BSP_ERROR_PERIPH_FAILURE;
		}
	}
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif
	return ret;
}

/**
 * @brief  Read a 8bit register of the device through BUS
 * @param  DevAddr Device address on BUS
 * @param  Reg     The target register address to read
 * @param  pData   Pointer to data buffer
 * @param  Length  Length of the data
 * @retval BSP status
 */
int32_t BSP_I2C4_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData,
		uint16_t Length)
{
	int32_t ret;
#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif
	if (I2C4_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_8BIT, pData, Length) == 0) {
		ret = BSP_ERROR_NONE;
	} else {
		if (HAL_I2C_GetError(&hi2c4) == HAL_I2C_ERROR_AF) {
			ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
		} else {
			ret = BSP_ERROR_PERIPH_FAILURE;
		}
	}
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif
	return ret;
}

/**
 * @brief  Write a 16bit value in a register of the device through BUS.
 * @param  DevAddr Device address on Bus.
 * @param  Reg    The target register address to write
 * @param  pData  The target register value to be written
 * @param  Length buffer size to be written
 * @retval BSP status
 */
int32_t BSP_I2C4_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData,
		uint16_t Length)
{
	int32_t ret;
#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif
	if (I2C4_WriteReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length) == 0) {
		ret = BSP_ERROR_NONE;
	} else {
		if (HAL_I2C_GetError(&hi2c4) == HAL_I2C_ERROR_AF) {
			ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
		} else {
			ret = BSP_ERROR_PERIPH_FAILURE;
		}
	}
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif
	return ret;
}

/**
 * @brief  Read a 16bit register of the device through BUS
 * @param  DevAddr Device address on BUS
 * @param  Reg     The target register address to read
 * @param  pData   Pointer to data buffer
 * @param  Length  Length of the data
 * @retval BSP status
 */
int32_t BSP_I2C4_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData,
		uint16_t Length)
{
	int32_t ret;
#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif
	if (I2C4_ReadReg(DevAddr, Reg, I2C_MEMADD_SIZE_16BIT, pData, Length) == 0) {
		ret = BSP_ERROR_NONE;
	} else {
		if (HAL_I2C_GetError(&hi2c4) == HAL_I2C_ERROR_AF) {
			ret = BSP_ERROR_BUS_ACKNOWLEDGE_FAILURE;
		} else {
			ret = BSP_ERROR_PERIPH_FAILURE;
		}
	}
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif
	return ret;
}

/**
 * @brief  Checks if target device is ready for communication.
 * @note   This function is used with Memory devices
 * @param  DevAddr  Target device address
 * @param  Trials      Number of trials
 * @retval BSP status
 */
int32_t BSP_I2C4_IsReady(uint16_t DevAddr, uint32_t Trials)
{
	int32_t ret = BSP_ERROR_NONE;
#if defined(BSP_USE_CMSIS_OS)
  /* Get semaphore to prevent multiple I2C access */
  osSemaphoreWait(BspI2cSemaphore, osWaitForever);
#endif
	if (HAL_I2C_IsDeviceReady(&hi2c4, DevAddr, Trials, 1000) != HAL_OK) {
		ret = BSP_ERROR_BUSY;
	}
#if defined(BSP_USE_CMSIS_OS)
  /* Release semaphore to prevent multiple I2C access */
  osSemaphoreRelease(BspI2cSemaphore);
#endif
	return ret;
}

/**
 * @brief  Delay function
 * @retval Tick value
 */
int32_t BSP_GetTick(void)
{
	return (int32_t) HAL_GetTick();
}

/**
 * @brief  Compute I2C timing according current I2C clock source and required I2C clock.
 * @param  clock_src_freq I2C clock source in Hz.
 * @param  i2c_freq Required I2C clock in Hz.
 * @retval I2C timing or 0 in case of error.
 */
static uint32_t I2C_GetTiming(uint32_t clock_src_freq, uint32_t i2c_freq)
{
	uint32_t ret = 0;
	uint32_t speed;
	uint32_t idx;

	if ((clock_src_freq != 0U) && (i2c_freq != 0U)) {
		for (speed = 0; speed <= (uint32_t) I2C_SPEED_FREQ_FAST_PLUS; speed++) {
			if ((i2c_freq >= I2C_Charac[speed].freq_min)
					&& (i2c_freq <= I2C_Charac[speed].freq_max)) {
				I2C_Compute_PRESC_SCLDEL_SDADEL(clock_src_freq, speed);
				idx = I2C_Compute_SCLL_SCLH(clock_src_freq, speed);

				if (idx < I2C_VALID_TIMING_NBR) {
					ret = ((I2c_valid_timing[idx].presc & 0x0FU) << 28)
							|\
 ((I2c_valid_timing[idx].tscldel & 0x0FU) << 20)
							|\
 ((I2c_valid_timing[idx].tsdadel & 0x0FU) << 16)
							|\
 ((I2c_valid_timing[idx].sclh & 0xFFU) << 8)
							|\
 ((I2c_valid_timing[idx].scll & 0xFFU) << 0);
				}
				break;
			}
		}
	}

	return ret;
}

/**
 * @brief  Compute PRESC, SCLDEL and SDADEL.
 * @param  clock_src_freq I2C source clock in HZ.
 * @param  I2C_speed I2C frequency (index).
 * @retval None.
 */
static void I2C_Compute_PRESC_SCLDEL_SDADEL(uint32_t clock_src_freq,
		uint32_t I2C_speed)
{
	uint32_t prev_presc = I2C_PRESC_MAX;
	uint32_t ti2cclk;
	int32_t tsdadel_min, tsdadel_max;
	int32_t tscldel_min;
	uint32_t presc, scldel, sdadel;
	uint32_t tafdel_min, tafdel_max;

	ti2cclk = (SEC2NSEC + (clock_src_freq / 2U)) / clock_src_freq;

	tafdel_min = I2C_ANALOG_FILTER_DELAY_MIN;
	tafdel_max = I2C_ANALOG_FILTER_DELAY_MAX;

	/* tDNF = DNF x tI2CCLK
	 tPRESC = (PRESC+1) x tI2CCLK
	 SDADEL >= {tf +tHD;DAT(min) - tAF(min) - tDNF - [3 x tI2CCLK]} / {tPRESC}
	 SDADEL <= {tVD;DAT(max) - tr - tAF(max) - tDNF- [4 x tI2CCLK]} / {tPRESC} */

	tsdadel_min =
			(int32_t) I2C_Charac[I2C_speed].tfall
					+ (int32_t) I2C_Charac[I2C_speed].hddat_min - (int32_t) tafdel_min
					- (int32_t) (((int32_t) I2C_Charac[I2C_speed].dnf + 3)
							* (int32_t) ti2cclk);

	tsdadel_max =
			(int32_t) I2C_Charac[I2C_speed].vddat_max
					- (int32_t) I2C_Charac[I2C_speed].trise - (int32_t) tafdel_max
					- (int32_t) (((int32_t) I2C_Charac[I2C_speed].dnf + 4)
							* (int32_t) ti2cclk);

	/* {[tr+ tSU;DAT(min)] / [tPRESC]} - 1 <= SCLDEL */
	tscldel_min = (int32_t) I2C_Charac[I2C_speed].trise
			+ (int32_t) I2C_Charac[I2C_speed].sudat_min;

	if (tsdadel_min <= 0) {
		tsdadel_min = 0;
	}

	if (tsdadel_max <= 0) {
		tsdadel_max = 0;
	}

	for (presc = 0; presc < I2C_PRESC_MAX; presc++) {
		for (scldel = 0; scldel < I2C_SCLDEL_MAX; scldel++) {
			/* TSCLDEL = (SCLDEL+1) * (PRESC+1) * TI2CCLK */
			uint32_t tscldel = (scldel + 1U) * (presc + 1U) * ti2cclk;

			if (tscldel >= (uint32_t) tscldel_min) {
				for (sdadel = 0; sdadel < I2C_SDADEL_MAX; sdadel++) {
					/* TSDADEL = SDADEL * (PRESC+1) * TI2CCLK */
					uint32_t tsdadel = (sdadel * (presc + 1U)) * ti2cclk;

					if ((tsdadel >= (uint32_t) tsdadel_min)
							&& (tsdadel <= (uint32_t) tsdadel_max)) {
						if (presc != prev_presc) {
							I2c_valid_timing[I2c_valid_timing_nbr].presc = presc;
							I2c_valid_timing[I2c_valid_timing_nbr].tscldel = scldel;
							I2c_valid_timing[I2c_valid_timing_nbr].tsdadel = sdadel;
							prev_presc = presc;
							I2c_valid_timing_nbr++;

							if (I2c_valid_timing_nbr >= I2C_VALID_TIMING_NBR) {
								return;
							}
						}
					}
				}
			}
		}
	}
}

/**
 * @brief  Calculate SCLL and SCLH and find best configuration.
 * @param  clock_src_freq I2C source clock in HZ.
 * @param  I2C_speed I2C frequency (index).
 * @retval config index (0 to I2C_VALID_TIMING_NBR], 0xFFFFFFFF for no valid config.
 */
static uint32_t I2C_Compute_SCLL_SCLH(uint32_t clock_src_freq,
		uint32_t I2C_speed)
{
	uint32_t ret = 0xFFFFFFFFU;
	uint32_t ti2cclk;
	uint32_t ti2cspeed;
	uint32_t prev_error;
	uint32_t dnf_delay;
	uint32_t clk_min, clk_max;
	uint32_t scll, sclh;
	uint32_t tafdel_min;

	ti2cclk = (SEC2NSEC + (clock_src_freq / 2U)) / clock_src_freq;
	ti2cspeed = (SEC2NSEC + (I2C_Charac[I2C_speed].freq / 2U))
			/ I2C_Charac[I2C_speed].freq;

	tafdel_min = I2C_ANALOG_FILTER_DELAY_MIN;

	/* tDNF = DNF x tI2CCLK */
	dnf_delay = I2C_Charac[I2C_speed].dnf * ti2cclk;

	clk_max = SEC2NSEC / I2C_Charac[I2C_speed].freq_min;
	clk_min = SEC2NSEC / I2C_Charac[I2C_speed].freq_max;

	prev_error = ti2cspeed;

	for (uint32_t count = 0; count < I2c_valid_timing_nbr; count++) {
		/* tPRESC = (PRESC+1) x tI2CCLK*/
		uint32_t tpresc = (I2c_valid_timing[count].presc + 1U) * ti2cclk;

		for (scll = 0; scll < I2C_SCLL_MAX; scll++) {
			/* tLOW(min) <= tAF(min) + tDNF + 2 x tI2CCLK + [(SCLL+1) x tPRESC ] */
			uint32_t tscl_l = tafdel_min + dnf_delay + (2U * ti2cclk)
					+ ((scll + 1U) * tpresc);

			/* The I2CCLK period tI2CCLK must respect the following conditions:
			 tI2CCLK < (tLOW - tfilters) / 4 and tI2CCLK < tHIGH */
			if ((tscl_l > I2C_Charac[I2C_speed].lscl_min)
					&& (ti2cclk < ((tscl_l - tafdel_min - dnf_delay) / 4U))) {
				for (sclh = 0; sclh < I2C_SCLH_MAX; sclh++) {
					/* tHIGH(min) <= tAF(min) + tDNF + 2 x tI2CCLK + [(SCLH+1) x tPRESC] */
					uint32_t tscl_h = tafdel_min + dnf_delay + (2U * ti2cclk)
							+ ((sclh + 1U) * tpresc);

					/* tSCL = tf + tLOW + tr + tHIGH */
					uint32_t tscl = tscl_l + tscl_h + I2C_Charac[I2C_speed].trise
							+ I2C_Charac[I2C_speed].tfall;

					if ((tscl >= clk_min) && (tscl <= clk_max)
							&& (tscl_h >= I2C_Charac[I2C_speed].hscl_min)
							&& (ti2cclk < tscl_h)) {
						int32_t error = (int32_t) tscl - (int32_t) ti2cspeed;

						if (error < 0) {
							error = -error;
						}

						/* look for the timings with the lowest clock error */
						if ((uint32_t) error < prev_error) {
							prev_error = (uint32_t) error;
							I2c_valid_timing[count].scll = scll;
							I2c_valid_timing[count].sclh = sclh;
							ret = count;
						}
					}
				}
			}
		}
	}

	return ret;
}

/**
 * @brief  Write a value in a register of the device through BUS.
 * @param  DevAddr    Device address on Bus.
 * @param  MemAddSize Size of internal memory address
 * @param  Reg        The target register address to write
 * @param  pData      The target register value to be written
 * @param  Length     data length in bytes
 * @retval BSP status
 */
static int32_t I2C4_WriteReg(uint16_t DevAddr, uint16_t Reg,
		uint16_t MemAddSize, uint8_t *pData, uint16_t Length)
{
	if (HAL_I2C_Mem_Write(&hi2c4, DevAddr, Reg, MemAddSize, pData, Length,
			1000) == HAL_OK) {
		return BSP_ERROR_NONE;
	}

	return BSP_ERROR_BUS_FAILURE;
}

/**
 * @brief  Read a register of the device through BUS
 * @param  DevAddr    Device address on BUS
 * @param  MemAddSize Size of internal memory address
 * @param  Reg        The target register address to read
 * @param  pData      The target register value to be read
 * @param  Length     data length in bytes
 * @retval BSP status
 */
static int32_t I2C4_ReadReg(uint16_t DevAddr, uint16_t Reg, uint16_t MemAddSize,
		uint8_t *pData, uint16_t Length)
{
	if (HAL_I2C_Mem_Read(&hi2c4, DevAddr, Reg, MemAddSize, pData, Length,
			1000) == HAL_OK) {
		return BSP_ERROR_NONE;
	}

	return BSP_ERROR_BUS_FAILURE;
}
