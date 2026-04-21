#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"

/* 
   ==========================================================
   ADMIN MODULE - BINARY SEARCH TREE (BST) IMPLEMENTATION
   ==========================================================
   Key: mentorID
   Purpose: Organize and search mentors efficiently.
*/

// BST Node Structure
typedef struct TreeNode {
    Mentor data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

// Function to create a new tree node
TreeNode* createNode(Mentor m) {
    TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
    if (newNode == NULL) return NULL;
    newNode->data = m;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// 1. BST INSERTION
// Logic: If ID is smaller, go left. If larger, go right.
TreeNode* insertMentor(TreeNode* root, Mentor m) {
    if (root == NULL) {
        return createNode(m);
    }

    if (m.mentorID < root->data.mentorID) {
        root->left = insertMentor(root->left, m);
    } else if (m.mentorID > root->data.mentorID) {
        root->right = insertMentor(root->right, m);
    }
    return root;
}

// 2. BST INORDER TRAVERSAL
// Logic: Left -> Root -> Right (This gives us a sorted list by ID)
void inorderTraversal(TreeNode* root) {
    if (root != NULL) {
        inorderTraversal(root->left);
        // Print in pipe-separated format for Python to parse
        printf("%d|%s|%s|%s|%s|%s\n", 
               root->data.mentorID, 
               root->data.name, 
               root->data.department, 
               root->data.designation, 
               root->data.email, 
               root->data.phone);
        inorderTraversal(root->right);
    }
}

// 3. BST SEARCH
// Logic: Efficiently find a specific mentor without looking at every record
TreeNode* searchMentor(TreeNode* root, int id) {
    if (root == NULL || root->data.mentorID == id) {
        return root;
    }
    
    if (id < root->data.mentorID) {
        return searchMentor(root->left, id);
    }
    return searchMentor(root->right, id);
}

// HELPER: Load all mentors from mentor.dat into the BST
TreeNode* loadMentorsToBST() {
    FILE *fp = fopen("../data/mentor.dat", "rb");
    if (!fp) return NULL;

    TreeNode* root = NULL;
    Mentor temp;
    // Read every mentor record from binary file
    while (fread(&temp, sizeof(Mentor), 1, fp)) {
        root = insertMentor(root, temp);
    }
    fclose(fp);
    return root;
}

// HELPER: Get counts for the dashboard statistics
void getStats() {
    int mentorCount = 0;
    int menteeCount = 0;
    
    FILE *fp1 = fopen("../data/mentor.dat", "rb");
    FILE *fp2 = fopen("../data/mentee.dat", "rb");
    
    Mentor m; 
    Mentee s;
    
    if(fp1) {
        while(fread(&m, sizeof(Mentor), 1, fp1)) mentorCount++;
        fclose(fp1);
    }
    
    if(fp2) {
        while(fread(&s, sizeof(Mentee), 1, fp2)) menteeCount++;
        fclose(fp2);
    }
    
    // Output format: mentorCount|menteeCount
    printf("%d|%d", mentorCount, menteeCount);
}

void calculateGlobalReport() {
    FILE *fp = fopen("../data/mentee.dat", "rb");
    if (!fp) { printf("0|0|0"); return; }

    Mentee m;
    int count = 0;
    float totalCGPA = 0, totalAttn = 0;

    while (fread(&m, sizeof(Mentee), 1, fp)) {
        totalCGPA += m.cgpa;
        totalAttn += m.attendance;
        count++;
    }
    fclose(fp);

    if (count == 0) printf("0|0|0");
    else printf("%d|%.2f|%.2f", count, totalCGPA/count, totalAttn/count);
}

void calculateMentorReport(int mid) {
    FILE *fp = fopen("../data/mentee.dat", "rb");
    if (!fp) { printf("0|0|0"); return; }

    Mentee m;
    int count = 0;
    float totalCGPA = 0, totalAttn = 0;

    while (fread(&m, sizeof(Mentee), 1, fp)) {
        if (m.mentorID == mid) {
            totalCGPA += m.cgpa;
            totalAttn += m.attendance;
            count++;
        }
    }
    fclose(fp);

    if (count == 0) printf("0|0|0");
    else printf("%d|%.2f|%.2f", count, totalCGPA/count, totalAttn/count);
}

/* 
   ==========================================================
   MAIN FUNCTION - ROUTING COMMANDS FROM FLASK
   ==========================================================
*/
int main(int argc, char *argv[]) {
    // We need at least the 'action' argument
    if (argc < 2) {
        return 1;
    }

    char *action = argv[1];

    // ADMIN LOGIN
    if (strcmp(action, "login") == 0) {
        if (argc < 4) return 1;
        char *username = argv[2];
        char *password = argv[3];
        
        // Simple authentication: user: admin, pass: admin123
        if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
            printf("1"); // Success
        } else {
            printf("0"); // Failure
        }
    } 

    // LIST ALL MENTORS (Sorted via BST Inorder)
    else if (strcmp(action, "list_mentors") == 0) {
        TreeNode* root = loadMentorsToBST();
        if (root == NULL) {
            return 0;
        }
        inorderTraversal(root);
    } 

    // SEARCH FOR A SPECIFIC MENTOR (BST Search)
    else if (strcmp(action, "search_mentor") == 0) {
        if (argc < 3) return 1;
        int searchID = atoi(argv[2]);
        TreeNode* root = loadMentorsToBST();
        TreeNode* result = searchMentor(root, searchID);
        
        if (result) {
            printf("%d|%s|%s|%s|%s|%s", 
                   result->data.mentorID, 
                   result->data.name, 
                   result->data.department, 
                   result->data.designation, 
                   result->data.email, 
                   result->data.phone);
        } else {
            printf("0"); // Not found
        }
    }

    // REPORTS
    else if (strcmp(action, "global_report") == 0) {
        calculateGlobalReport();
    }
    else if (strcmp(action, "mentor_report") == 0) {
        calculateMentorReport(atoi(argv[2]));
    }

    // SYSTEM STATISTICS
    else if (strcmp(action, "stats") == 0) {
        getStats();
    }

    return 0;
}