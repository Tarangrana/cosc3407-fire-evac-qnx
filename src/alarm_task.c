#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include "shared.h"

// Buzzer GPIO pin (you'll define this based on your hardware setup)
#define BUZZER_PIN 17  // Example GPIO pin

void* alarm_task(void* arg) {
    (void)arg;
    
    int buzzer_active = 0;
    
    printf("[Alarm] Task started (priority: HIGH)\n");
    
    while (1) {
        // Check fire status with semaphore protection
        sem_wait(&fire_sem);
        int current_fire = fire_status;
        sem_post(&fire_sem);
        
        // Control buzzer based on fire status
        if (current_fire && !buzzer_active) {
            // Activate buzzer
            printf("[Alarm] FIRE DETECTED! Activating alarm...\n");
            // TODO: Add GPIO code to activate buzzer
            buzzer_active = 1;
        }
        else if (!current_fire && buzzer_active) {
            // Deactivate buzzer
            printf("[Alarm] Fire cleared. Deactivating alarm...\n");
            // TODO: Add GPIO code to deactivate buzzer
            buzzer_active = 0;
        }
        
        sleep(1);  // Check every second
    }
    
    return NULL;
}