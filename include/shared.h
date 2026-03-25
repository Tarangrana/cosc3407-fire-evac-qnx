#ifndef SHARED_H
#define SHARED_H

#include <semaphore.h>

// Shared variables
extern int fire_status;   // 0 = no fire, 1 = fire detected
extern int exit_path;     // 0 = Exit A, 1 = Exit B
extern int current_temp;  // latest temperature reading

// Semaphore protecting access to shared variables
extern sem_t fire_sem;

// Thread function declarations
void* sensor_task(void* arg);
void* alarm_task(void* arg);
void* exit_task(void* arg);

#endif // SHARED_H