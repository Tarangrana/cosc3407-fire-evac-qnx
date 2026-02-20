# Thread Design

## Sensor Thread
- Reads temperature sensor
- Updates fire_status

## Exit Thread
- Reads fire_status
- Determines safest exit
- Updates exit_path

## Alarm Thread (Highest Priority)
- Monitors fire_status
- Activates buzzer
- Flashes LEDs

## Shared Variables
int fire_status;
int exit_path;

## Synchronization
Semaphore used to protect shared data.