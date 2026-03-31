#include <stdio.h>
#include <stdlib.h>
#include "structures.h"
#include "functions.h"

extern Mentee* head;

/* ── saveToFile: added NULL check to prevent crash ── */
void saveToFile() {
    FILE* fp = fopen("data/mentee.bin", "wb");

    if (fp == NULL) {                          /* FIX: guard against open failure */
        printf("ERROR: Could not open file for saving.\n");
        return;
    }

    Mentee* temp = head;
    while (temp != NULL) {
        Mentee tempData = *temp;
        tempData.next = NULL;                  /* never save pointer values */
        fwrite(&tempData, sizeof(Mentee), 1, fp);
        temp = temp->next;
    }

    fclose(fp);
}

/* ── loadFromFile: fixed order + added NULL check ── */
void loadFromFile() {
    FILE* fp = fopen("data/mentee.bin", "rb");
    if (fp == NULL) return;

    /* FIX: use a temporary array to preserve save order */
    Mentee* stack[1000];   /* simple temporary array, enough for most cases */
    int count = 0;

    Mentee temp;
    while (fread(&temp, sizeof(Mentee), 1, fp)) {
        Mentee* newNode = (Mentee*)malloc(sizeof(Mentee));
        *newNode = temp;
        newNode->next = NULL;
        stack[count++] = newNode;              /* collect first */
    }
    fclose(fp);

    /* Insert in reverse so the linked list order matches saved order */
    for (int i = count - 1; i >= 0; i--) {
        stack[i]->next = head;
        head = stack[i];
        insertHash(stack[i]);
    }
}