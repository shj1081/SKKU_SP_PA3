#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 100
#define NUM_BUCKETS 1000000

typedef struct Node {
    char bigram[2 * MAX_WORD_LENGTH];
    int freq;
    struct Node* next;
} Node;

unsigned int hash(char* str) {
    unsigned int hash = 0;
    while (*str) {
        hash = hash * 31 + tolower(*str++);
    }
    return hash % NUM_BUCKETS;
}

Node* insert(Node* head, char* bigram) {
    Node* current = head;
    while (current != NULL) {
        if (strcmp(current->bigram, bigram) == 0) {
            current->freq++;
            return head;
        }
        current = current->next;
    }

    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->bigram, bigram);
    newNode->freq = 1;
    newNode->next = head;
    return newNode;
}

void printBigrams(Node* bucket[], int numBuckets) {
    for (int i = 0; i < numBuckets; i++) {
        Node* current = bucket[i];
        while (current != NULL) {
            printf("%s: %d\n", current->bigram, current->freq);
            current = current->next;
        }
    }
}

void toLowerCase(char* str) {
    while (*str) {
        *str = tolower(*str);
        str++;
    }
}

int main() {
    Node* buckets[NUM_BUCKETS] = {NULL};
    char word1[MAX_WORD_LENGTH], word2[MAX_WORD_LENGTH];
    FILE* file = fopen("text.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    if (fscanf(file, "%s", word1) != 1) return 0; // Read the first word
    toLowerCase(word1);

    while (fscanf(file, "%s", word2) == 1) {
        toLowerCase(word2);
        char bigram[2 * MAX_WORD_LENGTH];
        sprintf(bigram, "%s %s", word1, word2);
        unsigned int bucketIndex = hash(bigram);
        buckets[bucketIndex] = insert(buckets[bucketIndex], bigram);
        strcpy(word1, word2);
    }

    printBigrams(buckets, NUM_BUCKETS);
    fclose(file);
    return 0;
}
