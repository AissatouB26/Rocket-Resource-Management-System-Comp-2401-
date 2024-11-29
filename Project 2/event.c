#include "defs.h"
#include <stdlib.h>
#include <stdio.h>

/* Event functions */

/**
 * Initializes an `Event` structure.
 *
 * Sets up an `Event` with the provided system, resource, status, priority, and amount.
 *
 * @param[out] event     Pointer to the `Event` to initialize.
 * @param[in]  system    Pointer to the `System` that generated the event.
 * @param[in]  resource  Pointer to the `Resource` associated with the event.
 * @param[in]  status    Status code representing the event type.
 * @param[in]  priority  Priority level of the event.
 * @param[in]  amount    Amount related to the event (e.g., resource amount).
 */
void event_init(Event *event, System *system, Resource *resource, int status, int priority, int amount) {
    event->system = system;
    event->resource = resource;
    event->status = status;
    event->priority = priority;
    event->amount = amount;
}

/* EventQueue functions */

/**
 * Initializes the `EventQueue`.
 *
 * Sets up the queue for use, initializing any necessary data (e.g., semaphores when threading).
 *
 * @param[out] queue  Pointer to the `EventQueue` to initialize.
 */
void event_queue_init(EventQueue *queue) {
    if(queue == NULL){
        printf("Error initializing EventQueue");
        exit(0);
    }
    queue->head = NULL;
}

/**
 * Cleans up the `EventQueue`.
 *
 * Frees any memory and resources associated with the `EventQueue`.
 * 
 * @param[in,out] queue  Pointer to the `EventQueue` to clean.
 */
void event_queue_clean(EventQueue *queue) {
    if(queue != NULL){
        EventNode *current = queue->head;
        while(current != NULL) {
            EventNode *temp = current;
            current = current->next;
            free(temp);
        }
        free(queue->head);
        free(queue);
    }
}
    

/**
 * Pushes an `Event` onto the `EventQueue`.
 *
 * Adds the event to the queue in a thread-safe manner, maintaining priority order (highest first).
 *
 * @param[in,out] queue  Pointer to the `EventQueue`.
 * @param[in]     event  Pointer to the `Event` to push onto the queue.
 */
void event_queue_push(EventQueue *queue, const Event *event) {
    //Null check
    if(queue == NULL || event == NULL){
        printf("Error pushing event.");
        exit(0);
    }

    //Creates pointers to the current and previous node
    EventNode *current = queue->head;
    EventNode *previous = NULL;

    //Allocates memory for new node
    EventNode *new_node = malloc(sizeof(EventNode));
    if (new_node == NULL) {
        printf("Memory allocation failed for new event node\n");
        exit(0); 
    }
    new_node->next = NULL;

    //Searches for spot to insert node
    while(current != NULL && current->event.priority > event->priority){
        previous = current;
        current = current->next;
    }
    printf("Found the spot to add it\n");

    //Checks if event will be the new head and inserts
    if(previous == NULL){
        new_node->next = queue->head;
        queue->head = new_node;
        printf("Added at the head!\n");
    }
    else{
        previous->next = new_node;
        new_node->next = current;
        printf("Added somewhere else!\n");
    }
    queue->size++;
    printf("Success!\n");
}

/**
 * Pops an `Event` from the `EventQueue`.
 *
 * Removes the highest priority event from the queue in a thread-safe manner.
 *
 * @param[in,out] queue  Pointer to the `EventQueue`.
 * @param[out]    event  Pointer to the `Event` structure to store the popped event.
 * @return               Non-zero if an event was successfully popped; zero otherwise.
 */
int event_queue_pop(EventQueue *queue, Event *event) {
    if(queue == NULL || event == NULL || queue->size == 0){
        return 0;
    }
    //Stores the old head event in the event parameter
    *event = queue->head->event;
    
    //Creates a temp variable to store the head node
    EventNode *temp = queue->head;

    //Reassigns head
    queue->head = queue->head->next;

    //Frees the old head node
    free(temp);

    //Decreases the size of the queue
    queue->size--;
    

    return 1;
}

void event_queue_print(const EventQueue *queue) {
    if (queue == NULL || queue->head == NULL) {
        printf("Event queue is empty.\n");
        return;
    }

    EventNode *current = queue->head;
    printf("Event Queue Contents:\n");
    while (current != NULL) {
        const Event *event = &current->event;
        printf("Event [Priority: %d, Status: %d, Amount: %d]\n",
               event->priority, event->status, event->amount);
        // Add more fields if needed, like event->system or event->resource
        current = current->next;
    }
}

// valgrind --leak-check=full -s ./program
// valgrind --leak-check=full --track-origins=yes ./program