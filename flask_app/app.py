#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "structures.h"

// Global Head for Linked List
Mentee* head = NULL;

// --- 1. FILE HANDLING FUNCTIONS ---

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

// --- 2. CORE LOGIC FUNCTIONS ---

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
}

void addMentee(Mentee m) {
    Mentee* newNode = (Mentee*)malloc(sizeof(Mentee));
    if (newNode == NULL) return;
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

// --- 3. MAIN FUNCTION (MUST BE AT THE BOTTOM) ---

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    
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
        memset(&m, 0, sizeof(Mentee)); 
        m.menteeID = (rand() % 9000) + 1000;
        strncpy(m.name, argv[2], 49);
        strncpy(m.regNo, argv[3], 19);
        m.cgpa = atof(argv[4]);
        m.attendance = atof(argv[5]);
        m.mentorID = atoi(argv[6]);
        strcpy(m.remarks, "None");
        strcpy(m.confidentialNotes, "No notes yet.");
        addMentee(m);
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
    else if (strcmp(action, "delete") == 0) {
        deleteMentee(atoi(argv[2]));
    }

    return 0;
}