#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <stdio.h>

static volatile uint32_t value = 0LLU;

static const char *TAG = "main";

/**
 * @brief Task that increments the variable value 1000000 times
 * 
 * @param param Task parameter (not used)
 */
void taskIncrement(void *param)
{
    (void)param;

    for (int i = 0; i < 1000000; i++)
    {
        value++;
    }

    vTaskDelete(NULL); // Delete the task
}

/**
 * @brief Main application function
 */
void app_main(void)
{
    vTaskPrioritySet(NULL, 5); // Set main task priority to 5

    // Start the two worker tasks with a lower priority than the main task
    if (xTaskCreate(taskIncrement, "increment_1", configMINIMAL_STACK_SIZE,
                    NULL, 4, NULL) != pdPASS ||
        xTaskCreate(taskIncrement, "increment_2", configMINIMAL_STACK_SIZE,
                    NULL, 4, NULL) != pdPASS)
    {
        ESP_LOGE(TAG, "Error creating tasks");
    }

    const uint64_t startTime_us = esp_timer_get_time();

    vTaskPrioritySet(NULL, 3); // Set main task priority to 3 - lower than the worker tasks

    const uint64_t difTime_ms = (esp_timer_get_time() - startTime_us) / 1000;

    printf("Value: %lu (used %llums)\n", value, difTime_ms);

    for (;;)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}