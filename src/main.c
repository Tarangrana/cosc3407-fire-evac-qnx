#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include "shared.h"

int fire_status = 0;
int exit_path = 0; // 0 for exit A, 1 for exit B
int current_temp = 0; // Current temperature in the building (reading the sensor thread will update this variable)
sem_t fire_sem;

int main(void)
{
    if (sem_init(&fire_sem, 0, 1) != 0)
    {
        perror("sem_init failed");
        return EXIT_FAILURE;
    }

    pthread_t sensor_thread, alarm_thread, exit_thread;

    pthread_attr_t sensor_attr, alarm_attr, exit_attr;
    struct sched_param sensor_param, alarm_param, exit_param;

    pthread_attr_init(&sensor_attr);
    pthread_attr_init(&alarm_attr);
    pthread_attr_init(&exit_attr);

    pthread_attr_setinheritsched(&sensor_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setinheritsched(&alarm_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setinheritsched(&exit_attr, PTHREAD_EXPLICIT_SCHED);

    pthread_attr_setschedpolicy(&sensor_attr, SCHED_RR);
    pthread_attr_setschedpolicy(&alarm_attr, SCHED_RR);
    pthread_attr_setschedpolicy(&exit_attr, SCHED_RR);

    sensor_param.sched_priority = 20;
    alarm_param.sched_priority  = 30; // highest priority
    exit_param.sched_priority   = 10;

    pthread_attr_setschedparam(&sensor_attr, &sensor_param);
    pthread_attr_setschedparam(&alarm_attr, &alarm_param);
    pthread_attr_setschedparam(&exit_attr, &exit_param);

    if (pthread_create(&sensor_thread, &sensor_attr, sensor_task, NULL) != 0)
    {
        perror("Failed to create sensor thread");
        sem_destroy(&fire_sem);
        return EXIT_FAILURE;
    }

    if (pthread_create(&alarm_thread, &alarm_attr, alarm_task, NULL) != 0)
    {
        perror("Failed to create alarm thread");
        sem_destroy(&fire_sem);
        return EXIT_FAILURE;
    }

    if (pthread_create(&exit_thread, &exit_attr, exit_task, NULL) != 0)
    {
        perror("Failed to create exit thread");
        sem_destroy(&fire_sem);
        return EXIT_FAILURE;
    }

    pthread_join(sensor_thread, NULL);
    pthread_join(alarm_thread, NULL);
    pthread_join(exit_thread, NULL);

    pthread_attr_destroy(&sensor_attr);
    pthread_attr_destroy(&alarm_attr);
    pthread_attr_destroy(&exit_attr);
    sem_destroy(&fire_sem);

    return EXIT_SUCCESS;
}