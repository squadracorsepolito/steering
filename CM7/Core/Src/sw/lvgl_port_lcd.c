#include "sw/lvgl_port_lcd.h"
#include "driver/lcd.h"
#include "lvgl/lvgl.h"
#include <stdlib.h>

#define LVGL_BUFFER_ADDR_AT_SDRAM (0xD007F810)
#define LVGL_BUFFER_2_ADDR_AT_SDRAM (0xD00FF020)
static void disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p);
static void disp_clean_dcache(lv_disp_drv_t *drv);
static uint8_t CopyImageToLcdFrameBuffer(void *pSrc, void *pDst, uint32_t xSize, uint32_t ySize);

static lv_disp_t *display = NULL;
static lv_disp_drv_t disp_drv;
static lv_disp_draw_buf_t disp_buf;

void LCD_Init()
{
  /* There is only one display on STM32 */
  if (display != NULL)
    abort();

  /* Initialize the LCD */
  BSP_LCD_Init(LCD_ORIENTATION_LANDSCAPE);

  BSP_LCD_SetBrightness(100);
  BSP_LCD_DisplayOn();

  lv_disp_draw_buf_init(&disp_buf, (void *)LVGL_BUFFER_ADDR_AT_SDRAM, (void *)LVGL_BUFFER_2_ADDR_AT_SDRAM,
                        Lcd_Ctx.XSize * Lcd_Ctx.YSize); /*Initialize the display buffer*/

  lv_disp_drv_init(&disp_drv);

  /*Set up the functions to access to your display*/

  /*Set the resolution of the display*/
  disp_drv.hor_res = Lcd_Ctx.XSize;
  disp_drv.ver_res = Lcd_Ctx.YSize;

  /*Used to copy the buffer's content to the display*/
  disp_drv.flush_cb = disp_flush;
  disp_drv.clean_dcache_cb = disp_clean_dcache;

  /*Set a display buffer*/
  disp_drv.draw_buf = &disp_buf;

  /*Finally register the driver*/
  display = lv_disp_drv_register(&disp_drv);
}

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished*/
static void disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
  /*Return if the area is out the screen*/
  if (area->x2 < 0)
    return;
  if (area->y2 < 0)
    return;
  if (area->x1 > Lcd_Ctx.XSize - 1)
    return;
  if (area->y1 > Lcd_Ctx.YSize - 1)
    return;
  // BSP_LED_Toggle(LED2);
  SCB_CleanInvalidateDCache();
  SCB_InvalidateICache();

  uint32_t address =
      hltdc.LayerCfg[Lcd_Ctx.ActiveLayer].FBStartAdress + (((Lcd_Ctx.XSize * area->y1) + area->x1) * Lcd_Ctx.BppFactor);

  CopyImageToLcdFrameBuffer((void *)color_p, (void *)address, lv_area_get_width(area), lv_area_get_height(area));

  lv_disp_flush_ready(&disp_drv);
  return;
}

static void disp_clean_dcache(lv_disp_drv_t *drv)
{
  SCB_CleanInvalidateDCache();
}

/**
 * @brief  Copy to LCD frame buffer area centered in WVGA resolution.
 * The area of copy is of size in ARGB8888.
 * @param  pSrc: Pointer to source buffer : source image buffer start here
 * @param  pDst: Pointer to destination buffer LCD frame buffer center area start here
 * @param  xSize: Buffer width
 * @param  ySize: Buffer height
 * @retval LCD Status : BSP_ERROR_NONE or BSP_ERROR_BUS_DMA_FAILURE
 */
static uint8_t CopyImageToLcdFrameBuffer(void *pSrc, void *pDst, uint32_t xSize, uint32_t ySize)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  uint8_t lcd_status;

  /* Configure the DMA2D Mode, Color Mode and output offset */
  hdma2d.Init.Mode = DMA2D_M2M_PFC;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;   /* Output color out of PFC */
  hdma2d.Init.AlphaInverted = DMA2D_REGULAR_ALPHA; /* No Output Alpha Inversion*/
  hdma2d.Init.RedBlueSwap = DMA2D_RB_REGULAR;      /* No Output Red & Blue swap */

  /* Output offset in pixels == nb of pixels to be added at end of line to come to the  */
  /* first pixel of the next line : on the output side of the DMA2D computation         */
  hdma2d.Init.OutputOffset = LCD_DEFAULT_WIDTH - xSize;

  hdma2d.Instance = DMA2D;

  /* DMA2D Initialization */
  if (HAL_DMA2D_Init(&hdma2d) == HAL_OK)
  {
    if (HAL_DMA2D_Start(&hdma2d, (uint32_t)pSrc, (uint32_t)pDst, xSize, ySize) == HAL_OK)
    {
      /* Polling For DMA transfer */
      hal_status = HAL_DMA2D_PollForTransfer(&hdma2d, 20);
      if (hal_status == HAL_OK)
      {
        /* return good status on exit */
        lcd_status = BSP_ERROR_NONE;
      }
      else
      {
        lcd_status = BSP_ERROR_BUS_DMA_FAILURE;
      }
    }
  }

  return (lcd_status);
}
