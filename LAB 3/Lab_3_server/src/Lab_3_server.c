#include <cairo.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>      // For pow, sqrt
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>    // For usleep
#include "wrapper.h"
#define MAX_SIZE 1024
 //------------------------------------------ Structures & Globals -------------------------------------------

 double DT = 10;
pthread_mutex_t planet_mutex;

// The head of our linked-list of planets
planet_type *planet_list = NULL;

// The map width/height for boundary checks
int width = 700;
int height = 500;

//------------------------------------------ Function Declarations -------------------------------------------
void* mq_listener(void* args);
void* planet_thread(void* args);
static void do_drawing(cairo_t *cr);

GtkWidget *window;
GtkWidget *darea;

void add_planet_to_data_base(planet_type *planet);
void remove_planet_from_data_base(planet_type *planet);
void calculate_planet_pos(planet_type *p1);

//------------------------------------------ the message queue listener Thread -------------------------------------------
void *mq_listener(void *args)
{
    mqd_t *mq = (mqd_t *) args;

    while(1) {
        // We expect a planet_type in the queue, so size at least sizeof(planet_type).
        char buffer[sizeof(planet_type)];
        // Allocate a new planet and copy message data in
              planet_type* planet = malloc(sizeof(planet_type));
              int nbytes= MQread(*mq, buffer);
       if (nbytes <= 0) {
            // No valid message or read error
            usleep(50000);  // Sleep a bit to avoid tight spinning
            continue;
        }



        memcpy(planet, buffer, sizeof(planet_type));

        // Debugging: Print the planet received from the queue
        printf("Received planet: %s\n", planet->name);
        printf("Planet Details - Position: (%.2f, %.2f), Velocity: (%.2f, %.2f), Mass: %.2f, Radius: %.2f, Life: %d\n",
               planet->sx, planet->sy, planet->vx, planet->vy, planet->mass, planet->radius, planet->life);

        // Add planet to our database (linked list)
        pthread_mutex_lock(&planet_mutex);
        add_planet_to_data_base(planet);
        pthread_mutex_unlock(&planet_mutex);

        // Debugging: Print when the planet is added to the database
        printf("Planet added to database: %s\n", planet->name);

        // Spawn a thread to handle that planet's updates
        pthread_t thread;
        pthread_create(&thread, NULL, planet_thread, planet);
        printf("Created planet thread for: %s\n", planet->name);
    }
    pthread_exit(NULL);
}

//------------------------------------------ Planet Thread -------------------------------------------
void *planet_thread(void *args)
{
    planet_type *this_planet = (planet_type *)args;

    // Debugging: Print initial state of the planet
    printf("Planet thread started for: %s\n", this_planet->name);
    printf("Initial Planet State - Position: (%.2f, %.2f), Velocity: (%.2f, %.2f), Life: %d\n",
           this_planet->sx, this_planet->sy, this_planet->vx, this_planet->vy, this_planet->life);

    // Keep updating as long as planet is alive and in bounds
    while (this_planet->life > 0 &&
           this_planet->sx >= 0 && this_planet->sy >= 0 &&
           this_planet->sx < width && this_planet->sy < height)
    {
        pthread_mutex_lock(&planet_mutex);
        calculate_planet_pos(this_planet); // Update planet's position
        pthread_mutex_unlock(&planet_mutex);

        // Debugging: Print updated position and velocity
      // printf("Planet updated - Name: %s, Position: (%.2f, %.2f), Velocity: (%.2f, %.2f), Life: %d\n",
               //this_planet->name, this_planet->sx, this_planet->sy, this_planet->vx, this_planet->vy, this_planet->life);

        usleep(6000); // Sleep to simulate time step
    }

    // If we exit the loop, planet has either died or gone out of bounds
    char msg[100];
    if (this_planet->life <= 0) {
        snprintf(msg, sizeof(msg), "%s has died", this_planet->name);
    } else {
        snprintf(msg, sizeof(msg), "%s went outside the boundaries", this_planet->name);
    }

    // Debugging: Inform the client about the planet's fate
    printf("Planet %s: %s\n", this_planet->name, msg);

    // Inform the client that the planet is gone
    mqd_t mq;
    if (MQconnect(&mq, this_planet->pid)) {
        MQwrite(mq, msg);  // send message to the client
    }

    // Remove from database
    pthread_mutex_lock(&planet_mutex);
    remove_planet_from_data_base(this_planet);
    pthread_mutex_unlock(&planet_mutex);

    // Debugging: Print when planet is removed from database
    printf("Planet removed from database\n");

    pthread_exit(NULL);
}

//------------------------------------------ Drawing Functions -------------------------------------------
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    do_drawing(cr);
    return FALSE;
}

static void do_drawing(cairo_t *cr)
{
    // Set color: black
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Purisa", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    // Draw all planets
    pthread_mutex_lock(&planet_mutex);
    planet_type *current = planet_list;
    while (current != NULL) {
        // draw planet as a filled circle
        cairo_arc(cr, current->sx, current->sy, current->radius, 0, 2 * M_PI);
        cairo_fill(cr);
        current = current->next;
    }
    pthread_mutex_unlock(&planet_mutex);
    usleep(4000);


}

