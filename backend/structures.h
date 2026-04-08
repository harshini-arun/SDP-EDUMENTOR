#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct {
    int mentorID;
    char name[50];
    char department[50];
    char designation[50];
    char email[50];
    char phone[15];
    char password[20]; // Added for login
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
    struct Mentee* next;
} Mentee;

typedef struct Meeting {
    int meetingID;
    int mentorID;
    int menteeID;
    char menteeName[50];
    char menteeReg[20];
    char topic[100];
    char datetime[20];   
    char status[20];     
    struct Meeting* next;
} Meeting;

#endif