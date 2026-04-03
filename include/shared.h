/*
 * shared.h
 * Smart Fire Evacuation and Exit Guidance System
 * COSC3407 - Operating Systems I
 *
 * Purpose:
 * Declares shared variables, synchronization objects, and task function prototypes used across the project modules.
 *
 * Source references:
 * - QNX semaphore documentation:
 *   https://www.qnx.com/developers/docs/8.0/#com.qnx.doc.neutrino.sys_arch/topic/kernel_Semaphores.html
 *
 * Notes:
 * The synchronization concept is based on QNX semaphore usage, while the shared state design and module organization are specific to this project.
 */

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