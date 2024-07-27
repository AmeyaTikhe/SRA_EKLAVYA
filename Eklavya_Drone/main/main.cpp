#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "DShotRMT.h"

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"

static const char *TAG = "dshot-example";


#define DSHOT_RMT_CHANNEL RMT_CHANNEL_0
#define DSHOT_GPIO GPIO_NUM_15

#define MIN_THROTTLE 1200
#define FULL_THROTTLE 2047

DShotRMT esc;

static void rampThrottle(int start, int stop, int step)
{
    if (step == 0)
        return;

    ESP_LOGI(TAG, "Sending throttle values: Start= %d Stop= %d Step= %d", start, stop, step);

    for (int i = start; step > 0 ? i < stop : i > stop; i += step)
    {
        esc.sendThrottle(i);
        vTaskDelay(10/ portTICK_PERIOD_MS);
        //ESP_LOGI(TAG, "i= %d", i);

    }
    esc.sendThrottle(stop);
}

extern "C" void app_main(void)
{
    // ESP_LOGI(TAG, "Initializing DShot RMT");
    // ESP_ERROR_CHECK(esc.install(DSHOT_GPIO, DSHOT_RMT_CHANNEL));
    // ESP_ERROR_CHECK(esc.init());

    esp_err_t err;

    ESP_LOGI(TAG, "Initializing DShot RMT");

    err = esc.install(DSHOT_GPIO, DSHOT_RMT_CHANNEL);
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to install DShot: %s", esp_err_to_name(err));
        return;
    }
    err = esc.init();

    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to initialize DShot: %s", esp_err_to_name(err));
        return;
    }


    ESP_LOGW(TAG, "Reversing");
    // ESP_ERROR_CHECK(esc.setReversed(true));
    err = esc.setReversed(true);
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to set reverse mode: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Increasing Speed");
    rampThrottle(MIN_THROTTLE, FULL_THROTTLE, 20);

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Decreasing Speed");
    rampThrottle(FULL_THROTTLE, MIN_THROTTLE, -20);

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    ESP_LOGW(TAG, "Normal direction");
    //ESP_ERROR_CHECK(esc.setReversed(false));
    err = esc.setReversed(false);
    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to set normal direction: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Increasing Speed");
    rampThrottle(MIN_THROTTLE, FULL_THROTTLE, 20);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Decreasing Speed");
    rampThrottle(FULL_THROTTLE, MIN_THROTTLE, -20);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    ESP_LOGW(TAG, "Exiting");
}