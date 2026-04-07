#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // Added for better random IDs
#include "structures.h"

// Global Head for Linked List
Mentee* head = NULL;

// --- FILE HANDLING ---
void loadMentees() {
    FILE* fp = fopen("../data/mentee.dat", "rb");
    if (!fp) return;
    
    Mentee temp;
    while (fread(&temp, sizeof(Mentee), 1, fp)) {
        Mentee* newNode = (Mentee*)malloc(sizeof(Mentee));
        if (newNode == NULL) break;
        *newNode = temp;
        newNode->next = head;
        head = newNode;
    }
    fclose(fp);
}

void saveMentees() {
    FILE* fp = fopen("../data/mentee.dat", "wb");
    if (!fp) return;
    Mentee* curr = head;
    while (curr) {
        fwrite(curr, sizeof(Mentee), 1, fp);
        curr = curr->next;
    }
    fclose(fp);
}

// --- CORE FUNCTIONS ---

void login(int id, char* pass) {
    FILE* fp = fopen("../data/mentor.dat", "rb");
    if (!fp) { printf("0"); return; }
    
    Mentor m;
    while (fread(&m, sizeof(Mentor), 1, fp)) {
        if (m.mentorID == id && strcmp(m.password, pass) == 0) {
            printf("1,%s,%s,%s,%s,%s", m.name, m.department, m.designation, m.email, m.phone);
            fclose(fp);
            return;
        }
    }
    printf("0");
    fclose(fp);
}

void listMentees(int mentorID) {
    Mentee* curr = head;
    int count = 0;
    while (curr) {
        if (curr->mentorID == mentorID) {
            printf("%d|%s|%s|%.2f|%.2f|%s\n", 
                curr->menteeID, curr->name, curr->regNo, 
                curr->cgpa, curr->attendance, curr->remarks);
            count++;
        }
        curr = curr->next;
    }
    if (count == 0) printf("0");
}

void addMentee(Mentee m) {
    Mentee* newNode = (Mentee*)malloc(sizeof(Mentee));
    *newNode = m;
    newNode->next = head;
    head = newNode;
    saveMentees();
    printf("SUCCESS");
}

void deleteMentee(int id) {
    Mentee *curr = head, *prev = NULL;
    while (curr) {
        if (curr->menteeID == id) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr);
            saveMentees();
            printf("DELETED");
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("NOT_FOUND");
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    
    // Seed random for unique IDs
    srand(time(NULL));
    
    loadMentees();

    char* action = argv[1];

    if (strcmp(action, "login") == 0) {
        login(atoi(argv[2]), argv[3]);
    } 
    else if (strcmp(action, "list") == 0) {
        listMentees(atoi(argv[2]));
    }
    else if (strcmp(action, "add") == 0) {
        Mentee m;
        // 1. CLEAR ENTIRE MEMORY BLOCK FIRST
        memset(&m, 0, sizeof(Mentee)); 

        // 2. Assign values
        m.menteeID = (rand() % 9000) + 1000;
        strncpy(m.name, argv[2], 49);
        strncpy(m.regNo, argv[3], 19);
        m.cgpa = atof(argv[4]);
        m.attendance = atof(argv[5]);
        m.mentorID = atoi(argv[6]);

        // 3. Set default strings to be safe
        strcpy(m.remarks, "None");
        strcpy(m.confidentialNotes, "No notes.");
        strcpy(m.department, "IT");
        strcpy(m.email, "student@univ.edu");
        strcpy(m.phone, "0000000000");
        m.year = 1;

        addMentee(m);
    }
    else if (strcmp(action, "find") == 0) {
        int targetId = atoi(argv[2]);
        Mentee* curr = head;
        while (curr) {
            if (curr->menteeID == targetId) {
                // Use a safe print
                printf("%d|%s|%s|%.2f|%.2f|%s|%s", 
                    curr->menteeID, 
                    curr->name[0] ? curr->name : "N/A", 
                    curr->regNo[0] ? curr->regNo : "N/A", 
                    curr->cgpa, 
                    curr->attendance, 
                    curr->remarks[0] ? curr->remarks : "None", 
                    curr->confidentialNotes[0] ? curr->confidentialNotes : "None");
                return 0;
            }
            curr = curr->next;
        }
        printf("0");
    }
    else if (strcmp(action, "delete") == 0) {
        deleteMentee(atoi(argv[2]));
    }
    else if (strcmp(action, "find") == 0) {
        int targetId = atoi(argv[2]);
        Mentee* curr = head;
        while (curr) {
            if (curr->menteeID == targetId) {
                printf("%d|%s|%s|%.2f|%.2f|%s|%s", 
                    curr->menteeID, curr->name, curr->regNo, 
                    curr->cgpa, curr->attendance, curr->remarks, curr->confidentialNotes);
                return 0;
            }
            curr = curr->next;
        }
        printf("0");
    }
    else if (strcmp(action, "update") == 0) {
        int targetId = atoi(argv[2]);
        Mentee* curr = head;
        while (curr) {
            if (curr->menteeID == targetId) {
                curr->cgpa = atof(argv[3]);
                curr->attendance = atof(argv[4]);
                strncpy(curr->remarks, argv[5], 99);
                strncpy(curr->confidentialNotes, argv[6], 199);
                
                saveMentees();
                printf("UPDATED");
                return 0;
            }
            curr = curr->next;
        }
        printf("FAILED");
    }

    return 0;
}