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
        return;
    }
    queue->head = NULL;
    queue->size = 0;
}

/**
 * Cleans up the `EventQueue`.
 *
 * Frees any memory and resources associated with the `EventQueue`.
 * 
 * @param[in,out] queue  Pointer to the `EventQueue` to clean.
 */
void event_queue_clean(EventQueue *queue) {
    if (queue != NULL) {
        EventNode *current = queue->head;
        while (current != NULL) {
            EventNode *temp = current;
            current = current->next;

            if(temp->event.system != NULL){
                system_destroy(temp->event.system);
                temp->event.system = NULL;
            }

            if(temp->event.resource != NULL){
                resource_destroy(temp->event.resource);
                temp->event.resource = NULL;
            }
            
            free(temp);
        }
        queue->head = NULL;
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
    // Null check
    if (queue == NULL || event == NULL) {
        printf("Error pushing event.\n");
        return;
    }

    // Allocate memory for the new node
    EventNode *new_node = malloc(sizeof(EventNode));
    if (new_node == NULL) {
        printf("Memory allocation failed for new event node\n");
        return;
    }

    // Copy the event data into the new node
    new_node->event = *event;  
    new_node->next = NULL;

    // Find the correct spot to insert the node
    EventNode *current = queue->head;
    EventNode *previous = NULL;
    while (current != NULL && current->event.priority > event->priority) {
        previous = current;
        current = current->next;
    }

    // Insert the new node
    if (previous == NULL) {
        // Insert at the head
        new_node->next = queue->head;
        queue->head = new_node;
    } else {
        // Insert elsewhere
        previous->next = new_node;
        new_node->next = current;
    }

    queue->size++;
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
    if (queue == NULL) {
        //printf("input queue invalid.");
        return 0; 
    }

    if(event == NULL){
        //printf("event invalid.");
        return 0; 
    }

    if(queue->size == 0){
        //printf("Tried to pop from empty queue.");
        return 0; 

    }

    if(queue->head == NULL){
        //printf("The head of the queue is null.");
        return 0; 
    }

    // Stores the old head event in the event parameter
    *event = queue->head->event;

    // Creates a temp variable to store the head node
    EventNode *temp = queue->head;

    // Reassigns head
    queue->head = queue->head->next;

    // Frees the old head node
    free(temp);

    // Decreases the size of the queue
    queue->size--;

    // Ensure `head` is NULL when queue becomes empty
    if (queue->size == 0) {
        queue->head = NULL;
    }

    return 1;
}



// valgrind --leak-check=full -s ./program
// valgrind --leak-check=full --track-origins=yes ./program