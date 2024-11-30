#include "defs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

void load_data(Manager *manager);

int main(void) {
    //event_pop_test();
    
    Manager manager;
    manager_init(&manager);
    load_data(&manager);

    while (manager.simulation_running) {
        manager_run(&manager);
        for (int i = 0; i < manager.system_array.size; ++i) {
            system_run(manager.system_array.systems[i]);
        }
    }

    manager_clean(&manager);
    
    return 0;
}

/**
 * Loads sample data for the simulation.
 *
 * Calls all of the functions required to create resources and systems and add them to the Manager's data.
 *
 * @param[in,out] manager  Pointer to the `Manager` to populate with resource and system data.
 */
void load_data(Manager *manager) {
    // Create resources
    Resource *fuel, *oxygen, *energy, *distance;
    resource_create(&fuel, "Fuel", 1000, 1000);
    resource_create(&oxygen, "Oxygen", 20, 50);
    resource_create(&energy, "Energy", 30, 50);
    resource_create(&distance, "Distance", 0, 5000);

    resource_array_add(&manager->resource_array, fuel);
    resource_array_add(&manager->resource_array, oxygen);
    resource_array_add(&manager->resource_array, energy);
    resource_array_add(&manager->resource_array, distance);

    // Create systems
    System *propulsion_system, *life_support_system, *crew_capsule_system, *generator_system;
    ResourceAmount consume_fuel, produce_distance;
    resource_amount_init(&consume_fuel, fuel, 5);
    resource_amount_init(&produce_distance, distance, 25);
    system_create(&propulsion_system, "Propulsion", consume_fuel, produce_distance, 50, &manager->event_queue);

    ResourceAmount consume_energy, produce_oxygen;
    resource_amount_init(&consume_energy, energy, 7);
    resource_amount_init(&produce_oxygen, oxygen, 4);
    system_create(&life_support_system, "Life Support", consume_energy, produce_oxygen, 10, &manager->event_queue);

    ResourceAmount consume_oxygen, produce_nothing;
    resource_amount_init(&consume_oxygen, oxygen, 1);
    resource_amount_init(&produce_nothing, NULL, 0);
    system_create(&crew_capsule_system, "Crew", consume_oxygen, produce_nothing, 2, &manager->event_queue);

    ResourceAmount consume_fuel_for_energy, produce_energy;
    resource_amount_init(&consume_fuel_for_energy, fuel, 5);
    resource_amount_init(&produce_energy, energy, 10);
    system_create(&generator_system, "Generator", consume_fuel_for_energy, produce_energy, 20, &manager->event_queue);

    system_array_add(&manager->system_array, propulsion_system);
    system_array_add(&manager->system_array, life_support_system);
    system_array_add(&manager->system_array, crew_capsule_system);
    system_array_add(&manager->system_array, generator_system);
}

#include <assert.h>

