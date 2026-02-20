# Smart Fire Evacuation & Exit Guidance System

## Course
COSC3407 - Operating Systems I

## Description
This project simulates a smart fire detection and evacuation guidance system using QNX on Raspberry Pi.

The system uses multiple threads to monitor environmental conditions, determine the safest exit path, and trigger emergency alarms.

## Threads
1. Sensor Monitoring Thread
2. Exit Control Thread
3. Emergency Alarm Thread (Highest Priority)

## Shared Resources
- fire_status
- exit_path

## Synchronization
A semaphore is used to protect shared variables and prevent race conditions.

## Scheduling
The Emergency Alarm thread is assigned the highest priority to demonstrate pre-emptive scheduling.