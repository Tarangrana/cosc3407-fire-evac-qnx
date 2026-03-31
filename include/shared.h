#ifndef SHARED_H
#define SHARED_H

#include <semaphore.h>

// Shared variables are declared here

extern int fire_status; // 0 for no fire, 1 for fire - there is a vairable exists somewhere to use
extern int exit_path; // 0 for no exit, 1 for exit - there is a vairable exists somewhere to use
extern int current_temp; // Current temperature in the building (reading)


// Semaphores protecting access to shared variables
extern sem_t fire_sem; // Semaphore for fire_status

// Thread fuction declarions

void* sensor_task(void* arg); // Function for the sensor thread
void* alarm_task(void* arg); // Function for the alarm thread
void* exit_task(void* arg); // Function for the evacuation thread

#endif // SHARED_H