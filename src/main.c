#include <esp_lcd_panel_io.h>
#include "../managed_components/espressif__esp_lcd_touch_gt911/include/esp_lcd_touch_gt911.h"
#include "../managed_components/espressif__esp_lcd_touch/include/esp_lcd_touch.h"
#include "driver/spi_master.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#define CONFIG_LCD_HRES 800
#define CONFIG_LCD_VRES 480

#define TOUCH_PIN_RESET       (gpio_num_t)GPIO_NUM_38 // REST
#define TOUCH_PIN_SCL         (gpio_num_t)GPIO_NUM_20
#define TOUCH_PIN_SDA         (gpio_num_t)GPIO_NUM_19
#define TOUCH_PIN_INT         (gpio_num_t)GPIO_NUM_18
#define TOUCH_FREQ_HZ         (400000)

const i2c_port_t i2c_master_port = I2C_NUM_0;

esp_err_t InitI2C(void) {
    i2c_config_t conf = {
            .mode       = I2C_MODE_MASTER,
            .sda_io_num = TOUCH_PIN_SDA,
            .scl_io_num = TOUCH_PIN_SCL,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master = {.clk_speed = TOUCH_FREQ_HZ,
            },
            .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
    };
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
};

void app_main() {

    InitI2C();

    esp_lcd_panel_io_i2c_config_t io_config = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();
    esp_lcd_touch_config_t tp_cfg = {
            .x_max = CONFIG_LCD_HRES,
            .y_max = CONFIG_LCD_VRES,
            .rst_gpio_num = TOUCH_PIN_RESET,
            .int_gpio_num = TOUCH_PIN_INT,
            .levels = {
                    .reset = 0,
                    .interrupt = 0,
            },
            .flags = {
                    .swap_xy = 0,
                    .mirror_x = 0,
                    .mirror_y = 0,
            },
    };

    esp_lcd_touch_handle_t tp = NULL;
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_touch_new_i2c_gt911(io_handle, &tp_cfg, &tp);


}
