#include <stdio.h>
#include <string.h>
#include "structures.h"

// Helper function to simplify filling mentor data
void fillMentor(Mentor *m, int id, char* name, char* dept, char* sec, char* desig, char* email, char* phone, char* pass) {
    m->mentorID = id;
    strcpy(m->name, name);
    strcpy(m->department, dept);
    strcpy(m->section, sec);
    strcpy(m->designation, desig);
    strcpy(m->email, email);
    strcpy(m->phone, phone);
    strcpy(m->password, pass);
}

int main() {
    FILE *fp = fopen("../data/mentor.dat", "wb");
    if (fp == NULL) {
        printf("Error: Could not open file for writing.\n");
        return 1;
    }

    Mentor mentors[8];

    // --- DEPARTMENT: IT ---
    // Section A
    fillMentor(&mentors[0], 101, "Dr. Rajesh Kumar", "IT", "A", "Professor", "rajesh@univ.edu", "9876543210", "rajesh123");
    fillMentor(&mentors[1], 102, "Dr. Sarah Jones", "IT", "A", "Asst Professor", "sarah@univ.edu", "9876543211", "sarah123");
    
    // Section B
    fillMentor(&mentors[2], 103, "Dr. Abdul Shah", "IT", "B", "Asst Professor", "abdul@univ.edu", "9876543212", "abdul123");
    fillMentor(&mentors[3], 104, "Dr. Priya Sharma", "IT", "B", "Lecturer", "priya@univ.edu", "9876543213", "priya123");

    // --- DEPARTMENT: CSE ---
    // Section A
    fillMentor(&mentors[4], 105, "Dr. John Smith", "CSE", "A", "Professor", "john@univ.edu", "9876543214", "john123");
    fillMentor(&mentors[5], 106, "Dr. Emily Davis", "CSE", "A", "Asst Professor", "emily@univ.edu", "9876543215", "emily123");

    // Section B
    fillMentor(&mentors[6], 107, "Dr. Michael Chen", "CSE", "B", "Professor", "mike@univ.edu", "9876543216", "mike123");
    fillMentor(&mentors[7], 108, "Dr. Ananya Iyer", "CSE", "B", "Asst Professor", "ananya@univ.edu", "9876543217", "ananya123");

    // 3. Write all 8 mentors at once
    fwrite(mentors, sizeof(Mentor), 8, fp);

    fclose(fp);

    printf("Database Initialized with 8 Mentors!\n");
    printf("IT Section A: 101, 102\n");
    printf("IT Section B: 103, 104\n");
    printf("CSE Section A: 105, 106\n");
    printf("CSE Section B: 107, 108\n");

    return 0;
}