#include "lcd.h"
#include "ts.h"
#include "bus.h"
#include "sdram.h"

static TIM_HandleTypeDef hlcd_tim;

const LCD_UTILS_Drv_t LCD_Driver =
{ BSP_LCD_DrawBitmap, BSP_LCD_FillRGBRect, BSP_LCD_DrawHLine, BSP_LCD_DrawVLine,
		BSP_LCD_FillRect, BSP_LCD_ReadPixel, BSP_LCD_WritePixel, BSP_LCD_GetXSize,
		BSP_LCD_GetYSize, BSP_LCD_SetActiveLayer, BSP_LCD_GetPixelFormat };

BSP_LCD_Ctx_t Lcd_Ctx[LCD_INSTANCES_NBR];

#define CONVERTRGB5652ARGB8888(Color)((((((((Color) >> (11U)) & 0x1FU) * 527U) + 23U) >> (6U)) << (16U)) |\
                                     (((((((Color) >> (5U)) & 0x3FU) * 259U) + 33U) >> (6U)) << (8U)) |\
                                     (((((Color) & 0x1FU) * 527U) + 23U) >> (6U)) | (0xFF000000U))


static void LL_FillBuffer(uint32_t Instance, uint32_t *pDst, uint32_t xSize,
		uint32_t ySize, uint32_t OffLine, uint32_t Color);
static void LL_ConvertLineToRGB(uint32_t Instance, uint32_t *pSrc,
		uint32_t *pDst, uint32_t xSize, uint32_t ColorMode);

/**
 * @brief  Initializes the LCD in default mode.
 * @param  Instance    LCD Instance
 * @param  Orientation LCD_ORIENTATION_PORTRAIT or LCD_ORIENTATION_LANDSCAPE
 * @retval BSP status
 */
int32_t BSP_LCD_Init(uint32_t Instance, uint32_t Orientation)
{
	return BSP_LCD_InitEx(Instance, Orientation, LTDC_PIXEL_FORMAT_ARGB8888,
	LCD_DEFAULT_WIDTH, LCD_DEFAULT_HEIGHT);
}

/**
 * @brief  Initializes the LCD.
 * @param  Instance    LCD Instance
 * @param  Orientation LCD_ORIENTATION_PORTRAIT or LCD_ORIENTATION_LANDSCAPE
 * @param  PixelFormat LCD_PIXEL_FORMAT_RGB565 or LCD_PIXEL_FORMAT_RGB888
 * @param  Width       Display width
 * @param  Height      Display height
 * @retval BSP status
 */
int32_t BSP_LCD_InitEx(uint32_t Instance, uint32_t Orientation,
		uint32_t PixelFormat, uint32_t Width, uint32_t Height)
{
	int32_t ret = BSP_ERROR_NONE;
	uint32_t ltdc_pixel_format;
	FT5336_Object_t ts_comp_obj;
	FT5336_IO_t io_comp_ctx;

	if ((Orientation > LCD_ORIENTATION_LANDSCAPE)
			|| (Instance >= LCD_INSTANCES_NBR)
			|| ((PixelFormat != LCD_PIXEL_FORMAT_RGB565)
					&& (PixelFormat != LTDC_PIXEL_FORMAT_ARGB8888))) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		if (PixelFormat == LCD_PIXEL_FORMAT_RGB565) {
			ltdc_pixel_format = LTDC_PIXEL_FORMAT_RGB565;
			Lcd_Ctx[Instance].BppFactor = 2U;
		} else /* LCD_PIXEL_FORMAT_RGB888 */
		{
			ltdc_pixel_format = LTDC_PIXEL_FORMAT_ARGB8888;
			Lcd_Ctx[Instance].BppFactor = 4U;
		}

		/* Store pixel format, xsize and ysize information */
		Lcd_Ctx[Instance].PixelFormat = PixelFormat;
		Lcd_Ctx[Instance].XSize = Width;
		Lcd_Ctx[Instance].YSize = Height;

		io_comp_ctx.Init = BSP_I2C4_Init;
		io_comp_ctx.ReadReg = BSP_I2C4_ReadReg;
		io_comp_ctx.Address = TS_I2C_ADDRESS;
		if (FT5336_RegisterBusIO(&ts_comp_obj, &io_comp_ctx) < 0) {
			ret = BSP_ERROR_COMPONENT_FAILURE;
		}

		if (ret == BSP_ERROR_NONE) {
			/* Before configuring LTDC layer, ensure SDRAM is initialized */
#if !defined(DATA_IN_ExtSDRAM)
			/* Initialize the SDRAM */
			if (BSP_SDRAM_Init(0) != BSP_ERROR_NONE) {
				return BSP_ERROR_PERIPH_FAILURE;
			}
#endif /* DATA_IN_ExtSDRAM */

			/* Configure default LTDC Layer 0. This configuration can be override by calling
			 BSP_LCD_ConfigLayer() at application level */

			/* By default the reload is activated and executed immediately */
			Lcd_Ctx[Instance].ReloadEnable = 1U;
		}
	}

	return ret;
}

