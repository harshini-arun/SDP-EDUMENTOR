#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "structures.h"

/* Core functions */
void addMentee();
void displayMentees();
void searchMentee();
void updateMentee();
void deleteMentee();

/* File handling */
void loadFromFile();
void saveToFile();

/* Hashing — UPDATED: insertHash and deleteHash signatures unchanged */
void insertHash(Mentee* m);
Mentee* searchHash(char regNo[]);
void deleteHash(char regNo[]);

#endif