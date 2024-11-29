#include "defs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

void load_data(Manager *manager);

int main(void) {

   event_test();
   
    /*
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
    */
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

void event_test(){
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
    event_init(&event1, system1, resource1, STATUS_OK, PRIORITY_HIGH, 10);
    event_init(&event2, system2, resource2, STATUS_INSUFFICIENT, PRIORITY_LOW, 5);
    event_init(&event3, system1, resource1, STATUS_EMPTY, PRIORITY_MED, 15);

    printf("Pushing events onto the queue...\n");
    event_queue_push(&queue, &event1);
    event_queue_push(&queue, &event2);
    event_queue_push(&queue, &event3);

    event_queue_print(&queue);

    /*
    // Pop events and display them
    printf("\nPopping events from the queue...\n");
    Event popped_event;
    while (event_queue_pop(&queue, &popped_event)) {
        printf("Popped event:\n");
        printf("  System: %s\n", popped_event.system->name);
        printf("  Resource: %s\n", popped_event.resource->name);
        printf("  Status: %d\n", popped_event.status);
        printf("  Priority: %d\n", popped_event.priority);
        printf("  Amount: %d\n", popped_event.amount);
    }
    */
    // Clean up resources and systems
    resource_destroy(resource1);
    resource_destroy(resource2);
    system_destroy(system1);
    system_destroy(system2);

    // Clean up the event queue
    event_queue_clean(&queue);
}