/**
 * @brief  De-Initializes the LCD resources.
 * @param  Instance    LCD Instance
 * @retval BSP status
 */
int32_t BSP_LCD_DeInit(uint32_t Instance)
{
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		(void) HAL_LTDC_DeInit(&hltdc);
		if (HAL_DMA2D_DeInit(&hdma2d) != HAL_OK) {
			ret = BSP_ERROR_PERIPH_FAILURE;
		} else {
			/* DeInit TIM PWM */
			HAL_TIM_PWM_DeInit(&hlcd_tim);

			Lcd_Ctx[Instance].IsMspCallbacksValid = 0;
		}
	}

	return ret;
}

/**
 * @brief  Gets the LCD Active LCD Pixel Format.
 * @param  Instance    LCD Instance
 * @param  PixelFormat Active LCD Pixel Format
 * @retval BSP status
 */
int32_t BSP_LCD_GetPixelFormat(uint32_t Instance, uint32_t *PixelFormat)
{
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		/* Only RGB565 format is supported */
		*PixelFormat = Lcd_Ctx[Instance].PixelFormat;
	}

	return ret;
}

/**
 * @brief  Set the LCD Active Layer.
 * @param  Instance    LCD Instance
 * @param  LayerIndex  LCD layer index
 * @retval BSP status
 */
int32_t BSP_LCD_SetActiveLayer(uint32_t Instance, uint32_t LayerIndex)
{
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		Lcd_Ctx[Instance].ActiveLayer = LayerIndex;
	}

	return ret;
}

/**
 * @brief  Control the LTDC reload
 * @param  Instance    LCD Instance
 * @param  ReloadType can be one of the following values
 *         - BSP_LCD_RELOAD_NONE
 *         - BSP_LCD_RELOAD_IMMEDIATE
 *         - BSP_LCD_RELOAD_VERTICAL_BLANKING
 * @retval BSP status
 */
int32_t BSP_LCD_Relaod(uint32_t Instance, uint32_t ReloadType)
{
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else if (ReloadType == BSP_LCD_RELOAD_NONE) {
		Lcd_Ctx[Instance].ReloadEnable = 0U;
	} else if (HAL_LTDC_Reload(&hltdc, ReloadType) != HAL_OK) {
		ret = BSP_ERROR_PERIPH_FAILURE;
	} else {
		Lcd_Ctx[Instance].ReloadEnable = 1U;
	}

	return ret;
}

/**
 * @brief  Sets an LCD Layer visible
 * @param  Instance    LCD Instance
 * @param  LayerIndex  Visible Layer
 * @param  State  New state of the specified layer
 *          This parameter can be one of the following values:
 *            @arg  ENABLE
 *            @arg  DISABLE
 * @retval BSP status
 */
