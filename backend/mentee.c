#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"
#include "functions.h"

#define SIZE 100

Mentee* head = NULL;

/* ── Hash table: each slot is a chain of HashNodes ── */
HashNode* hashTable[SIZE];


/* ────────────────────────────────────────────────
   HASH FUNCTIONS
   ──────────────────────────────────────────────── */

int hashFunction(char regNo[]) {
    int sum = 0;
    for (int i = 0; regNo[i] != '\0'; i++)
        sum += regNo[i];
    return sum % SIZE;
}

void insertHash(Mentee* m) {
    int index = hashFunction(m->regNo);

    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    strcpy(newNode->regNo, m->regNo);
    newNode->menteePtr = m;

    /* Insert at front of chain */
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}

Mentee* searchHash(char regNo[]) {
    int index = hashFunction(regNo);
    HashNode* current = hashTable[index];

    while (current != NULL) {
        if (strcmp(current->regNo, regNo) == 0)
            return current->menteePtr;  /* exact match found */
        current = current->next;
    }
    return NULL;  /* not found */
}

void deleteHash(char regNo[]) {
    int index = hashFunction(regNo);
    HashNode* current = hashTable[index];
    HashNode* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->regNo, regNo) == 0) {
            if (prev == NULL)
                hashTable[index] = current->next;
            else
                prev->next = current->next;
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}


/* ────────────────────────────────────────────────
   ADD MENTEE
   ──────────────────────────────────────────────── */

void addMentee() {
    Mentee* newNode = (Mentee*)malloc(sizeof(Mentee));

    printf("Enter ID: ");
    scanf("%d", &newNode->menteeID);
    printf("Enter Name: ");
    scanf(" %[^\n]", newNode->name);
    printf("Enter Register No: ");
    scanf("%s", newNode->regNo);
    printf("Enter Department: ");
    scanf("%s", newNode->department);
    printf("Enter Year: ");
    scanf("%d", &newNode->year);
    printf("Enter Email: ");
    scanf("%s", newNode->email);
    printf("Enter Phone: ");
    scanf("%s", newNode->phone);
    printf("Enter CGPA: ");
    scanf("%f", &newNode->cgpa);
    printf("Enter Attendance: ");
    scanf("%f", &newNode->attendance);
    printf("Enter Remarks: ");
    scanf(" %[^\n]", newNode->remarks);
    printf("Enter Confidential Notes: ");
    scanf(" %[^\n]", newNode->confidentialNotes);

    /* Add to front of linked list */
    newNode->next = head;
    head = newNode;

    /* Add to hash table */
    insertHash(newNode);

    saveToFile();
    printf("Mentee Added Successfully!\n");
}


/* ────────────────────────────────────────────────
   DISPLAY ALL MENTEES
   ──────────────────────────────────────────────── */

void displayMentees() {
    Mentee* temp = head;

    if (temp == NULL) {
        printf("No mentees found.\n");
        return;
    }

    while (temp != NULL) {
        printf("\n--------------------------\n");
        printf("ID         : %d\n",   temp->menteeID);
        printf("Name       : %s\n",   temp->name);
        printf("Reg No     : %s\n",   temp->regNo);
        printf("Department : %s\n",   temp->department);
        printf("Year       : %d\n",   temp->year);
        printf("Email      : %s\n",   temp->email);
        printf("Phone      : %s\n",   temp->phone);
        printf("CGPA       : %.2f\n", temp->cgpa);
        printf("Attendance : %.2f\n", temp->attendance);
        printf("Remarks    : %s\n",   temp->remarks);
        temp = temp->next;
    }
    printf("--------------------------\n");
}


/* ────────────────────────────────────────────────
   SEARCH MENTEE
   ──────────────────────────────────────────────── */

void searchMentee() {
    char regNo[20];
    printf("Enter Register No to search: ");
    scanf("%s", regNo);

    Mentee* m = searchHash(regNo);

    if (m != NULL) {
        printf("\n--- Mentee Found ---\n");
        printf("ID         : %d\n",   m->menteeID);
        printf("Name       : %s\n",   m->name);
        printf("Reg No     : %s\n",   m->regNo);
        printf("Department : %s\n",   m->department);
        printf("Year       : %d\n",   m->year);
        printf("CGPA       : %.2f\n", m->cgpa);
        printf("Attendance : %.2f\n", m->attendance);
    } else {
        printf("Mentee with Reg No '%s' not found.\n", regNo);
    }
}


/* ────────────────────────────────────────────────
   UPDATE MENTEE
   ──────────────────────────────────────────────── */

void updateMentee() {
    char regNo[20];
    printf("Enter Register No to update: ");
    scanf("%s", regNo);

    Mentee* m = searchHash(regNo);

    if (m == NULL) {
        printf("Mentee not found.\n");
        return;
    }

    printf("Updating record for: %s\n", m->name);
    printf("Enter New CGPA: ");
    scanf("%f", &m->cgpa);
    printf("Enter New Attendance: ");
    scanf("%f", &m->attendance);
    printf("Enter New Remarks: ");
    scanf(" %[^\n]", m->remarks);
    printf("Enter New Confidential Notes: ");
    scanf(" %[^\n]", m->confidentialNotes);

    saveToFile();
    printf("Updated Successfully!\n");
}


/* ────────────────────────────────────────────────
   DELETE MENTEE
   ──────────────────────────────────────────────── */

void deleteMentee() {
    char regNo[20];
    printf("Enter Register No to delete: ");
    scanf("%s", regNo);

    /* Step 1: Remove from linked list */
    Mentee* temp = head;
    Mentee* prev = NULL;

    while (temp != NULL && strcmp(temp->regNo, regNo) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Mentee not found.\n");
        return;
    }

    if (prev == NULL)
        head = temp->next;       /* deleting the first node */
    else
        prev->next = temp->next; /* bypass the node */

    /* Step 2: Remove from hash table */
    deleteHash(regNo);

    /* Step 3: Free memory */
    free(temp);

    saveToFile();
    printf("Deleted Successfully!\n");
}