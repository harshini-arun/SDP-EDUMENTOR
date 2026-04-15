#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 3) return 1;

    char* action = argv[1];

    if (strcmp(action, "login") == 0) {
        char* user = argv[2];
        char* pass = argv[3];

        // For a first-year project, you can use a hardcoded admin 
        // OR read from a simple admin.txt file.
        if (strcmp(user, "admin") == 0 && strcmp(pass, "admin123") == 0) {
            printf("1"); // Success
        } else {
            printf("0"); // Fail
        }
    }
    return 0;
}