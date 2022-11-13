#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include <time.h>

void Task1(void *pvParameter) {
    int maxcount = 5000;
    for(; ;) {
        int count = 0;
        printf("///// START TASK1 /////");
        while (count < maxcount) {
            printf("1");
            count++;
        }
        printf("///// END TASK1 /////");
        vTaskDelay(2000 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

void Task2(void * pvParameter) {
    int maxcount = 5000;
    
    for(; ;) {
        int count = 0;
        printf("///// START TASK2 /////");
        while (count < maxcount) {
            printf("2");
            count++;
        }
        printf("///// END TASK2 /////");
        vTaskDelay(2000 / portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}

//even_higher_priority_task
void Task3(void* pvParameter) {
    int maxcount = 500;
    
    for(; ;) {
        int count = 0;
        printf("///// START TASK3 /////");
        while (count < maxcount) {
            printf("3");
            count++;
        }
        printf("///// START TASK3 /////");
        vTaskDelay(200 /portTICK_RATE_MS);
    }
    vTaskDelete(NULL);
}


void app_main(void)
{
    xTaskCreatePinnedToCore(&Task1, "task1", 2048, NULL, 1, NULL, 0);

    xTaskCreatePinnedToCore(&Task2, "task2", 2048, NULL, 2, NULL, 0);

    xTaskCreatePinnedToCore(&Task3, "task3", 2048, NULL, 3, NULL, 0);
}
