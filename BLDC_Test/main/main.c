#include<stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include"sra_board.h"
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (15) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz

#define MIN_THROTTLE_FOR 1100
#define FULL_THROTTLE_FOR 1900
#define MIN_THROTTLE_REV -48
#define FULL_THROTTLE_REV -2047

#define TAG "BLDC_Test"

static void example_ledc_init(void);

static void rampThrottle(int start, int stop, int step)
{
    if (step == 0)
        return;

    for (int i = start; step > 0 ? i < stop : i > stop; i += step)
    {
        /*esc.writeMicroseconds(i);
        vTaskDelay(1);*/
        /*int duty = (i > 0) ? i : -i; // Ensure duty is positive
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
        vTaskDelay(1 / portTICK_PERIOD_MS);*/
    }
    //esc.writeMicroseconds(stop);

    int final_duty = (stop > 0) ? stop : -stop; // Ensure final duty is positive
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, final_duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

void app_main()
{
    example_ledc_init();
    // Set duty to 50%
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY));
    // Update duty to apply the new value
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

    //motor_handle_t motor_a_0;
    //ESP_ERROR_CHECK(enable_motor_driver(&motor_a_0, MOTOR_A_0));

    /*(TAG, "Initializing DShot RMT");
    ESP_ERROR_CHECK(esc.install(DSHOT_GPIO, DSHOT_RMT_CHANNEL));
    ESP_ERROR_CHECK(esc.init());*/   

    //ESP_LOGI(TAG, "Reversing");
    //ESP_ERROR_CHECK(esc.setReversed(true));

    ESP_LOGI(TAG, "Increasing Speed");
    rampThrottle(MIN_THROTTLE_FOR, FULL_THROTTLE_FOR, 20);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Decreasing Speed");
    rampThrottle(FULL_THROTTLE_FOR, MIN_THROTTLE_FOR, -20);

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    /*ESP_LOGI(TAG, "Normal direction");
    //ESP_ERROR_CHECK(esc.setReversed(false));

    ESP_LOGI(TAG, "Increasing Speed");
    rampThrottle(MIN_THROTTLE_REV, FULL_THROTTLE_REV, -20);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Decreasing Speed");
    rampThrottle(FULL_THROTTLE_REV, MIN_THROTTLE_REV, 20);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Exiting");*/
}

static void example_ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}