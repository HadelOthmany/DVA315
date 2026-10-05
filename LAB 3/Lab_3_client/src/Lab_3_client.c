#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "wrapper.h"
#define MAX_SIZE 1024


void take_planet_info(planet_type* planet) {
    printf("Enter planet details\nName: ");
    scanf("%s", planet->name);
    printf("X Position: ");
    scanf("%lf", &planet->sx);
    printf("Y Position: ");
    scanf("%lf", &planet->sy);
    printf("X Velocity: ");
    scanf("%lf", &planet->vx);
    printf("Y Velocity: ");
    scanf("%lf", &planet->vy);
    printf("Mass: ");
    scanf("%lf", &planet->mass);
    printf("Radius: ");
    scanf("%lf", &planet->radius);
    printf("Life: ");
    scanf("%d", &planet->life);

    // Debugging: Print out the planet info entered
    printf("Planet Info Entered:\n");
    printf("Name: %s\n", planet->name);
    printf("Position: (%.2f, %.2f)\n", planet->sx, planet->sy);
    printf("Velocity: (%.2f, %.2f)\n", planet->vx, planet->vy);
    printf("Mass: %.2f\n", planet->mass);
    printf("Radius: %.2f\n", planet->radius);
    printf("Life: %d\n", planet->life);
}

void pid_string(char* buffer, int buffer_length) {
    pid_t pid = getpid();
    snprintf(buffer, buffer_length, "/death_queue_%d", pid);
}

// handle messages from the server
void* server_messages(void* args) {
    mqd_t* mq = args;
    while(1) {
        char buffer[MAX_SIZE];  // Define a buffer to store incoming messages

        // Read a message from the message queue
        if (!MQread(*mq, buffer)) {
            fprintf(stderr, "Error reading message from queue\n");
            pthread_exit((void*)EXIT_FAILURE);
        }

        // Print the received message
        printf("Received message from server: %s\n", buffer);
    }
    pthread_exit((void*)EXIT_SUCCESS);  // Exit the thread
}

int main(void)
{
    char pid[30];                               // character array to store the process ID
    mqd_t planet_queue, death_queue;           // Define message queue descriptors
    pthread_t thread;
    pid_string(pid, sizeof(pid));               // Generate a process ID string

    // Debugging: Print the process ID
    printf("Client PID: %s\n", pid);

    if (!MQconnect(&planet_queue, "/planet_queue")) {    // Connect to the server message queue
        fprintf(stderr, "Error connecting to server message queue\n");
        return EXIT_FAILURE;                          // Return failure if connection fails
    }

    // Debugging: Confirm the server queue connection
    printf("Connected to server message queue '/planet_queue'\n");

    if (!MQcreate(&death_queue, pid)) {                 // Create a message queue for death notifications
        fprintf(stderr, "Error creating death queue\n");
        return EXIT_FAILURE;
    }

    // Debugging: Confirm the client queue creation
    printf("Death queue created with name: %s\n", pid);

    pthread_create(&thread, NULL, server_messages, &death_queue);      // Create a thread to handle server messages

    while (1) {
        planet_type planet = { 0 };  // Create a planet object
        take_planet_info(&planet);   // Input planet information

        // Debugging: Print the planet info before sending
        printf("Sending planet info to server:\n");
        printf("Name: %s\n", planet.name);
        printf("Position: (%.2f, %.2f)\n", planet.sx, planet.sy);
        printf("Velocity: (%.2f, %.2f)\n", planet.vx, planet.vy);
        printf("Mass: %.2f\n", planet.mass);
        printf("Radius: %.2f\n", planet.radius);
        printf("Life: %d\n", planet.life);

        strncpy(planet.pid, pid, sizeof(planet.pid));  // Copy the process ID to the planet structure

        // Debugging: Print message before sending the planet
        printf("Sending planet to server via message queue...\n");
        MQwrite(planet_queue, &planet);

        usleep(1000000);  // Delay before entering next loop iteration (1 second delay)
    }

    // Clean up
    if (!MQclose(&planet_queue,"/planet_queue")) {
        fprintf(stderr, "Error closing server message queue\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
