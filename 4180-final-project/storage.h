#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include "config.h"
// Function prototypes
void storage_init();
void storage_add_entry(float grams_drank, float goal, float timestamp);
void storage_load_entries(Entry entries[MAX_ENTRIES]);
void storage_reset_entries();
void storage_print_entries();

#endif // STORAGE_H