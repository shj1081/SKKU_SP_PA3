#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD_COUNT 1000000
#define MAX_WORD_LENGTH 100
#define BUCKET_NUM 1000000

/*
Each word is read from the file and converted to lowercase. Our initial
version used the function lower1 (Figure 5.7), which we know to have
quadratic run time due to repeated calls to strlen
*/

/*
Convert string to lowercase: slow
from textbook p.546
*/
void lower1(char *s) {
  long i;
  for (i = 0; i < strlen(s); i++) {
    if (s[i] >= 'A' && s[i] <= 'Z') {
      s[i] -= ('A' - 'a');
    }
  }
}

/*
Read each word from the file and lower the case of each word
*/
char **readWords() {
  FILE *file = fopen("shakespeare.txt", "r");

  char **words = (char **)malloc(MAX_WORD_COUNT * sizeof(char *));
  char word[MAX_WORD_LENGTH];
  int i = 0;

  while (fscanf(file, "%99s", word) == 1) {
    lower1(word);
    words[i] = (char *)malloc(MAX_WORD_LENGTH * sizeof(char));
    strcpy(words[i], word);
    i++;
  }

  words[i] = NULL;  // NULL-terminate the array
  fclose(file);

  return words;
}

/*
Node structure of bigrams for linked list of hash table
*/
typedef struct Node {
  char bigram[2 * MAX_WORD_LENGTH];
  int freq;
  struct Node *next;
} Node;

/*
Hash function that sums the ASCII codes for the characters modulo s
*/
int hashFunction(char *s) {
  int sum = 0;
  for (int i = 0; s[i] != '\0'; i++) {
    sum += (int)s[i];
  }
  return sum % BUCKET_NUM;
}

/*
Insert bigram into hash table
*/
Node *insert2HashTable(Node *head, char *bigram) {
  Node *current = head;
  // Traverse the linked list to find the bigram
  while (current != NULL) {
    if (strcmp(current->bigram, bigram) == 0) {
      current->freq++;
      return head;
    }
    current = current->next;
  }

  // If the bigram is not found,
  // create a new node and insert it at the head of the linked list
  Node *newNode = (Node *)malloc(sizeof(Node));
  strcpy(newNode->bigram, bigram);
  newNode->freq = 1;
  newNode->next = head;
  return newNode;
}

/*
Insertion sort
*/
void insertionSort(Node *head) {
  Node *current = head;
  Node *sorted = NULL;
  Node *next;
  while (current != NULL) {
    next = current->next;
    sortedInsert(&sorted, current);
    current = next;
  }
  head = sorted;
}


int main() {
  char **words = readWords();
  if (words == NULL) {
    return 1;
  }

  char bigram[2 * MAX_WORD_LENGTH];
  int bucketIndex;
  Node *hashTable[BUCKET_NUM] = {NULL};

  for (int i = 0; i < MAX_WORD_COUNT && words[i + 1] != NULL; i++) {
    // bigram = "word[i] word[i+1]"
    strcpy(bigram, words[i]);
    strcat(bigram, " ");
    strcat(bigram, words[i + 1]);
    strcat(bigram, "\0");
    bucketIndex = hashFunction(bigram);
    hashTable[bucketIndex] = insert2HashTable(hashTable[bucketIndex], bigram);
  }

  Node* sortResult[MAX_WORD_COUNT];

  return 0;
}