int32_t BSP_LCD_SetLayerVisible(uint32_t Instance, uint32_t LayerIndex,
		FunctionalState State)
{
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		if (State == ENABLE) {
			__HAL_LTDC_LAYER_ENABLE(&hltdc, LayerIndex);
		} else {
			__HAL_LTDC_LAYER_DISABLE(&hltdc, LayerIndex);
		}

		if (Lcd_Ctx[Instance].ReloadEnable == 1U) {
			__HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&hltdc);
		}
	}

	return ret;
}

/**
 * @brief  Configures the transparency.
 * @param  Instance      LCD Instance
 * @param  LayerIndex    Layer foreground or background.
 * @param  Transparency  Transparency
 *           This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF
 * @retval BSP status
 */
int32_t BSP_LCD_SetTransparency(uint32_t Instance, uint32_t LayerIndex,
		uint8_t Transparency)
{
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		if (Lcd_Ctx[Instance].ReloadEnable == 1U) {
			(void) HAL_LTDC_SetAlpha(&hltdc, Transparency, LayerIndex);
		} else {
			(void) HAL_LTDC_SetAlpha_NoReload(&hltdc, Transparency, LayerIndex);
		}
	}

	return ret;
}

/**
 * @brief  Sets an LCD layer frame buffer address.
 * @param  Instance    LCD Instance
 * @param  LayerIndex  Layer foreground or background
 * @param  Address     New LCD frame buffer value
 * @retval BSP status
 */
int32_t BSP_LCD_SetLayerAddress(uint32_t Instance, uint32_t LayerIndex,
		uint32_t Address)
{
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		if (Lcd_Ctx[Instance].ReloadEnable == 1U) {
			(void) HAL_LTDC_SetAddress(&hltdc, Address, LayerIndex);
		} else {
			(void) HAL_LTDC_SetAddress_NoReload(&hltdc, Address, LayerIndex);
		}
	}

	return ret;
}

/**
 * @brief  Sets display window.
 * @param  Instance    LCD Instance
 * @param  LayerIndex  Layer index
 * @param  Xpos   LCD X position
 * @param  Ypos   LCD Y position
 * @param  Width  LCD window width
 * @param  Height LCD window height
 * @retval BSP status
 */
int32_t BSP_LCD_SetLayerWindow(uint32_t Instance, uint16_t LayerIndex,
		uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		if (Lcd_Ctx[Instance].ReloadEnable == 1U) {
			/* Reconfigure the layer size  and position */
			(void) HAL_LTDC_SetWindowSize(&hltdc, Width, Height, LayerIndex);
			(void) HAL_LTDC_SetWindowPosition(&hltdc, Xpos, Ypos, LayerIndex);
		} else {
			/* Reconfigure the layer size and position */
			(void) HAL_LTDC_SetWindowSize_NoReload(&hltdc, Width, Height,
					LayerIndex);
			(void) HAL_LTDC_SetWindowPosition_NoReload(&hltdc, Xpos, Ypos,
					LayerIndex);
		}

		Lcd_Ctx[Instance].XSize = Width;
		Lcd_Ctx[Instance].YSize = Height;
	}

	return ret;
}

/**
 * @brief  Configures and sets the color keying.
 * @param  Instance    LCD Instance
 * @param  LayerIndex  Layer foreground or background
 * @param  Color       Color reference
 * @retval BSP status
 */
int32_t BSP_LCD_SetColorKeying(uint32_t Instance, uint32_t LayerIndex,
		uint32_t Color)
{
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		if (Lcd_Ctx[Instance].ReloadEnable == 1U) {
			/* Configure and Enable the color Keying for LCD Layer */
			(void) HAL_LTDC_ConfigColorKeying(&hltdc, Color, LayerIndex);
			(void) HAL_LTDC_EnableColorKeying(&hltdc, LayerIndex);
		} else {
			/* Configure and Enable the color Keying for LCD Layer */
			(void) HAL_LTDC_ConfigColorKeying_NoReload(&hltdc, Color, LayerIndex);
			(void) HAL_LTDC_EnableColorKeying_NoReload(&hltdc, LayerIndex);
		}
	}

	return ret;
}

