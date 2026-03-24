#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include "shared.h"

// LED GPIO pins for exit guidance (you'll define based on your setup)
#define EXIT_A_LED 27   // Green LED for normal exit
#define EXIT_B_LED 22   // Red LED for emergency exit

void* exit_task(void* arg) {
    (void)arg;
    
    printf("[Exit ] Task started (priority: LOW)\n");
    
    while (1) {
        // Check fire status and determine exit path
        sem_wait(&fire_sem);
        int current_fire = fire_status;
        int current_exit = exit_path;
        sem_post(&fire_sem);
        
        if (current_fire) {
            // Fire detected - recommend emergency exit B
            sem_wait(&fire_sem);
            exit_path = 1;  // Exit B
            sem_post(&fire_sem);
            
            printf("[Exit ] EMERGENCY: Directing to Exit B\n");
            // TODO: Add GPIO code to light up Exit B LED
        }
        else {
            // Normal operation - use exit A
            sem_wait(&fire_sem);
            exit_path = 0;  // Exit A
            sem_post(&fire_sem);
            
            printf("[Exit ] Normal: Exit A available\n");
            // TODO: Add GPIO code to light up Exit A LED
        }
        
        sleep(2);  // Update every 2 seconds
    }
    
    return NULL;
}