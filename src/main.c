#include <esp_lcd_panel_io.h>
#include "esp_psram.h"

#include "../managed_components/espressif__esp_lcd_touch_gt911/include/esp_lcd_touch_gt911.h"
//#include "../managed_components/espressif__esp_lcd_touch/include/esp_lcd_touch.h"
//#include "driver/spi_master.h"
#include "driver/i2c.h"
//#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "lcd.h"


#define CONFIG_LCD_HRES 800
#define CONFIG_LCD_VRES 480

#define TOUCH_PIN_RESET       (gpio_num_t)GPIO_NUM_38 // REST
#define TOUCH_PIN_SCL         (gpio_num_t)GPIO_NUM_20
#define TOUCH_PIN_SDA         (gpio_num_t)GPIO_NUM_19
#define TOUCH_PIN_INT         (gpio_num_t)GPIO_NUM_18
#define TOUCH_FREQ_HZ         (400000)

const i2c_port_t i2c_master_port = I2C_NUM_0;
static void periodic_timer_callback(void* arg);
static esp_lcd_touch_handle_t tp = NULL;
static uint16_t touch_x[1];
static uint16_t touch_y[1];
static uint16_t touch_strength[1];
static uint8_t touch_cnt = 0;

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

esp_err_t InitTouch(){

    esp_lcd_panel_io_handle_t io_handle;
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

    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)i2c_master_port, &io_config, &io_handle));
    return esp_lcd_touch_new_i2c_gt911(io_handle, &tp_cfg, &tp);

};
void app_main() {

//    esp_psram_init();
//    size_t psram_size = esp_psram_get_size();
//    printf("PSRAM size: %d bytes\n", psram_size);

    InitI2C();
    InitTouch();

    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "periodic"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 50000));



    LCDInit();
}

static void periodic_timer_callback(void* arg)
{
    esp_lcd_touch_read_data(tp);
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(tp, touch_x, touch_y, touch_strength, &touch_cnt, 1);
    if (touchpad_pressed == true){
        printf("x: %u y: %u strength: %u count: %u \n" , *touch_x, *touch_y, *touch_strength , touch_cnt);
    };
}