/**
 * @brief  Disables the color keying.
 * @param  Instance    LCD Instance
 * @param  LayerIndex Layer foreground or background
 * @retval BSP status
 */
int32_t BSP_LCD_ResetColorKeying(uint32_t Instance, uint32_t LayerIndex)
{
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		if (Lcd_Ctx[Instance].ReloadEnable == 1U) {
			/* Disable the color Keying for LCD Layer */
			(void) HAL_LTDC_DisableColorKeying(&hltdc, LayerIndex);
		} else {
			/* Disable the color Keying for LCD Layer */
			(void) HAL_LTDC_DisableColorKeying_NoReload(&hltdc, LayerIndex);
		}
	}

	return ret;
}

/**
 * @brief  Gets the LCD X size.
 * @param  Instance  LCD Instance
 * @param  XSize     LCD width
 * @retval BSP status
 */
int32_t BSP_LCD_GetXSize(uint32_t Instance, uint32_t *XSize)
{
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		*XSize = Lcd_Ctx[Instance].XSize;
	}

	return ret;
}

/**
 * @brief  Gets the LCD Y size.
 * @param  Instance  LCD Instance
 * @param  YSize     LCD Height
 * @retval BSP status
 */
int32_t BSP_LCD_GetYSize(uint32_t Instance, uint32_t *YSize)
{
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		*YSize = Lcd_Ctx[Instance].YSize;
	}

	return ret;
}

/**
 * @brief  Switch On the display.
 * @param  Instance    LCD Instance
 * @retval BSP status
 */
int32_t BSP_LCD_DisplayOn(uint32_t Instance)
{
	GPIO_InitTypeDef gpio_init_structure;
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		/* Assert LCD_DISP_EN pin */
		HAL_GPIO_WritePin(LCD_DISP_EN_GPIO_Port, LCD_DISP_EN_Pin, GPIO_PIN_SET);

		/* re-connect the BL pin to the brightness TIMER */
		gpio_init_structure.Mode = GPIO_MODE_AF_PP;
		gpio_init_structure.Pull = GPIO_NOPULL;
		gpio_init_structure.Speed = GPIO_SPEED_FREQ_MEDIUM;
		gpio_init_structure.Alternate = LCD_TIMx_CHANNEL_AF;
		gpio_init_structure.Pin = LCD_BL_CTRL_Pin; /* BL_CTRL */

		HAL_GPIO_Init(LCD_BL_CTRL_GPIO_Port, &gpio_init_structure);
	}

	return ret;
}

/**
 * @brief  Switch Off the display.
 * @param  Instance    LCD Instance
 * @retval BSP status
 */
int32_t BSP_LCD_DisplayOff(uint32_t Instance)
{
	GPIO_InitTypeDef gpio_init_structure;
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
		gpio_init_structure.Pull = GPIO_NOPULL;
		gpio_init_structure.Speed = GPIO_SPEED_FREQ_MEDIUM;
		gpio_init_structure.Pin = LCD_BL_CTRL_Pin; /* BL_CTRL */
		HAL_GPIO_Init(LCD_BL_CTRL_GPIO_Port, &gpio_init_structure);

		__HAL_LTDC_DISABLE(&hltdc);
		/* Assert LCD_DISP_EN pin */
		HAL_GPIO_WritePin(LCD_DISP_EN_GPIO_Port, LCD_DISP_EN_Pin, GPIO_PIN_RESET);
		/* Assert LCD_BL_CTRL pin */
		HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_Port, LCD_BL_CTRL_Pin, GPIO_PIN_RESET);
	}

	return ret;
}

/**
 * @brief  Set the brightness value
 * @param  Instance    LCD Instance
 * @param  Brightness [00: Min (black), 100 Max]
 * @retval BSP status
 */