void event_pop_test() {
    // Initialize an EventQueue
    EventQueue queue;
    event_queue_init(&queue);

    // Create dummy resources
    Resource *resource1, *resource2;
    resource_create(&resource1, "Resource1", 100, 200);
    resource_create(&resource2, "Resource2", 50, 150);

    // Create dummy systems
    System *system1, *system2;
    ResourceAmount consumed1, produced1, consumed2, produced2;
    resource_amount_init(&consumed1, resource1, 10);
    resource_amount_init(&produced1, resource1, 20);
    resource_amount_init(&consumed2, resource2, 5);
    resource_amount_init(&produced2, resource2, 15);

    system_create(&system1, "System1", consumed1, produced1, 1000, &queue);
    system_create(&system2, "System2", consumed2, produced2, 1500, &queue);

    // Create and push events with different priorities
    Event event1, event2, event3;
    event_init(&event1, system1, resource1, STATUS_OK, 3, 10);  // High priority
    event_init(&event2, system2, resource2, STATUS_INSUFFICIENT, 1, 5);  // Low priority
    event_init(&event3, system1, resource1, STATUS_EMPTY, 2, 15);  // Medium priority

    event_queue_push(&queue, &event1);
    event_queue_push(&queue, &event2);
    event_queue_push(&queue, &event3);

    // Check if events are being popped correctly
    printf("\nPopping events from the queue...\n");
    Event popped_event;

    // First pop should return the highest priority event (Priority: 3)
    if (event_queue_pop(&queue, &popped_event)) {
        if (popped_event.priority == 3) {
            printf("PASS: First pop returned highest priority event.\n");
        } else {
            printf("FAIL: First pop did not return highest priority event.\n");
        }
    } else {
        printf("FAIL: Pop returned 0 from non-empty queue.\n");
    }

    // Second pop should return the next highest priority event (Priority: 2)
    if (event_queue_pop(&queue, &popped_event)) {
        if (popped_event.priority == 2) {
            printf("PASS: Second pop returned next highest priority event.\n");
        } else {
            printf("FAIL: Second pop did not return next highest priority event.\n");
        }
    } else {
        printf("FAIL: Pop returned 0 from non-empty queue.\n");
    }

    // Third pop should return the lowest priority event (Priority: 1)
    if (event_queue_pop(&queue, &popped_event)) {
        if (popped_event.priority == 1) {
            printf("PASS: Third pop returned correct event.\n");
        } else {
            printf("FAIL: Third pop did not return correct event.\n");
        }
    } else {
        printf("FAIL: Pop returned 0 from non-empty queue.\n");
    }

    // Queue should be empty after popping all events
    if (!event_queue_pop(&queue, &popped_event)) {
        if (queue.head == NULL && queue.size == 0) {
            printf("PASS: Queue is empty after popping all events.\n");
        } else {
            printf("FAIL: Queue is not empty after popping all events.\n");
        }
    } else {
        printf("FAIL: Pop did not return 0 on empty queue.\n");
    }

    // Clean up resources and systems
    resource_destroy(resource1);
    resource_destroy(resource2);
    system_destroy(system1);
    system_destroy(system2);

    // Clean up the event queue
    event_queue_clean(&queue);
}

void push_test() {
    // Initialize an EventQueue
    EventQueue queue;
    event_queue_init(&queue);

    // Create dummy resources
    Resource *resource1, *resource2;
    resource_create(&resource1, "Resource1", 100, 200);
    resource_create(&resource2, "Resource2", 50, 150);

    // Create dummy systems
    System *system1, *system2;
    ResourceAmount consumed1, produced1, consumed2, produced2;
    resource_amount_init(&consumed1, resource1, 10);
    resource_amount_init(&produced1, resource1, 20);
    resource_amount_init(&consumed2, resource2, 5);
    resource_amount_init(&produced2, resource2, 15);

    system_create(&system1, "System1", consumed1, produced1, 1000, &queue);
    system_create(&system2, "System2", consumed2, produced2, 1500, &queue);

    // Create and push events with different priorities
    Event event1, event2, event3;
    event_init(&event1, system1, resource1, STATUS_OK, 3, 10);  // High priority
    event_init(&event2, system2, resource2, STATUS_INSUFFICIENT, 1, 5);  // Low priority
    event_init(&event3, system1, resource1, STATUS_EMPTY, 2, 15);  // Medium priority

    event_queue_push(&queue, &event1);
    event_queue_push(&queue, &event2);
    event_queue_push(&queue, &event3);

    // Directly print the event queue after pushing events
    printf("Event Queue after pushing events:\n");

    EventNode *current = queue.head;
    while (current != NULL) {
        printf("Event: Priority: %d, Amount: %d,  Status: %c\n", 
                current->event.priority, current->event.amount, current->event.status);
        current = current->next;
    }

    // Clean up resources and systems
    resource_destroy(resource1);
    resource_destroy(resource2);
    system_destroy(system1);
    system_destroy(system2);

    // Clean up the event queue
    event_queue_clean(&queue);
}



