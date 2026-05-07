#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "structures.h"

// LOGIN
int menteeLogin(int id, char* pass) {
    FILE* fp = fopen("../data/mentee.dat", "rb");
    if (!fp) return 0;

    Mentee temp;
    while (fread(&temp, sizeof(Mentee), 1, fp)) {
        if (temp.menteeID == id && strcmp(temp.password, pass) == 0) {
            printf("1|%d|%s|%d", temp.menteeID, temp.name, temp.mentorID);
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// VIEW SELF DETAILS
void viewMyDetails(int id) {
    FILE* fp = fopen("../data/mentee.dat", "rb");
    if (!fp) return;
    Mentee temp;
    while (fread(&temp, sizeof(Mentee), 1, fp)) {
        if (temp.menteeID == id) {
            printf("%d|%s|%s|%s|%d|%s|%s|%.2f|%.2f|%s",
                temp.menteeID, temp.name, temp.regNo, temp.department,
                temp.year, temp.email, temp.phone, temp.cgpa, 
                temp.attendance, temp.remarks);
            break;
        }
    }
    fclose(fp);
}

// Function to fetch mentor details based on mentorID
void viewMyMentor(int mID) {
    FILE* fp = fopen("../data/mentor.dat", "rb");
    if (!fp) return;
    Mentor m;
    while (fread(&m, sizeof(Mentor), 1, fp)) {
        if (m.mentorID == mID) {
            printf("%s|%s|%s|%s|%s", m.name, m.department, m.designation, m.email, m.phone);
            break;
        }
    }
    fclose(fp);
}

// Function to request a meeting (Appends to meetings.dat)
void requestMeeting(int mentorID, int menteeID, char* topic, char* dt) {
    // 1. Get Mentee Name and Reg first
    FILE* fpM = fopen("../data/mentee.dat", "rb");
    Mentee temp;
    char mName[50] = "Unknown", mReg[20] = "";
    while (fread(&temp, sizeof(Mentee), 1, fpM)) {
        if (temp.menteeID == menteeID) {
            strcpy(mName, temp.name);
            strcpy(mReg, temp.regNo);
            break;
        }
    }
    fclose(fpM);

    // 2. Create Meeting object
    Meeting m;
    memset(&m, 0, sizeof(Meeting));
    srand(time(NULL));
    m.meetingID = (rand() % 9000) + 1000;
    m.mentorID = mentorID;
    m.menteeID = menteeID;
    strcpy(m.menteeName, mName);
    strcpy(m.menteeReg, mReg);
    strncpy(m.topic, topic, 99);
    strncpy(m.datetime, dt, 19);
    strcpy(m.status, "pending");

    // 3. Append to file
    FILE* fp = fopen("../data/meetings.dat", "ab"); // "ab" for Append Binary
    if (fp) {
        fwrite(&m, sizeof(Meeting), 1, fp);
        fclose(fp);
        printf("SUCCESS");
    } else {
        printf("FAILED");
    }
}

// MEETING LIST
void listMyMeetings(int menteeID) {
    FILE* fp = fopen("../data/meetings.dat", "rb");
    if (!fp) return;
    Meeting m;
    while (fread(&m, sizeof(Meeting), 1, fp)) {
        if (m.menteeID == menteeID) {
            printf("%d|%s|%s|%s\n", m.meetingID, m.topic, m.datetime, m.status);
        }
    }
    fclose(fp);
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;

    char* action = argv[1];

    if (strcmp(action, "login") == 0) {
        if (!menteeLogin(atoi(argv[2]), argv[3])) printf("0");
    } 
    else if (strcmp(action, "view_details") == 0) {
        viewMyDetails(atoi(argv[2]));
    }
    else if (strcmp(action, "view_mentor") == 0) {
        viewMyMentor(atoi(argv[2]));
    }
    else if (strcmp(action, "request_meeting") == 0) {
        requestMeeting(atoi(argv[2]), atoi(argv[3]), argv[4], argv[5]);
    }
    else if (strcmp(action, "list_meetings") == 0) {
        listMyMeetings(atoi(argv[2]));
    }

    return 0;
}