int32_t BSP_LCD_SetBrightness(uint32_t Instance, uint32_t Brightness)
{
	int32_t ret = BSP_ERROR_NONE;
	/* Timer Configuration */
	TIM_OC_InitTypeDef LCD_TIM_Config;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		/* Stop PWM Timer channel */
		HAL_TIM_PWM_Stop(&hlcd_tim, LCD_TIMx_CHANNEL);

		/* Common configuration for all channels */
		LCD_TIM_Config.OCMode = TIM_OCMODE_PWM1;
		LCD_TIM_Config.OCPolarity = TIM_OCPOLARITY_HIGH;
		LCD_TIM_Config.OCFastMode = TIM_OCFAST_DISABLE;
		LCD_TIM_Config.OCNPolarity = TIM_OCNPOLARITY_HIGH;
		LCD_TIM_Config.OCNIdleState = TIM_OCNIDLESTATE_RESET;
		LCD_TIM_Config.OCIdleState = TIM_OCIDLESTATE_RESET;

		/* Set the pulse value for channel */
		LCD_TIM_Config.Pulse = (uint32_t) ((LCD_TIMX_PERIOD_VALUE * Brightness)
				/ 100);

		HAL_TIM_PWM_ConfigChannel(&hlcd_tim, &LCD_TIM_Config, LCD_TIMx_CHANNEL);

		/* Start PWM Timer channel */
		HAL_TIM_PWM_Start(&hlcd_tim, LCD_TIMx_CHANNEL);

		Lcd_Ctx[Instance].Brightness = Brightness;
	}

	return ret;
}

/**
 * @brief  Set the brightness value
 * @param  Instance    LCD Instance
 * @param  Brightness [00: Min (black), 100 Max]
 * @retval BSP status
 */
int32_t BSP_LCD_GetBrightness(uint32_t Instance, uint32_t *Brightness)
{
	int32_t ret = BSP_ERROR_NONE;

	if (Instance >= LCD_INSTANCES_NBR) {
		ret = BSP_ERROR_WRONG_PARAM;
	} else {
		*Brightness = Lcd_Ctx[Instance].Brightness;
	}

	return ret;
}

/**
 * @brief  Draws a bitmap picture loaded in the internal Flash in currently active layer.
 * @param  Instance LCD Instance
 * @param  Xpos Bmp X position in the LCD
 * @param  Ypos Bmp Y position in the LCD
 * @param  pBmp Pointer to Bmp picture address in the internal Flash.
 * @retval BSP status
 */
int32_t BSP_LCD_DrawBitmap(uint32_t Instance, uint32_t Xpos, uint32_t Ypos,
		uint8_t *pBmp)
{
	int32_t ret = BSP_ERROR_NONE;
	uint32_t index, width, height, bit_pixel;
	uint32_t Address;
	uint32_t input_color_mode;
	uint8_t *pbmp;

	/* Get bitmap data address offset */
	index = (uint32_t) pBmp[10] + ((uint32_t) pBmp[11] << 8)
			+ ((uint32_t) pBmp[12] << 16) + ((uint32_t) pBmp[13] << 24);

	/* Read bitmap width */
	width = (uint32_t) pBmp[18] + ((uint32_t) pBmp[19] << 8)
			+ ((uint32_t) pBmp[20] << 16) + ((uint32_t) pBmp[21] << 24);

	/* Read bitmap height */
	height = (uint32_t) pBmp[22] + ((uint32_t) pBmp[23] << 8)
			+ ((uint32_t) pBmp[24] << 16) + ((uint32_t) pBmp[25] << 24);

	/* Read bit/pixel */
	bit_pixel = (uint32_t) pBmp[28] + ((uint32_t) pBmp[29] << 8);

	/* Set the address */
	Address =
			hltdc.LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress
					+ (((Lcd_Ctx[Instance].XSize * Ypos) + Xpos)
							* Lcd_Ctx[Instance].BppFactor);

	/* Get the layer pixel format */
	if ((bit_pixel / 8U) == 4U) {
		input_color_mode = DMA2D_INPUT_ARGB8888;
	} else if ((bit_pixel / 8U) == 2U) {
		input_color_mode = DMA2D_INPUT_RGB565;
	} else {
		input_color_mode = DMA2D_INPUT_RGB888;
	}

	/* Bypass the bitmap header */
	pbmp = pBmp + (index + (width * (height - 1U) * (bit_pixel / 8U)));

	/* Convert picture to ARGB8888 pixel format */
	for (index = 0; index < height; index++) {
		/* Pixel format conversion */
		LL_ConvertLineToRGB(Instance, (uint32_t*) pbmp, (uint32_t*) Address, width,
				input_color_mode);

		/* Increment the source and destination buffers */
		Address += (Lcd_Ctx[Instance].XSize * Lcd_Ctx[Instance].BppFactor);
		pbmp -= width * (bit_pixel / 8U);
	}

	return ret;
}