// This ticks every frame and queues a redraw
GtkTickCallback on_frame_tick(GtkWidget *widget, GdkFrameClock *frame_clock, gpointer user_data)
{
    gdk_frame_clock_begin_updating(frame_clock);
    gtk_widget_queue_draw(darea);
    gdk_frame_clock_end_updating(frame_clock);
}
//------------------------------------------ Planet Movement -------------------------------------------
void calculate_planet_pos(planet_type *p1)  //Function for calculating the position of a planet, relative to all other planets in the system
{
    planet_type *current = planet_list; //Poiinter to head in the linked list
    //Variable declarations
    double Atotx = 0;
    double Atoty = 0;
    double x = 0;
    double y = 0;
    double r = 0;
    double a = 0;
    double ax = 0;
    double ay = 0;

    double G = 6.67259 * pow(10, -11); //Declaration of the gravitational constant

    // Debugging: Print before calculation
    //printf("Calculating position for planet: %s\n", p1->name);
    while (current != NULL) //Loop through all planets in the list (there are more planets to process)
    {
//the current planet being processed is not the same as the planet for which the position is being calculated (p1)
        if (p1 != current) //Only update variables according to properties of other planets
        {
            x = current->sx - p1->sx;//calculation the difference in x_coordinate between the current planet and p1
            y = current->sy - p1->sy; //calculation the difference in y_coordinate between the current planet and p1
            r = sqrt(pow(x, 2.0) + pow(y, 2.0));//Euclidean distance, to find the distance between the two planets(current and p1), pow(base value, power value) returns the power value
            a = G * (current->mass / pow(r, 2.0));//computes the gravitational acceleration formula

            ay = a * (y / r);//computes the acceleration component along y-axis
            ax = a * (x / r);//computes the acceleration component along x-axis

            //accumulates the total acceleration components due to all planets
            Atotx += ax;
            Atoty += ay;

        }
        current = current->next;
    }
    //Update planet velocity, position and life after calculating the total acceleration
    p1->vx = p1->vx + (Atotx * DT); // updating it by the velocity formula: adding the total acc and the time step(DT=10ms)to the current velocity
    p1->vy = p1->vy + (Atoty * DT);
    p1->sx = p1->sx + (p1->vx * DT);//updating it by the position formula: (adding its velocity and DT) to the current position
    p1->sy = p1->sy + (p1->vy * DT);
    p1->life -= 1;

    // Debugging: Print the new velocity and position
   // printf("Updated position for planet: %s, Position: (%.2f, %.2f), Velocity: (%.2f, %.2f), Life: %d\n",
           //p1->name, p1->sx, p1->sy, p1->vx, p1->vy, p1->life);
}


//------------------------------------------ Linked-List Handling -------------------------------------------
void add_planet_to_data_base(planet_type *planet)
{
    // Debugging: Print when we add a planet to the database
    printf("Adding planet to database: %s\n", planet->name);

    planet->next = NULL;

    if (planet_list == NULL) {
        planet_list = planet;
        return;
    }

    planet_type *list = planet_list;
    while (list->next != NULL) {
        list = list->next;
    }
    list->next = planet;

    // Debugging: Print after adding the planet to the linked list
    printf("Planet added to database: %s\n", planet->name);
}

void remove_planet_from_data_base(planet_type *planet)
{
    if (!planet_list) return;  // empty list

    // If the head is the planet to remove
    if (planet_list == planet) {
        planet_list = planet_list->next;
        free(planet);
        return;
    }

    // Otherwise search the list
    planet_type *prev = planet_list;
    planet_type *cur  = planet_list->next;

    while (cur != NULL) {
        if (cur == planet) {
            prev->next = cur->next;
            free(cur);
            return;
        }
        prev = cur;
        cur  = cur->next;
    }
    // Not found, do nothing
}


//------------------------------------------ Main -------------------------------------------
int main(int argc, char *argv[])
{
    // Initialize mutex
    pthread_mutex_init(&planet_mutex, NULL);

    // Initialize GTK
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);
    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_title(GTK_WINDOW(window), "GTK window");
    gtk_widget_show_all(window);
    gtk_widget_add_tick_callback(darea, on_frame_tick, NULL, 1);

    // Create the server queue
    mqd_t mq;
    if (!MQcreate(&mq, "/planet_queue")) {
        return 1;
    }

    // Start the thread that listens for incoming planets
    pthread_t mq_listener_thread;
    pthread_create(&mq_listener_thread, NULL, mq_listener, &mq);
    // Typically, we do NOT join here, or we'd block and never reach GUI.
    // We'll join after gtk_main exits (if you want a clean shutdown).

    // Run the GTK main loop
    gtk_main();

   // pthread_cancel(mq_listener_thread); // or signal it to exit
    pthread_join(mq_listener_thread, NULL);

    return 0;
}
