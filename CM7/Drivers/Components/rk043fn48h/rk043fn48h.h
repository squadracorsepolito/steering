#ifndef __RK043FN48H_H
#define __RK043FN48H_H

#define LCD_PIXEL_FORMAT_ARGB8888 0x00000000U /* ARGB8888 LTDC pixel format */
#define LCD_PIXEL_FORMAT_RGB888 0x00000001U   /* RGB888 LTDC pixel format   */
#define LCD_PIXEL_FORMAT_RGB565 0x00000002U   /* RGB565 LTDC pixel format   */
#define LCD_PIXEL_FORMAT_ARGB1555 0x00000003U /* ARGB1555 LTDC pixel format */
#define LCD_PIXEL_FORMAT_ARGB4444 0x00000004U /* ARGB4444 LTDC pixel format */
#define LCD_PIXEL_FORMAT_L8 0x00000005U       /* L8 LTDC pixel format       */
#define LCD_PIXEL_FORMAT_AL44 0x00000006U     /* AL44 LTDC pixel format     */
#define LCD_PIXEL_FORMAT_AL88 0x00000007U     /* AL88 LTDC pixel format     */

#define RK043FN48H_WIDTH ((uint16_t)480)  /* LCD PIXEL WIDTH            */
#define RK043FN48H_HEIGHT ((uint16_t)272) /* LCD PIXEL HEIGHT           */
#define RK043FN48H_HSYNC ((uint16_t)41)   /* Horizontal synchronization */
#define RK043FN48H_HBP ((uint16_t)13)     /* Horizontal back porch      */
#define RK043FN48H_HFP ((uint16_t)32)     /* Horizontal front porch     */
#define RK043FN48H_VSYNC ((uint16_t)10)   /* Vertical synchronization   */
#define RK043FN48H_VBP ((uint16_t)2)      /* Vertical back porch        */
#define RK043FN48H_VFP ((uint16_t)2)      /* Vertical front porch       */

#define RK043FN48H_FREQUENCY_DIVIDER 5 /* LCD Frequency divider      */

#endif
