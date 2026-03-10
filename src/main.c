#include <stdio.h>
#include <pthread.h>

void* sensor_task(void* arg);

int main(void) {
    printf("\n=== Progress Demo: One Thread + I2C Scan (QNX) ===\n\n");

    pthread_t t;
    if (pthread_create(&t, NULL, sensor_task, NULL) != 0) {
        perror("pthread_create");
        return 1;
    }
    pthread_join(t, NULL);

    printf("\n=== Demo Complete ===\n\n");
    return 0;
}