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

## Code Sources and Adaptation
This project has been developed based on original implementation, QNX official documentation, and QNX hardware sample libraries. These references were mainly used as learning material and then adapted into a custom multi-threaded fire evacuation simulation program instead of copying reference programs as is.

This means that instead of copying reference programs as is, these references were used as learning material and then adapted into a custom multi-threaded fire evacuation simulation program.

## QNX Raspberry Pi GPIO Sample Library
One of the main resources used during development was the QNX Raspberry Pi GPIO hardware component sample library.

Reference:
https://gitlab.com/qnx/projects/hardware-component-samples

This library is used for basic operations like configuring GPIO pins, reading input signals, and controlling output devices on the Raspberry Pi. This library was helpful in understanding the way to initialize the pins.

The GPIO interfaces were used as references for:

initializing GPIO pins
controlling LEDs used for exit guidance
activating the buzzer alarm
reading button or sensor inputs

Instead of directly using the sample code, the patterns of access to GPIO were incorporated into this project’s modules. For instance, LED access was incorporated into exit_task.c, while buzzer access was incorporated into alarm_task.c, with its behavior determined by the system’s shared state for fire detection.

## QNX POSIX Threads and Scheduling Documentation
The system relies on concurrent execution using POSIX threads. The official QNX documentation was used to understand how to create threads, configure scheduling attributes, and assign thread priorities.

Reference:
https://www.qnx.com/developers/docs/8.0/#com.qnx.doc.neutrino.getting_started/topic/s1_procs_Thread_attributes_scheduling.html

This documentation guided the implementation of:

pthread_create() for starting concurrent tasks
configuring thread attributes
explicit scheduling using PTHREAD_EXPLICIT_SCHED
round-robin scheduling with SCHED_RR
assigning different priorities to system tasks

These concepts were utilized in designing three concurrent tasks in the system. They include:

Sensor Task – This task is responsible for monitoring sensor data and updating the fire state
Alarm Task – This task is responsible for sounding the buzzer when fire is detected
Exit Task – This task is responsible for controlling the LED lights to guide the exit of the occupants

The alarm task was assigned the highest priority so that the emergency actions can respond quickly when the fire state changes.

## QNX Semaphore Synchronization Documentation
Since there are multiple threads accessing the same variables, it is necessary to perform some kind of synchronization so that the variables are not updated in an inconsistent manner. The documentation for the QNX semaphore has been used to determine how shared variables can be synchronized between tasks.

Reference:
https://www.qnx.com/developers/docs/8.0/#com.qnx.doc.neutrino.sys_arch/topic/kernel_Semaphores.html

The following functions were used in the project:

sem_init() to initialize the semaphore
sem_wait() to enter a critical section
sem_post() to release the critical section
sem_destroy() to clean up the semaphore

The semaphores will be used to protect shared variables such as fire_status to ensure the sensor, alarm, and exit tasks access consistent system information.

## Build System and Cross-Compilation
The project will be compiled using the QNX qcc compiler and the target system will be the Raspberry Pi running QNX Neutrino.

A Makefile has been created to simplify the compilation process. Instead of having to type out long compilation commands, the Makefile will take care of everything.

compiling all source files
building required hardware libraries
compiling test programs
deploying executables to the Raspberry Pi
running the program through SSH

The format of the Makefile was based on common practices in creating a Makefile using the GNU Make utility and was modified to suit the structure and requirements of the project.

## Challenges During Development
There were several challenges encountered in the process of integrating the external references to the project.

The first challenge was in integrating the QNX GPIO sample library to the directory structure. This was because the library had its own directory structure and settings. Hence, the Makefile had to be modified to suit the requirements.

The second challenge was in understanding the function signatures of the GPIO library and matching them with the correct parameters. This had to be done by creating a simple hardware test program to understand the correct parameters.

The third challenge was in the testing of the fire detection logic. This was because the sensor would sometimes be difficult to trigger in a demonstration. Hence, a simple simulation using a button was created to trigger the fire condition.

The fourth and final challenge was in the coordination of the shared data access by multiple threads. This was achieved by using a semaphore to avoid a race condition. Hence, the logic was refined to allow the safe access of the shared system state.

## AI Assistance Disclosure
The main use of the AI tools was in the debugging process, where the tools were helpful in understanding the QNX APIs and providing suggestions to improve the structure of the program.

The final integration and testing, along with the design decisions, were done by the student. The tools were used as a supporting source, but the source code was not obtained directly from the tools.