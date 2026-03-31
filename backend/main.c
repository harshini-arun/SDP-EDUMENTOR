#include <stdio.h>
#include "functions.h"

int main() {
    int choice;
    extern HashNode* hashTable[];   /* CHANGED: was Mentee* */

    for (int i = 0; i < 100; i++) {
        hashTable[i] = NULL;
    }

    loadFromFile();

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Add Mentee\n");
        printf("2. View Mentees\n");
        printf("3. Search Mentee\n");
        printf("4. Update Mentee\n");
        printf("5. Delete Mentee\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addMentee();      break;
            case 2: displayMentees(); break;
            case 3: searchMentee();   break;
            case 4: updateMentee();   break;
            case 5: deleteMentee();   break;
            case 6: return 0;
            default: printf("Invalid choice\n");
        }
    }
}