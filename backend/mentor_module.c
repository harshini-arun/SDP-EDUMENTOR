#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "structures.h"

Mentee* head = NULL;

void loadMentees() {
    FILE* fp = fopen("../data/mentee.dat", "rb");
    if (!fp) return;
    Mentee temp;
    Mentee* tail = NULL;
    while (fread(&temp, sizeof(Mentee), 1, fp)) {
        Mentee* newNode = (Mentee*)malloc(sizeof(Mentee));
        *newNode = temp;
        newNode->next = NULL;
        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
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

void addMentee(Mentee m) {
    Mentee* newNode = (Mentee*)malloc(sizeof(Mentee));
    *newNode = m;
    newNode->next = NULL;
    if (!head) {
        head = newNode;
    } else {
        Mentee* tail = head;
        while (tail->next) tail = tail->next;
        tail->next = newNode;
    }
    saveMentees();
    printf("ADDED");
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

// --- MEETING QUEUE ---
Meeting* qHead = NULL;
Meeting* qTail = NULL;

void loadMeetings() {
    FILE* fp = fopen("../data/meetings.dat", "rb");
    if (!fp) return;
    Meeting temp;
    while (fread(&temp, sizeof(Meeting), 1, fp)) {
        Meeting* newNode = (Meeting*)malloc(sizeof(Meeting));
        *newNode = temp;
        newNode->next = NULL;
        if (!qHead) { qHead = newNode; qTail = newNode; }
        else { qTail->next = newNode; qTail = newNode; }
    }
    fclose(fp);
}

void saveMeetings() {
    FILE* fp = fopen("../data/meetings.dat", "wb");
    if (!fp) {
        printf("ERROR_FILE_OPEN"); // This helps debug from Python
        return;
    }
    Meeting* curr = qHead;
    while (curr) {
        fwrite(curr, sizeof(Meeting), 1, fp);
        curr = curr->next;
    }
    fclose(fp);
}

void enqueueMeeting(Meeting m) {
    Meeting* newNode = (Meeting*)malloc(sizeof(Meeting));
    *newNode = m;
    newNode->next = NULL;
    if (!qTail) { qHead = newNode; qTail = newNode; }
    else { qTail->next = newNode; qTail = newNode; }
    saveMeetings();
    printf("MEETING_ADDED");
}

void completeMeeting(int meetingID) {
    Meeting* curr = qHead;
    while (curr) {
        if (curr->meetingID == meetingID) {
            strncpy(curr->status, "completed", 19);
            saveMeetings();
            printf("COMPLETED");
            return;
        }
        curr = curr->next;
    }
    printf("NOT_FOUND");
}

void cancelMeeting(int meetingID) {
    Meeting *curr = qHead, *prev = NULL;
    while (curr) {
        if (curr->meetingID == meetingID) {
            if (prev) prev->next = curr->next;
            else qHead = curr->next;
            if (curr == qTail) qTail = prev;
            free(curr);
            saveMeetings();
            printf("CANCELLED");
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("NOT_FOUND");
}

void listMeetings(int mentorID) {
    Meeting* curr = qHead;
    while (curr) {
        if (curr->mentorID == mentorID) {
            printf("%d|%d|%s|%s|%s|%s|%s\n",
                curr->meetingID, curr->menteeID,
                curr->menteeName, curr->menteeReg,
                curr->topic, curr->datetime, curr->status);
        }
        curr = curr->next;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    srand(time(NULL));
    loadMentees();
    loadMeetings();
    char* action = argv[1];

    // 1. LOGIN
    if (strcmp(action, "login") == 0) {
        FILE* fp = fopen("../data/mentor.dat", "rb");
        if (!fp) { printf("0"); return 0; }
        Mentor m;
        int id = atoi(argv[2]);
        char* inputPass = argv[3];
        while (fread(&m, sizeof(Mentor), 1, fp)) {
            if (m.mentorID == id && strcmp(m.password, argv[3]) == 0) {
                printf("1,%s,%s,%s,%s,%s", m.name, m.department, m.designation, m.email, m.phone);
                fclose(fp); return 0;
            }
        }
        printf("0"); fclose(fp);
    }
    // 2. LIST (Find this in your main function)
    else if (strcmp(action, "list") == 0) {
        Mentee* curr = head;
        int mentorID = atoi(argv[2]);
        while (curr) {
            if (curr->mentorID == mentorID) {
                // EXACT ORDER: ID|Name|Reg|Dept|Year|Email|Phone|CGPA|Attn|Remarks|Notes
                printf("%d|%s|%s|%s|%d|%s|%s|%.2f|%.2f|%s|%s\n",
                    curr->menteeID, curr->name, curr->regNo,
                    curr->department, curr->year, curr->email, 
                    curr->phone, curr->cgpa, curr->attendance, 
                    curr->remarks, curr->confidentialNotes);
            }
            curr = curr->next;
        }
    }
    // 3. ADD (Updated to include new fields)
    else if (strcmp(action, "add") == 0) {
        Mentee m;
        memset(&m, 0, sizeof(Mentee));
        m.menteeID = (rand() % 9000) + 1000;
        strncpy(m.name, argv[2], 49);
        strncpy(m.regNo, argv[3], 19);
        strncpy(m.password, argv[3], 19); // Password is RegNo
        strncpy(m.department, argv[4], 49);
        m.year = atoi(argv[5]);
        strncpy(m.email, argv[6], 49);
        strncpy(m.phone, argv[7], 14);
        m.cgpa = atof(argv[8]);
        m.attendance = atof(argv[9]);
        strncpy(m.remarks, argv[10], 99); // Saving "Notes" as Remarks
        m.mentorID = atoi(argv[11]);
        addMentee(m);
    }
    // 4. FIND (Updated for pre-filling the edit form)
    else if (strcmp(action, "find") == 0) {
        int targetId = atoi(argv[2]);
        Mentee* curr = head;
        while (curr) {
            if (curr->menteeID == targetId) {
                printf("%d|%s|%s|%s|%d|%s|%s|%.2f|%.2f|%s",
                    curr->menteeID, curr->name, curr->regNo, curr->department,
                    curr->year, curr->email, curr->phone, curr->cgpa, 
                    curr->attendance, curr->remarks);
                return 0;
            }
            curr = curr->next;
        }
        printf("0");
    }
    // 5. UPDATE (Updated to save all fields)
    else if (strcmp(action, "update") == 0) {
        int targetId = atoi(argv[2]);
        Mentee* curr = head;
        while (curr) {
            if (curr->menteeID == targetId) {
                strncpy(curr->department, argv[3], 49);
                curr->year = atoi(argv[4]);
                strncpy(curr->email, argv[5], 49);
                strncpy(curr->phone, argv[6], 14);
                curr->cgpa = atof(argv[7]);
                curr->attendance = atof(argv[8]);
                strncpy(curr->remarks, argv[9], 99); // Notes saved as Remarks
                saveMentees();
                printf("UPDATED");
                return 0;
            }
            curr = curr->next;
        }
        printf("FAILED");
    }
    // 6. DELETE
    else if (strcmp(action, "delete") == 0) {
        deleteMentee(atoi(argv[2]));
    }
    // 7. MEETING ADD (Updated)
    else if (strcmp(action, "meeting_add") == 0) {
        Meeting m;
        memset(&m, 0, sizeof(Meeting));
        m.meetingID = (rand() % 9000) + 1000;
        m.mentorID  = atoi(argv[2]);
        m.menteeID  = atoi(argv[3]);
        strncpy(m.menteeName, argv[4], 49);
        strncpy(m.menteeReg,  argv[5], 19);
        strncpy(m.topic,      argv[6], 99);
        strncpy(m.datetime,   argv[7], 19);
        
        // NEW LOGIC: If argv[8] is "mentor", set to scheduled, else pending
        if (argc > 8 && strcmp(argv[8], "mentor") == 0)
            strcpy(m.status, "scheduled");
        else
            strcpy(m.status, "pending");

        enqueueMeeting(m);
    }
    // NEW ACTION: ACCEPT MEETING
    else if (strcmp(action, "meeting_accept") == 0) {
        int meetingID = atoi(argv[2]);
        Meeting* curr = qHead;
        while (curr) {
            if (curr->meetingID == meetingID) {
                strcpy(curr->status, "scheduled");
                saveMeetings();
                printf("ACCEPTED");
                return 0;
            }
            curr = curr->next;
        }
        printf("NOT_FOUND");
    }

    // 8. MEETING LIST
    else if (strcmp(action, "meeting_list") == 0) {
        listMeetings(atoi(argv[2]));
    }
    // 9. MEETING COMPLETE
    else if (strcmp(action, "meeting_complete") == 0) {
        completeMeeting(atoi(argv[2]));
    }
    // 10. MEETING CANCEL
    else if (strcmp(action, "meeting_cancel") == 0) {
        cancelMeeting(atoi(argv[2]));
    }

    return 0;
}