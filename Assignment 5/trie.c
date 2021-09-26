// Arup Guha
// 11/5/2015
// An example trie (just insertion) written in class (COP 3502)
// later modified and added more functions by Tanvir Ahmed

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct trie {
    int isWord;//0/1
    struct trie* next[26];
};

struct trie* init();
void insert(struct trie* tree, char word[], int k);
int search(struct trie* tree, char word[], int k) ;
void printAll(struct trie* tree, char cur[]) ;
struct trie* del(struct trie* root, char key[], int depth);
int isEmpty(struct trie* root);

void freeDictionary(struct trie* tree);

int main() {

    struct trie* myDictionary = init();

    // Read in number of words.
    int i, n;
    FILE* ifp = fopen("dictionary.txt", "r");
    fscanf(ifp, "%d", &n);

    // Read in each word and insert it.
    for (i=0; i<n; i++) {
        char word[100];
        fscanf(ifp, "%s", word);
        insert(myDictionary, word, 0);
        //debug code to trace which word is being processed for large list of words
       // if (i%10000 == 0) printf("%d word is %s\n", i, word);
    }

    char searchKey[100];
    printf("Enter a word to search: ");
    scanf("%s", searchKey);

    if (search(myDictionary, searchKey, 0))
            printf("Found %s\n", searchKey);
    else
        printf("Not found %s\n", searchKey);


    //Printing all the words!
    printf("Printing all the words: \n");
    char word[100];
    word[0] = '\0';
    printAll(myDictionary, word);

    char deleteKey[50];

    printf("Enter a word to remove: ");
    scanf("%s", deleteKey);

    myDictionary= del(myDictionary, deleteKey, 0);

    /*for(int i=0; i<26; i++)
    {
        if(myDictionary->next[i])
            printf("--%c--\n", 97+i);
    }*/
    word[0] = '\0';

    printAll(myDictionary, word);


    // Clean up.
    fclose(ifp);
    freeDictionary(myDictionary);
    return 0;
}

struct trie* init() {

    // Create the struct, not a word.
    struct trie* myTree = malloc(sizeof(struct trie));
    myTree->isWord = 0;

    // Set each pointer to NULLL.
    int i;
    for (i=0; i<26; i++)
        myTree->next[i] = NULL;

    // Return a pointer to the new root.
    return myTree;
}

//checks if all the 26 childners are NULL or not.
int isEmpty(struct trie* root)
{
    for (int i = 0; i < 26; i++)
        if (root->next[i])
            return 0;
    return 1;
}


// Recursive function to delete a key from given Trie
struct trie* del(struct trie* root, char key[], int k)
{
    // If tree is empty
    if (!root)
        return NULL;

    // If last character of key is being processed
    if (k == strlen(key)) {

        // This node is no more end of word after
        // removal of given key
        if (root->isWord) //if it is a valid word
            root->isWord = 0; //mark it as not word anymore

        // If it is not prefix of any other word
        if (isEmpty(root)) {
            free (root);
            root = NULL;
        }

        return root;
    }

    // If not last character, recur for the child
    int index = key[k] - 'a';
    root->next[index] = del(root->next[index], key, k + 1);

    // If root does not have any child (its only child got
    // deleted), and it is not end of another word.
    if (isEmpty(root) && root->isWord == 0) {
        free (root);
        root = NULL;
    }

    return root;
}

void insert(struct trie* tree, char word[], int k) { //  cat

    // Down to the end, insert the word.
    if (k == strlen(word)) {
        tree->isWord = 1;
        return;
    }

    // See if the next place to go exists, if not, create it.
    int nextIndex = word[k] - 'a';
    if (tree->next[nextIndex] == NULL)
        tree->next[nextIndex] = init();

    insert(tree->next[nextIndex], word, k+1);
}

int search(struct trie* tree, char word[], int k) {

    // Down to the end, insert the word.
    if (k == strlen(word))
        return tree->isWord;

    // If the next place doesn't exist, word is not a word.
    int nextIndex = word[k] - 'a';
    if (tree->next[nextIndex] == NULL)
        return 0;

    return search(tree->next[nextIndex], word, k+1);
}

// Just frees all the memory pointed to by tree (directly and indirectly)
void freeDictionary(struct trie* tree) {

    int i;
    for (i=0; i<26; i++)
        if (tree->next[i] != NULL)
            freeDictionary(tree->next[i]);

    free(tree);
}

// Prints all words stored in the trie in alphabetical order.
void printAll(struct trie* tree, char cur[]) {

    // Stop!
    if (tree == NULL) return;

    // Print this node, if it's a word.
    if (tree->isWord)
        printf("%s\n", cur);

    // Safer if we store this.
    int len = strlen(cur);

    // Recursively print all words in each subtree,
    // in alpha order.
    int i;
    for (i=0; i<26; i++) {
        cur[len] = (char)('a'+i);
        cur[len+1] = '\0';
        printAll(tree->next[i], cur);
    }
}
