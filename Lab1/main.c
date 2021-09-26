#include <stdio.h>
#include <stdlib.h>

typedef struct Student {
    int id;
    int g1, g2, g3;
    float average;
} Student;

int main() {
    FILE* input = fopen("students.txt", "r");
    FILE* output = fopen("out.txt", "w");
    int n;

    fscanf(input, "%d", &n);
    Student *students = (Student*)malloc(sizeof(Student) * n);

    for (int i=0; i<n; i++) {
        fscanf(input, "%d", &students[i].id);

        fscanf(input, "%d %d %d", &students[i].g1, &students[i].g2, &students[i].g3);

        students[i].average = (students[i].g1 + students[i].g2 + students[i].g3)/3.0;

        fprintf(output, "%d %d %d %d %.2f\n", students[i].id, students[i].g1, students[i].g2, students[i].g3, students[i].average);
    }

    Student *bestStudent = &students[0];

    for (int i=0; i<n; i++) {
        if (students[i].average > bestStudent->average) {
            bestStudent = &students[i];
        }
    }
    
    fprintf(output, "\nMaximum Average is %.2f and the student is %d", bestStudent->average, bestStudent->id);

    return 0;
}