/**
 * @brief  Draw a horizontal line on LCD.
 * @param  Instance LCD Instance.
 * @param  Xpos X position.
 * @param  Ypos Y position.
 * @param  pData Pointer to RGB line data
 * @param  Width Rectangle width.
 * @param  Height Rectangle Height.
 * @retval BSP status.
 */
int32_t BSP_LCD_FillRGBRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos,
		uint8_t *pData, uint32_t Width, uint32_t Height)
{
	uint32_t i;
	uint8_t *pdata = pData;

#if (USE_DMA2D_TO_FILL_RGB_RECT == 1)
  uint32_t  Xaddress;
  for(i = 0; i < Height; i++)
  {
    /* Get the line address */
    Xaddress = hltdc.LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress + (Lcd_Ctx[Instance].BppFactor*(((Lcd_Ctx[Instance].XSize + i)*Ypos) + Xpos));

    /* Write line */
    if(Lcd_Ctx[Instance].PixelFormat == LCD_PIXEL_FORMAT_RGB565)
    {
      LL_ConvertLineToRGB(Instance, (uint32_t *)pdata, (uint32_t *)Xaddress, Width, DMA2D_INPUT_RGB565);
    }
    else
    {
      LL_ConvertLineToRGB(Instance, (uint32_t *)pdata, (uint32_t *)Xaddress, Width, DMA2D_INPUT_ARGB8888);
    }
    pdata += Lcd_Ctx[Instance].BppFactor*Width;
  }
#else
	uint32_t color, j;
	for (i = 0; i < Height; i++) {
		for (j = 0; j < Width; j++) {
			color = (uint32_t) ((uint32_t) *pdata | ((uint32_t) (*(pdata + 1U)) << 8U)
					| ((uint32_t) (*(pdata + 2U)) << 16U)
					| ((uint32_t) (*(pdata + 3U)) << 24U));
			(void) BSP_LCD_WritePixel(Instance, Xpos + j, Ypos + i, color);
			pdata += Lcd_Ctx[Instance].BppFactor;
		}
	}
#endif

	return BSP_ERROR_NONE;
}

/**
 * @brief  Draws an horizontal line in currently active layer.
 * @param  Instance   LCD Instance
 * @param  Xpos  X position
 * @param  Ypos  Y position
 * @param  Length  Line length
 * @param  Color RGB color
 * @retval BSP status
 */
int32_t BSP_LCD_DrawHLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos,
		uint32_t Length, uint32_t Color)
{
	uint32_t Xaddress;

	/* Get the line address */
	Xaddress =
			hltdc.LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress
					+ (Lcd_Ctx[Instance].BppFactor
							* ((Lcd_Ctx[Instance].XSize * Ypos) + Xpos));

	/* Write line */
	if ((Xpos + Length) > Lcd_Ctx[Instance].XSize) {
		Length = Lcd_Ctx[Instance].XSize - Xpos;
	}
	LL_FillBuffer(Instance, (uint32_t*) Xaddress, Length, 1, 0, Color);

	return BSP_ERROR_NONE;
}

