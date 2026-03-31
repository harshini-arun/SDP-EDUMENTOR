#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct {
    int mentorID;
    char name[50];
    char department[50];
    char designation[50];
    char email[50];
    char phone[15];
} Mentor;

typedef struct Mentee {
    int menteeID;
    char name[50];
    char regNo[20];
    char department[50];
    int year;
    char email[50];
    char phone[15];
    float cgpa;
    float attendance;
    char remarks[100];
    char confidentialNotes[200];
    int mentorID;
    struct Mentee* next;  /* linked list pointer */
} Mentee;

/* ── NEW: one node in the hash chain ── */
typedef struct HashNode {
    char regNo[20];        /* key  */
    Mentee* menteePtr;     /* value: points to the real Mentee node */
    struct HashNode* next; /* next node in the chain at this slot   */
} HashNode;

#endif