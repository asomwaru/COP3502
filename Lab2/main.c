#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 50

typedef struct student{
    char *lname;
    int assignment;
    int finalExam;
    int total;
    int *quizzes;
} student;

void release_memory(student ** st, int C, int N, int M);
student** readCourses(FILE *in, int *C,int *N, int *M);
void show_courses(student** courses, int C, int N, int M);
void best_student(student** courses, FILE* out, int C, int N, int M);

int main() {
    FILE* input;
    FILE* output;

    int C, N, M;

    student** students = readCourses(input, &C, &N, &M);
    show_courses(students, C, N, M);

    best_student(students, output, C, N, M);

    release_memory(students, C, N, M);
    return 0;
}

void release_memory(student ** st, int C, int N, int M) {
    for (int i=0; i<C; i++) {
        for (int j=0; j<N; j++) {
            free(st[i][j].lname);
            free(st[i][j].quizzes);
        }
        
        free(st[i]);
    }

    free(st);
}

student** readCourses(FILE *in, int *C,int *N, int *M) {
    in = fopen("in.txt", "r");

    fscanf(in, "%d %d %d", C, N, M);

    student** courses = (student**)calloc(*C, sizeof(student*));

    for (int i=0; i<*C; i++) {
        courses[i] = (student*)calloc(*N, sizeof(student));

        char temp_lname[SIZE];
        for (int j=0; j<*N; j++) {
            fscanf(in, "%s", temp_lname);

            int lname_size = strlen(temp_lname) + 1;
            courses[i][j].lname = (char*)malloc(sizeof(char) * lname_size);
            strcpy(courses[i][j].lname, temp_lname);

            fscanf(in, "%d", &courses[i][j].assignment);
            courses[i][j].total += courses[i][j].assignment;

            courses[i][j].quizzes = (int*)calloc(*M, sizeof(int));

            for (int k=0; k<*M; k++) {
                fscanf(in, "%d", &courses[i][j].quizzes[k]);
                courses[i][j].total += courses[i][j].quizzes[k];
            }

            fscanf(in, "%d", &courses[i][j].finalExam);
            courses[i][j].total += courses[i][j].finalExam;
        }
    }

    return courses;
}

void best_student(student** courses, FILE* out, int C, int N, int M) {
    out = fopen("out.txt", "w");

    int courseIndex = 0;
    student* bestStudent = NULL;

    for (int i=0; i<C; i++) {
        for (int j=0; j<N; j++) {
            if (bestStudent == NULL || courses[i][j].total > bestStudent->total) {
                bestStudent = &courses[i][j];
                courseIndex = i;
            }
        }
    }

    fprintf(out, "Name: %s\n", bestStudent->lname);
    fprintf(out, "Assignment: %d\n", bestStudent->assignment);

    fprintf(out, "Quizzes: ");
    for (int k=0; k<M-1; k++) {
        fprintf(out, "%d ", bestStudent->quizzes[k]);
    }

    fprintf(out, "%d\n", bestStudent->quizzes[M-1]);

    fprintf(out, "Final exam: %d\n", bestStudent->finalExam);
    fprintf(out, "Total: %d\n", bestStudent->total);
    fprintf(out, "Course Number: %d", courseIndex);
}

void show_courses(student** courses, int C, int N, int M) {
    for (int i=0; i<C; i++) {
        printf("Course #%d\n", i+1);

        for (int j=0; j<N; j++) {
            printf("Student: %s\n", courses[i][j].lname);
            printf("Assignment score: %d\n", courses[i][j].assignment);
            
            printf("Quizzes: ");
            for (int k=0; k<M; k++) {
                printf("%d ", courses[i][j].quizzes[k]);
            }

            printf("\nFinal Exam: %d\n", courses[i][j].finalExam);
            printf("Total grade: %d\n\n", courses[i][j].total);
        }

        printf("\n");
    }
}