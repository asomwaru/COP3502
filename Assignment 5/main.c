#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"
#define MAX_LEN 30000

typedef struct trie {
    int count;

    struct trie* next[26];
} trie;


trie* init(int count);
int isEmpty(struct trie* root);
void insert(trie* tree, char word[], int k, int count);
void free_dict(struct trie* tree);
trie* search(trie* tree, char word[], int k);
char* biggest_character(trie* node);


int main() {
    atexit(report_mem_leak);
    FILE* in = fopen("in2.txt", "r");
    FILE* out = fopen("out.txt", "w");

    int queries, command;
    char string[MAX_LEN];

    fscanf(in, "%d", &queries);

    trie* root = init(-1);

    for (int i=0; i<queries; i++) {
        fscanf(in, "%d", &command);

        if (command == 1) {
            int count;

            fscanf(in, "%s %d", string, &count);

            int index = string[0] - 'a';
            insert(root, string, 0, count);

        } else {

            fscanf(in, "%s", string);
            trie* node = search(root, string, 0);

            if (node == NULL || isEmpty(node)) {
                fprintf(out, "unknown word\n");

            } else {
                char* temp = biggest_character(node);

                fprintf(out, "%s\n", temp);

                free(temp);
            }

        }
    }

    free_dict(root);
    
    fclose(in);
    fclose(out);
    return 0;
}


trie* init(int count) {
    trie* node = (trie*)malloc(sizeof(trie));

    for (int i=0; i<26; i++) {
        node->next[i] = NULL;
    }
    
    node->count = count;

    return node;
}

int isEmpty(struct trie* root) {
    for (int i = 0; i < 26; i++) {
        if (root->next[i] != NULL) {
            return 0;
        }
    }

    return 1;
}

void insert(trie* tree, char word[], int k, int count) {
    if (k == strlen(word)) {
        return;
    }

    int nextIndex = word[k] - 'a';

    if (tree->next[nextIndex] == NULL) {
        tree->next[nextIndex] = init(count);
    } else {
        tree->next[nextIndex]->count += count;
    }

    insert(tree->next[nextIndex], word, k+1, count);
}

trie* search(trie* tree, char word[], int k) {
    if (tree == NULL) {
        return NULL;
    }

    int index = word[k] - 'a';
    if (k == strlen(word)) {
        return tree;
    }

    return search(tree->next[index], word, k+1);
}

char* biggest_character(trie* node) {
    char* string = (char*)malloc(sizeof(char) * 26+1);

    int biggest = -1;

    for (int i=0; i<26; i++) {
        if (node->next[i] == NULL) {
            continue;
        }

        if (biggest == -1) {
            biggest = i;

        } else if (node->next[i]->count > node->next[biggest]->count) {
            biggest = i;
        }
    }

    for (int i=0; i<26; i++) {
        if (node->next[i] == NULL) {
            continue;
        }

        if (node->next[i]->count == node->next[biggest]->count) {
            char next = i+'a';
            strncat(string, &next, 1);
        }
    }

    return string;
}

void free_dict(struct trie* tree) {
    for (int i=0; i<26; i++) {
        if (tree->next[i] != NULL) {
            free_dict(tree->next[i]);
        }
    }

    free(tree);
}
