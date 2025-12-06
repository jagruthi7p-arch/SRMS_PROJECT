/* student_app.c -- Student Manager with student.txt + duplicate checks */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #define CHDIR _chdir
    #define GETCWD _getcwd
    #define strcasecmp _stricmp
#else
    #include <unistd.h>
    #define CHDIR chdir
    #define GETCWD getcwd
#endif

#define STUD_FILE "student.txt"
#define CRE_FILE  "credentials.txt"

#define MAX_USER 50
#define MAX_ROLE 20
#define MAX_LINE 512
#define MAX_NAME 100

char currentUser[MAX_USER];
char currentRole[MAX_ROLE];

char exePath[1024];
char exeDir[1024];

/* ------------------------ UTILITY FUNCTIONS ------------------------ */

void trim(char *s) {
    if (!s) return;
    char *start = s;
    while (*start && isspace((unsigned char)*start)) start++;
    if (*start == 0) { s[0] = 0; return; }
    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    end[1] = 0;
    if (start != s) memmove(s, start, strlen(start) + 1);
}

void removeBOMCRLF(char *s) {
    if (!s) return;
    unsigned char *u = (unsigned char*)s;
    if (u[0] == 0xEF && u[1] == 0xBB && u[2] == 0xBF)
        memmove(s, s+3, strlen(s+3)+1);

    size_t n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r'))
        s[--n] = '\0';
}

int get_line(char *buf, size_t size) {
    if (!fgets(buf, size, stdin)) return 0;
    removeBOMCRLF(buf);
    trim(buf);
    return 1;
}

void upper(char *s) {
    while (*s) { *s = toupper(*s); s++; }
}

/* ------------------------ FIX PATHS ------------------------ */

void fixPaths() {
#ifdef _WIN32
    DWORD len = GetModuleFileNameA(NULL, exePath, sizeof(exePath));
    exePath[len] = 0;
#else
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath)-1);
    exePath[len] = 0;
#endif

    strcpy(exeDir, exePath);
    for (int i = strlen(exeDir)-1; i >= 0; i--) {
        if (exeDir[i] == '\\' || exeDir[i] == '/') {
            exeDir[i] = 0;
            break;
        }
    }

    CHDIR(exeDir);
}

/* ------------------------ INPUT VALIDATION ------------------------ */

float getValidMarks() {
    char buf[100];
    float value;
    char extra;
    while (1) {
        printf("Marks: ");
        if (!fgets(buf, sizeof(buf), stdin)) continue;
        if (sscanf(buf, "%f %c", &value, &extra) == 1)
            return value;
        printf("❌ Only numbers allowed. Try again.\n");
    }
}

/* ------------------------ LOGIN SYSTEM ------------------------ */

int loginSystem() {
    char user[50], pass[50], line[256];

    printf("==== LOGIN ====\n");
    printf("Username: ");
    get_line(user, sizeof(user));
    printf("Password: ");
    get_line(pass, sizeof(pass));

    FILE *fp = fopen(CRE_FILE, "r");
    if (!fp) { printf("ERROR: Missing %s\n", CRE_FILE); return 0; }

    while (fgets(line, sizeof(line), fp)) {
        removeBOMCRLF(line);
        trim(line);
        if (line[0] == 0) continue;

        char *u = strtok(line, " \t:,");
        char *p = strtok(NULL, " \t:,");
        char *r = strtok(NULL, " \t:,");

        if (!u || !p) continue;
        if (!r) r = "GUEST";

        trim(u); trim(p); trim(r);
        upper(r);

        if (strcasecmp(user, u) == 0 && strcmp(pass, p) == 0) {
            strcpy(currentUser, u);
            strcpy(currentRole, r);
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

/* ------------------------ DUPLICATE CHECKS ------------------------ */

int rollExists(int roll) {
    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) return 0;
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), fp)) {
        removeBOMCRLF(line);
        int r = atoi(strtok(line, "|"));
        if (r == roll) { fclose(fp); return 1; }
    }
    fclose(fp);
    return 0;
}

int nameExists(const char *name) {
    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) return 0;

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp)) {
        removeBOMCRLF(line);
        strtok(line, "|");
        char *n = strtok(NULL, "|");

        if (n && strcasecmp(n, name) == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

/* ------------------------ STUDENT FUNCTIONS ------------------------ */

void addStudent() {
    int roll;
    char name[MAX_NAME];

    printf("Roll: ");
    scanf("%d", &roll);
    while (getchar() != '\n');

    if (rollExists(roll)) {
        printf("ERROR: Roll %d already exists.\n", roll);
        return;
    }

    printf("Name: ");
    get_line(name, sizeof(name));

    if (nameExists(name)) {
        printf("ERROR: Name already exists.\n");
        return;
    }

    float marks = getValidMarks();

    FILE *fp = fopen(STUD_FILE, "a");
    if (!fp) { printf("File write error.\n"); return; }

    fprintf(fp, "%d|%s|%.2f\n", roll, name, marks);
    fclose(fp);
    printf("Student added successfully.\n");
}

void displayStudents() {
    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) { printf("No records found.\n"); return; }

    char line[MAX_LINE];
    printf("\nROLL\tNAME\t\tMARKS\n----------------------------------\n");

    while (fgets(line, sizeof(line), fp)) {
        removeBOMCRLF(line);
        char *r = strtok(line, "|");
        char *n = strtok(NULL, "|");
        char *m = strtok(NULL, "|");

        if (r && n && m)
            printf("%s\t%-16s\t%s\n", r, n, m);
    }

    fclose(fp);
}

void searchStudent() {
    int target;
    printf("Enter roll: ");
    scanf("%d", &target);
    while (getchar() != '\n');

    FILE *fp = fopen(STUD_FILE, "r");
    if (!fp) { printf("No records.\n"); return; }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp)) {
        removeBOMCRLF(line);
        char *r = strtok(line, "|");
        char *n = strtok(NULL, "|");
        char *m = strtok(NULL, "|");

        if (atoi(r) == target) {
            printf("\nFOUND:\nRoll: %s\nName: %s\nMarks: %s\n", r, n, m);
            fclose(fp);
            return;
        }
    }

    printf("Record not found.\n");
    fclose(fp);
}

