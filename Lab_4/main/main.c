#include <stdio.h>
#include <stdlib.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include <time.h>
#include <string.h>

struct QueueData
{
    int dataID;
    char message[20];
    char reject;
};

struct TaskType
{
    int taskID;
    char taskName[20];
};

QueueHandle_t xQueue;

struct TaskType Sensor_Air = {.taskName = "air", .taskID = 0};
struct TaskType Sensor_Moisture = {.taskName = "moisture", .taskID = 1};
struct TaskType Sensor_Water = {.taskName = "water", .taskID = 2};
struct TaskType Sensor_Light = {.taskName = "light", .taskID = 3};

void XQueue_Start(void *pvParameter)
{
    xQueue = xQueueCreate(10, sizeof(struct QueueData *));
    if (xQueue == NULL)
    {
        printf("Failed to create queue, not enough memory\n");
    }
    vTaskDelete(NULL);
}

void reception_Task(void *pvParameter)
{
    time_t t;

    srand((unsigned)time(&t));
    for (;;)
    {
        while (xQueue == NULL)
        {
            printf("xQueue is NULL\n");
        }
        int ran_job = (rand() % 4);
        int ran_delay = (rand() % 5) + 1;
        struct QueueData *xData = malloc(sizeof(struct QueueData));

        if (xData != NULL)
        {
            switch (ran_job)
            {
            case 0:
                xData->dataID = 0;
                strcpy(xData->message, "AIR");
                xData->reject = 0;
                break;
            case 1:
                xData->dataID = 1;
                strcpy(xData->message, "MOISTURE");
                xData->reject = 0;
                break;
            case 2:
                xData->dataID = 2;
                strcpy(xData->message, "WATER");
                xData->reject = 0;
                break;
            case 3:
                xData->dataID = 3;
                strcpy(xData->message, "LIGHT");
                xData->reject = 0;
                break;
            }
            if (xQueueSend(xQueue, (void *)&xData, 100) == errQUEUE_FULL)
            {
                printf("Failed to Import job with ID %d", ran_job);
            }
        }
        else
        {
            printf("Can't allocate new struct");
        }
        vTaskDelay(pdMS_TO_TICKS(100 * ran_delay));
    }
    vTaskDelete(NULL);
}

void active_Task(void *pvParameter)
{
    for (;;)
    {
        struct TaskType *task = (struct TaskType *)pvParameter;
        struct QueueData *pRxMessage;
        if (xQueue != NULL)
        {
            if (xQueueReceive(xQueue, &pRxMessage, (TickType_t)10) == pdPASS)
            {
                if (pRxMessage->dataID == task->taskID)
                {
                    printf("%s\n", pRxMessage->message);
                    // always remember to free the memory when done.
                    free(pRxMessage);
                }
                else
                {
                    printf("%s: received %s, but it's not my task\n", task->taskName, pRxMessage->message);
                    if (pRxMessage->reject < 3)
                    {
                        pRxMessage->reject++;
                        xQueueSendToFront(xQueue, (void *)&pRxMessage, (TickType_t)10);
                    }
                    else
                    {
                        printf("This task %s is rejected %d times, skiping the task\n", pRxMessage->message, pRxMessage->reject++);
                        free(pRxMessage);
                    }
                }
            }
            else
            {
                printf("queue empty\n");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreate(&XQueue_Start, "queue_start", 2048, NULL, 10, NULL);
    xTaskCreate(&reception_Task, "rec", 2048, NULL, 10, NULL);
    xTaskCreate(&active_Task, "air", 2048, (void *)&Sensor_Air, 10, NULL);
    xTaskCreate(&active_Task, "moisture", 2048, (void *)&Sensor_Moisture, 10, NULL);
    xTaskCreate(&active_Task, "water", 2048, (void *)&Sensor_Water, 10, NULL);
    xTaskCreate(&active_Task, "light", 2048, (void *)&Sensor_Light, 10, NULL);
}