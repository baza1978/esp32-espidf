#include <esp_lcd_panel_rgb.h>
#include <esp_heap_caps.h>
#include "esp_lcd_panel_ops.h"
#include "lcd.h"
#include "lvgl.h"
#include "esp_err.h"
#include "driver/gpio.h"

static esp_lcd_panel_handle_t panel_handle = NULL;

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    //esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) disp->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;

    esp_lcd_panel_draw_bitmap( panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, px_map);
    lv_disp_flush_ready(disp);
}

esp_err_t LCDInit(void){


    //static lv_disp_drv_t disp_drv;

    gpio_config_t bk_gpio_config = {
            1ULL << LCD_PIN_BK_LIGHT,
            GPIO_MODE_OUTPUT,
            GPIO_PULLUP_DISABLE,
            GPIO_PULLDOWN_DISABLE,
            GPIO_INTR_DISABLE
    };

    esp_lcd_rgb_panel_config_t panel_config = {
            LCD_CLK_SRC_DEFAULT, // PLL_F160M
            {
                    LCD_PIXEL_CLOCK_HZ,
                    LCD_H_RES,
                    LCD_V_RES,
                    4,
                    8,
                    8,
                    4,
                    8,
                    8,
                    {
                            .hsync_idle_low   = false,
                            .vsync_idle_low   = false,
                            .de_idle_high     = false,
                            .pclk_active_neg  = true,
                            .pclk_idle_high   = false
                    }
            },
            16,
            0,
            0,
            0, //4 * LCD_H_RES,
            0,
            64,
            LCD_PIN_HSYNC,
            LCD_PIN_VSYNC,
            LCD_PIN_DE,
            LCD_PIN_PCLK,
            LCD_PIN_DISP_EN,
            {
                    LCD_PIN_DATA0,
                    LCD_PIN_DATA1,
                    LCD_PIN_DATA2,
                    LCD_PIN_DATA3,
                    LCD_PIN_DATA4,
                    LCD_PIN_DATA5,
                    LCD_PIN_DATA6,
                    LCD_PIN_DATA7,
                    LCD_PIN_DATA8,
                    LCD_PIN_DATA9,
                    LCD_PIN_DATA10,
                    LCD_PIN_DATA11,
                    LCD_PIN_DATA12,
                    LCD_PIN_DATA13,
                    LCD_PIN_DATA14,
                    LCD_PIN_DATA15
            },
            {
                            0,
                            0,
                            true,
                            false,
                            0,
                            0
                    }
    };

    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    void *buf1 = heap_caps_malloc(LCD_H_RES * LCD_V_RES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf1);

    lv_init();
    lv_disp_t * disp = lv_display_create(LCD_H_RES, LCD_V_RES);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_buffers(disp, buf1, NULL, LCD_H_RES * LCD_V_RES * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_FULL );

    return ESP_OK;
};