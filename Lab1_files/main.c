#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_log.h"
// CHANGE this to the GPIO your external LED is connected to.
#define LED_GPIO GPIO_NUM_4
static const char *TAG = "LAB01";
static volatile int g_n = 10;
static volatile int g_result;
__attribute__((noinline))
static int sum_to_n(int n)
{
int total = 0;
for (int i = 1; i <= n; ++i) total += i;
return total;
}
void app_main(void)
{
ESP_ERROR_CHECK(gpio_reset_pin(LED_GPIO));
ESP_ERROR_CHECK(gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT));
g_result = sum_to_n(g_n);
ESP_LOGI(TAG, "sum(1..%d) = %d", (int)g_n, (int)g_result);
while (1) {
ESP_ERROR_CHECK(gpio_set_level(LED_GPIO, 1));
vTaskDelay(pdMS_TO_TICKS(200));
ESP_ERROR_CHECK(gpio_set_level(LED_GPIO, 0));
vTaskDelay(pdMS_TO_TICKS(800));
}
}
