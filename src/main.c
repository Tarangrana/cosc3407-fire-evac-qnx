#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <pthread.h>
#include <semaphore.h>
#include "shared.h"

int fire_status = 0; // 0 = no fire, 1 = fire detected
int exit_path = 0;   // 0 = normal, 1 = exit path  
sem_t fire_sem;     // Semaphore to protect access to fire_status and exit_path

int main(void) 
{
    // Initialize the semaphore to protect shared variables (fire_status and exit_path)
    if (sem_init(&fire_sem, 0, 1) != 0) 
    {
        perror("Failed to initialize semaphore");
        return EXIT_FAILURE;
    } 

    // Set up thread attributes for real-time scheduling
    pthread_attr_t sensor_attr, alarm_attr, exit_attr;
    struct sched_param sensor_param, alarm_param, exit_param; // Scheduling parameters for each thread

    // Initialize thread attributes
    pthread_attr_init(&sensor_attr);
    pthread_attr_init(&alarm_attr);
    pthread_attr_init(&exit_attr);

    // Set threads to use explicit scheduling parameters
    pthread_attr_setinheritsched(&sensor_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setinheritsched(&alarm_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setinheritsched(&exit_attr, PTHREAD_EXPLICIT_SCHED);

    // Set scheduling policy to Round Robin
    pthread_attr_setschedpolicy(&sensor_attr, SCHED_RR);
    pthread_attr_setschedpolicy(&alarm_attr, SCHED_RR);
    pthread_attr_setschedpolicy(&exit_attr, SCHED_RR);

    // Set priorities (higher number = higher priority)
    sensor_param.sched_priority = 20; // Medium priority
    alarm_param.sched_priority = 30;  // Highest priority
    exit_param.sched_priority = 10;   // Lowest priority

    // Apply scheduling parameters to thread attributes
    pthread_attr_setschedparam(&sensor_attr, &sensor_param);
    pthread_attr_setschedparam(&alarm_attr, &alarm_param);
    pthread_attr_setschedparam(&exit_attr, &exit_param);

    pthread_t sensor_thread, alarm_thread, exit_thread;

    // Create threads for each task
    if (pthread_create(&sensor_thread, &sensor_attr, sensor_task, NULL) != 0) 
    {
        perror("Failed to create sensor thread");
        pthread_attr_destroy(&sensor_attr);
        pthread_attr_destroy(&alarm_attr);
        pthread_attr_destroy(&exit_attr);
        sem_destroy(&fire_sem);
        return EXIT_FAILURE;
    }

    if (pthread_create(&alarm_thread, &alarm_attr, alarm_task, NULL) != 0) 
    {
        perror("Failed to create alarm thread");
        pthread_attr_destroy(&sensor_attr);
        pthread_attr_destroy(&alarm_attr);
        pthread_attr_destroy(&exit_attr);
        sem_destroy(&fire_sem);
        return EXIT_FAILURE;
    }

    if (pthread_create(&exit_thread, &exit_attr, exit_task, NULL) != 0) 
    {
        perror("Failed to create exit thread");
        pthread_attr_destroy(&sensor_attr);
        pthread_attr_destroy(&alarm_attr);
        pthread_attr_destroy(&exit_attr);
        sem_destroy(&fire_sem);
        return EXIT_FAILURE;
    }

    // Wait for threads to finish (in this case they run indefinitely)
    pthread_join(sensor_thread, NULL);
    pthread_join(alarm_thread, NULL);
    pthread_join(exit_thread, NULL);

    // Clean up thread attributes and semaphore
    pthread_attr_destroy(&sensor_attr);
    pthread_attr_destroy(&alarm_attr);
    pthread_attr_destroy(&exit_attr);
    sem_destroy(&fire_sem);  //Destroy semaphore before exiting

    return EXIT_SUCCESS;    
}