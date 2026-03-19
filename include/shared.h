#ifndef SHARED_H
#define SHARED_H

#include <semaphore.h>

//Shared variables
extern int fire_status;
extern int exit_path;

//Semaphore protecting access to shared variables
extern sem_t fire_sem;

//Thread function declarations
void* sensor_task(void* arg);
void* alarm_task(void* arg);
void* exit_task(void* arg);

#endif // SHARED_H