/**
 * @brief  Draws a vertical line in currently active layer.
 * @param  Instance   LCD Instance
 * @param  Xpos  X position
 * @param  Ypos  Y position
 * @param  Length  Line length
 * @param  Color RGB color
 * @retval BSP status
 */
int32_t BSP_LCD_DrawVLine(uint32_t Instance, uint32_t Xpos, uint32_t Ypos,
		uint32_t Length, uint32_t Color)
{
	uint32_t Xaddress;

	/* Get the line address */
	Xaddress =
			(hltdc.LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress)
					+ (Lcd_Ctx[Instance].BppFactor
							* ((Lcd_Ctx[Instance].XSize * Ypos) + Xpos));

	/* Write line */
	if ((Ypos + Length) > Lcd_Ctx[Instance].YSize) {
		Length = Lcd_Ctx[Instance].YSize - Ypos;
	}
	LL_FillBuffer(Instance, (uint32_t*) Xaddress, 1, Length,
			(Lcd_Ctx[Instance].XSize - 1U), Color);

	return BSP_ERROR_NONE;
}

/**
 * @brief  Draws a full rectangle in currently active layer.
 * @param  Instance   LCD Instance
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Width Rectangle width
 * @param  Height Rectangle height
 * @param  Color RGB color
 * @retval BSP status
 */
int32_t BSP_LCD_FillRect(uint32_t Instance, uint32_t Xpos, uint32_t Ypos,
		uint32_t Width, uint32_t Height, uint32_t Color)
{
	uint32_t Xaddress;

	/* Get the rectangle start address */
	Xaddress =
			(hltdc.LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress)
					+ (Lcd_Ctx[Instance].BppFactor
							* ((Lcd_Ctx[Instance].XSize * Ypos) + Xpos));

	/* Fill the rectangle */
	LL_FillBuffer(Instance, (uint32_t*) Xaddress, Width, Height,
			(Lcd_Ctx[Instance].XSize - Width), Color);

	return BSP_ERROR_NONE;
}

/**
 * @brief  Reads a LCD pixel.
 * @param  Instance    LCD Instance
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Color RGB pixel color
 * @retval BSP status
 */
int32_t BSP_LCD_ReadPixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos,
		uint32_t *Color)
{
	if (hltdc.LayerCfg[Lcd_Ctx[Instance].ActiveLayer].PixelFormat
			== LTDC_PIXEL_FORMAT_ARGB8888) {
		/* Read data value from SDRAM memory */
		*Color =
				*(__IO uint32_t*) (hltdc.LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress
						+ (4U * ((Ypos * Lcd_Ctx[Instance].XSize) + Xpos)));
	} else /* if((hltdc.LayerCfg[layer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565) */
	{
		/* Read data value from SDRAM memory */
		*Color =
				*(__IO uint16_t*) (hltdc.LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress
						+ (2U * ((Ypos * Lcd_Ctx[Instance].XSize) + Xpos)));
	}

	return BSP_ERROR_NONE;
}

/**
 * @brief  Draws a pixel on LCD.
 * @param  Instance    LCD Instance
 * @param  Xpos X position
 * @param  Ypos Y position
 * @param  Color Pixel color
 * @retval BSP status
 */
int32_t BSP_LCD_WritePixel(uint32_t Instance, uint32_t Xpos, uint32_t Ypos,
		uint32_t Color)
{
	if (hltdc.LayerCfg[Lcd_Ctx[Instance].ActiveLayer].PixelFormat
			== LTDC_PIXEL_FORMAT_ARGB8888) {
		/* Write data value to SDRAM memory */
		*(__IO uint32_t*) (hltdc.LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress
				+ (4U * ((Ypos * Lcd_Ctx[Instance].XSize) + Xpos))) = Color;
	} else {
		/* Write data value to SDRAM memory */
		*(__IO uint16_t*) (hltdc.LayerCfg[Lcd_Ctx[Instance].ActiveLayer].FBStartAdress
				+ (2U * ((Ypos * Lcd_Ctx[Instance].XSize) + Xpos))) = (uint16_t) Color;
	}

	return BSP_ERROR_NONE;
}

