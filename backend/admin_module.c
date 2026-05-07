#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"

/* 
   ==========================================================
   ADMIN MODULE - HIERARCHICAL BINARY TREE
   ==========================================================
   Structure:
   Level 0: ROOT
   Level 1: Dept (Left: IT, Right: CSE)
   Level 2: Section (Left: A, Right: B)
   Level 3: Mentors (Leaf Nodes)
*/

typedef struct HierarchyNode {
    char label[50];          // Stores Dept/Section names
    int isLeaf;              // 1 if Mentor node, 0 if Branch node
    Mentor mentorData;       // Only populated if isLeaf == 1
    struct HierarchyNode *left;
    struct HierarchyNode *right;
} HierarchyNode;

// Create a branch node (Dept, Section, or Root)
HierarchyNode* createBranchNode(char* label) {
    HierarchyNode* node = (HierarchyNode*)malloc(sizeof(HierarchyNode));
    strcpy(node->label, label);
    node->isLeaf = 0;
    node->left = node->right = NULL;
    return node;
}

// Create a leaf node (Mentor)
HierarchyNode* createLeafNode(Mentor m) {
    HierarchyNode* node = (HierarchyNode*)malloc(sizeof(HierarchyNode));
    node->isLeaf = 1;
    node->mentorData = m;
    node->left = node->right = NULL;
    return node;
}

// Insert mentor into the Hierarchical Binary Tree based on Dept and Section
// Note: We use mentorID parity (even/odd) or ID range to simulate Sections A/B
HierarchyNode* insertIntoHierarchy(HierarchyNode* root, Mentor m) {
    if (root == NULL) root = createBranchNode("ROOT");

    // Level 1: Department (Left = IT, Right = CSE)
    HierarchyNode** deptPointer;
    if (strstr(m.department, "IT") || strstr(m.department, "Information")) {
        if (!root->left) root->left = createBranchNode("IT");
        deptPointer = &(root->left);
    } else {
        if (!root->right) root->right = createBranchNode("CSE");
        deptPointer = &(root->right);
    }

    // Level 2: Section (Left = A, Right = B)
    HierarchyNode** sectionPointer;
    if (strcmp(m.section, "A") == 0 || strcmp(m.section, "a") == 0) {
        if (!(*deptPointer)->left) (*deptPointer)->left = createBranchNode("Section A");
        sectionPointer = &((*deptPointer)->left);
    } else {
        if (!(*deptPointer)->right) (*deptPointer)->right = createBranchNode("Section B");
        sectionPointer = &((*deptPointer)->right);
    }

    // Level 3: Mentor (Binary Tree Leaf)
    if ((*sectionPointer)->left == NULL) {
        (*sectionPointer)->left = createLeafNode(m);
    } else {
        (*sectionPointer)->right = createLeafNode(m);
    }

    return root;
}

// Traverse and print only Leaf Nodes (Mentors) for Flask compatibility
void traverseHierarchy(HierarchyNode* root) {
    if (root == NULL) return;

    // Inorder: Left, Root (if leaf), Right
    traverseHierarchy(root->left);
    
    if (root->isLeaf) {
        printf("%d|%s|%s|%s|%s|%s\n", 
               root->mentorData.mentorID, 
               root->mentorData.name, 
               root->mentorData.department, 
               root->mentorData.designation, 
               root->mentorData.email, 
               root->mentorData.phone);
    }
    
    traverseHierarchy(root->right);
}

// Search hierarchy for a specific Mentor ID
HierarchyNode* searchHierarchy(HierarchyNode* root, int id) {
    if (root == NULL) return NULL;
    if (root->isLeaf && root->mentorData.mentorID == id) return root;

    HierarchyNode* leftRes = searchHierarchy(root->left, id);
    if (leftRes) return leftRes;
    
    return searchHierarchy(root->right, id);
}