void updateStudent() {
    int target;
    char name[MAX_NAME];

    printf("Enter roll to update: ");
    scanf("%d", &target);
    while (getchar() != '\n');

    FILE *fp = fopen(STUD_FILE, "r");
    FILE *ft = fopen("temp.txt", "w");
    if (!fp || !ft) { printf("File error.\n"); return; }

    char line[MAX_LINE];
    int found = 0;

    while (fgets(line, sizeof(line), fp)) {
        removeBOMCRLF(line);

        char *r = strtok(line, "|");
        char *n = strtok(NULL, "|");
        char *m = strtok(NULL, "|");

        int roll = atoi(r);

        if (roll == target) {
            found = 1;
            printf("New name: ");
            get_line(name, sizeof(name));

            if (strcasecmp(name, n) != 0 && nameExists(name)) {
                printf("ERROR: Name already exists.\n");
                fclose(fp); fclose(ft);
                remove("temp.txt");
                return;
            }

            float marks = getValidMarks();
            fprintf(ft, "%d|%s|%.2f\n", roll, name, marks);
        } else {
            fprintf(ft, "%s|%s|%s\n", r, n, m);
        }
    }

    fclose(fp);
    fclose(ft);

    if (!found) {
        printf("Roll not found.\n");
        remove("temp.txt");
    } else {
        remove(STUD_FILE);
        rename("temp.txt", STUD_FILE);
        printf("Updated successfully.\n");
    }
}

void deleteStudent() {
    int target;
    printf("Enter roll to delete: ");
    scanf("%d", &target);
    while (getchar() != '\n');

    FILE *fp = fopen(STUD_FILE, "r");
    FILE *ft = fopen("temp.txt", "w");
    if (!fp || !ft) { printf("File error.\n"); return; }

    char line[MAX_LINE];
    int found = 0;

    while (fgets(line, sizeof(line), fp)) {
        removeBOMCRLF(line);
        char *r = strtok(line, "|");
        char *n = strtok(NULL, "|");
        char *m = strtok(NULL, "|");

        if (atoi(r) == target) {
            found = 1;
            continue;
        }

        fprintf(ft, "%s|%s|%s\n", r, n, m);
    }

    fclose(fp);
    fclose(ft);

    if (!found) {
        printf("Roll not found.\n");
        remove("temp.txt");
    } else {
        remove(STUD_FILE);
        rename("temp.txt", STUD_FILE);
        printf("Record deleted successfully.\n");
    }
}

/* ------------------------ MENUS ------------------------ */

void adminMenu() {
    int c;
    do {
        printf("\n==== ADMIN MENU ====\n"
               "1 Add Student\n"
               "2 Display Students\n"
               "3 Search Student\n"
               "4 Update Student\n"
               "5 Delete Student\n"
               "6 Logout\n"
               "Enter your choice: ");
        scanf("%d", &c);
        while (getchar()!='\n');

        switch(c) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
            case 5: deleteStudent(); break;
        }
    } while (c != 6);
}

void staffMenu() {
    int c;
    do {
        printf("\n==== STAFF MENU ====\n"
               "1 Add Student\n"
               "2 Display Students\n"
               "3 Search Student\n"
               "4 Update Student\n"
               "5 Logout\n"
               "Enter your choice: ");
        scanf("%d", &c);
        while (getchar()!='\n');

        switch(c) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
        }
    } while (c != 5);
}

void userMenu() {
    int c;
    do {
        printf("\n==== USER MENU ====\n"
               "1 Display Students\n"
               "2 Logout\n"
               "Enter your choice: ");
        scanf("%d", &c);
        while (getchar()!='\n');

        if (c == 1)
            displayStudents();

    } while (c != 2);
}

void guestMenu() {
    int c;
    do {
        printf("\n==== GUEST MENU ====\n"
               "1 Display Students\n"
               "2 Search Student\n"
               "3 Exit\n"
               "Enter your choice: ");
        scanf("%d", &c);
        while (getchar()!='\n');

        if (c == 1) displayStudents();
        else if (c == 2) searchStudent();

    } while (c != 3);
}

void mainMenu() {
    if (strcmp(currentRole, "ADMIN") == 0) adminMenu();
    else if (strcmp(currentRole, "STAFF") == 0) staffMenu();
    else if (strcmp(currentRole, "USER") == 0) userMenu();
    else guestMenu();
}

/* ------------------------ MAIN ------------------------ */

int main() {
    fixPaths();

    if (loginSystem()) {
    mainMenu();
} else {
    printf("Invalid login.\n");
}

    return 0;
}