/**
 * @brief  Fills a buffer.
 * @param  Instance LCD Instance
 * @param  pDst Pointer to destination buffer
 * @param  xSize Buffer width
 * @param  ySize Buffer height
 * @param  OffLine Offset
 * @param  Color Color index
 */
static void LL_FillBuffer(uint32_t Instance, uint32_t *pDst, uint32_t xSize,
		uint32_t ySize, uint32_t OffLine, uint32_t Color)
{
	uint32_t output_color_mode, input_color = Color;

	switch (Lcd_Ctx[Instance].PixelFormat)
	{
	case LCD_PIXEL_FORMAT_RGB565:
		output_color_mode = DMA2D_OUTPUT_RGB565; /* RGB565 */
		input_color = CONVERTRGB5652ARGB8888(Color);
		break;
	case LCD_PIXEL_FORMAT_RGB888:
	default:
		output_color_mode = DMA2D_OUTPUT_ARGB8888; /* ARGB8888 */
		break;
	}

	/* Register to memory mode with ARGB8888 as color Mode */
	hdma2d.Init.Mode = DMA2D_R2M;
	hdma2d.Init.ColorMode = output_color_mode;
	hdma2d.Init.OutputOffset = OffLine;

	hdma2d.Instance = DMA2D;

	/* DMA2D Initialization */
	if (HAL_DMA2D_Init(&hdma2d) == HAL_OK) {
		if (HAL_DMA2D_Start(&hdma2d, input_color, (uint32_t) pDst, xSize, ySize)
				== HAL_OK) {
			/* Polling For DMA transfer */
			(void) HAL_DMA2D_PollForTransfer(&hdma2d, 50);
		}
	}
}

/**
 * @brief  Converts a line to an RGB pixel format.
 * @param  Instance LCD Instance
 * @param  pSrc Pointer to source buffer
 * @param  pDst Output color
 * @param  xSize Buffer width
 * @param  ColorMode Input color mode
 */
static void LL_ConvertLineToRGB(uint32_t Instance, uint32_t *pSrc,
		uint32_t *pDst, uint32_t xSize, uint32_t ColorMode)
{
	uint32_t output_color_mode;

	switch (Lcd_Ctx[Instance].PixelFormat)
	{
	case LCD_PIXEL_FORMAT_RGB565:
		output_color_mode = DMA2D_OUTPUT_RGB565; /* RGB565 */
		break;
	case LCD_PIXEL_FORMAT_RGB888:
	default:
		output_color_mode = DMA2D_OUTPUT_ARGB8888; /* ARGB8888 */
		break;
	}

	/* Configure the DMA2D Mode, Color Mode and output offset */
	hdma2d.Init.Mode = DMA2D_M2M_PFC;
	hdma2d.Init.ColorMode = output_color_mode;
	hdma2d.Init.OutputOffset = 0;

	/* Foreground Configuration */
	hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	hdma2d.LayerCfg[1].InputAlpha = 0xFF;
	hdma2d.LayerCfg[1].InputColorMode = ColorMode;
	hdma2d.LayerCfg[1].InputOffset = 0;

	hdma2d.Instance = DMA2D;

	/* DMA2D Initialization */
	if (HAL_DMA2D_Init(&hdma2d) == HAL_OK) {
		if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK) {
			if (HAL_DMA2D_Start(&hdma2d, (uint32_t) pSrc, (uint32_t) pDst, xSize,
					1) == HAL_OK) {
				/* Polling For DMA transfer */
				(void) HAL_DMA2D_PollForTransfer(&hdma2d, 50);
			}
		}
	}
}