// Helper: Build the tree from the binary file
HierarchyNode* buildTreeFromFile() {
    FILE *fp = fopen("../data/mentor.dat", "rb");
    if (!fp) return NULL;

    HierarchyNode* root = createBranchNode("ROOT");
    Mentor temp;
    while (fread(&temp, sizeof(Mentor), 1, fp)) {
        insertIntoHierarchy(root, temp);
    }
    fclose(fp);
    return root;
}

// Function for global stats (Count logic remains the same)
void getStats() {
    int mCount = 0, sCount = 0;
    FILE *f1 = fopen("../data/mentor.dat", "rb");
    FILE *f2 = fopen("../data/mentee.dat", "rb");
    Mentor m; Mentee s;
    if(f1){ while(fread(&m, sizeof(Mentor), 1, f1)) mCount++; fclose(f1); }
    if(f2){ while(fread(&s, sizeof(Mentee), 1, f2)) sCount++; fclose(f2); }
    printf("%d|%d", mCount, sCount);
}

// Global/Mentor report logic (Unchanged to ensure Flask compatibility)
void calculateGlobalReport() {
    FILE *fp = fopen("../data/mentee.dat", "rb");
    if (!fp) { printf("0|0|0"); return; }
    Mentee m; int count = 0; float gpa = 0, att = 0;
    while (fread(&m, sizeof(Mentee), 1, fp)) { gpa += m.cgpa; att += m.attendance; count++; }
    fclose(fp);
    if (count == 0) printf("0|0|0");
    else printf("%d|%.2f|%.2f", count, gpa/count, att/count);
}

void calculateMentorReport(int mid) {
    FILE *fp = fopen("../data/mentee.dat", "rb");
    if (!fp) { printf("0|0|0"); return; }
    Mentee m; int count = 0; float gpa = 0, att = 0;
    while (fread(&m, sizeof(Mentee), 1, fp)) {
        if (m.mentorID == mid) { gpa += m.cgpa; att += m.attendance; count++; }
    }
    fclose(fp);
    if (count == 0) printf("0|0|0");
    else printf("%d|%.2f|%.2f", count, gpa/count, att/count);
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    char *action = argv[1];

    if (strcmp(action, "login") == 0) {
        if (strcmp(argv[2], "admin") == 0 && strcmp(argv[3], "admin123") == 0) printf("1");
        else printf("0");
    } 
    else if (strcmp(action, "list_mentors") == 0) {
        HierarchyNode* root = buildTreeFromFile();
        traverseHierarchy(root);
    } 
    else if (strcmp(action, "search_mentor") == 0) {
        int id = atoi(argv[2]);
        HierarchyNode* root = buildTreeFromFile();
        HierarchyNode* res = searchHierarchy(root, id);
        if (res) printf("%d|%s|%s|%s|%s|%s", res->mentorData.mentorID, res->mentorData.name, res->mentorData.department, res->mentorData.designation, res->mentorData.email, res->mentorData.phone);
        else printf("0");
    }
    else if (strcmp(action, "stats") == 0) getStats();
    else if (strcmp(action, "global_report") == 0) calculateGlobalReport();
    else if (strcmp(action, "mentor_report") == 0) calculateMentorReport(atoi(argv[2]));
    else if (strcmp(action, "list_depts") == 0) {
        // Level 1: Root children
        printf("IT|CSE"); 
    } 
    else if (strcmp(action, "list_sections") == 0) {
        // Level 2: Dept children
        printf("Section A|Section B");
    }
    else if (strcmp(action, "list_by_hierarchy") == 0) {
        // Level 3: Get mentors in a specific Section of a specific Dept
        char* targetDept = argv[2];
        char* targetSection = argv[3];
        HierarchyNode* root = buildTreeFromFile();
        
        // Navigate manually: Root -> Dept -> Section
        HierarchyNode* deptNode = (strstr(targetDept, "IT")) ? root->left : root->right;
        if (!deptNode) return 0;

        HierarchyNode* sectionNode = (strstr(targetSection, "A")) ? deptNode->left : deptNode->right;
        if (!sectionNode) return 0;

        // Print mentors in this leaf
        traverseHierarchy(sectionNode);
    }

    return 0;
}