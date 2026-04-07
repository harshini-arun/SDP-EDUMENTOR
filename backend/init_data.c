#include <stdio.h>
#include <string.h>
#include "structures.h"

int main() {
    // 1. Open the file in "wb" (Write Binary) mode
    // This will create the file if it doesn't exist, or overwrite it if it does
    FILE *fp = fopen("../data/mentor.dat", "wb");

    if (fp == NULL) {
        printf("Error: Could not create data folder or file. Make sure the 'data' folder exists.\n");
        return 1;
    }

    // 2. Create sample mentor data
    Mentor m1;
    m1.mentorID = 101;
    strcpy(m1.name, "Dr. Rajesh Kumar");
    strcpy(m1.department, "Information Technology");
    strcpy(m1.designation, "Senior Professor");
    strcpy(m1.email, "rajesh.k@university.edu");
    strcpy(m1.phone, "9876543210");
    strcpy(m1.password, "admin"); // This is the password for login

    // Create a second mentor for testing
    Mentor m2;
    m2.mentorID = 102;
    strcpy(m2.name, "Dr. Sarah Jones");
    strcpy(m2.department, "Computer Science");
    strcpy(m2.designation, "Assistant Professor");
    strcpy(m2.email, "sarah.j@university.edu");
    strcpy(m2.phone, "9123456789");
    strcpy(m2.password, "mentor123");

    // 3. Write the structs to the binary file
    fwrite(&m1, sizeof(Mentor), 1, fp);
    fwrite(&m2, sizeof(Mentor), 1, fp);

    // 4. Close the file
    fclose(fp);

    printf("Database initialized successfully!\n");
    printf("Mentor 1: ID 101, Pass: admin\n");
    printf("Mentor 2: ID 102, Pass: mentor123\n");

    return 0;
}