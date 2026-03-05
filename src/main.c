#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// Sensor monitoring thread
void* sensor_thread(void* arg) {
    while (1) {
        printf("[Sensor Thread] Checking temperature and fire status...\n");
        sleep(2);
    }
    return NULL;
}

// Exit control thread
void* exit_thread(void* arg) {
    while (1) {
        printf("[Exit Thread] Determining safest exit path...\n");
        sleep(3);
    }
    return NULL;
}

// Alarm monitoring thread
void* alarm_thread(void* arg) {
    while (1) {
        printf("[Alarm Thread] Monitoring system for emergency...\n");
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t sensor, exit, alarm;

    printf("Smart Fire Evacuation System - Thread Demo\n");

    // Create threads
    pthread_create(&sensor, NULL, sensor_thread, NULL);
    pthread_create(&exit, NULL, exit_thread, NULL);
    pthread_create(&alarm, NULL, alarm_thread, NULL);

    // Wait for threads (program will keep running)
    pthread_join(sensor, NULL);
    pthread_join(exit, NULL);
    pthread_join(alarm, NULL);

    return 0;
}