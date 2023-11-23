#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXT_FILE "shakespeare.txt"
#define MAX_WORD_COUNT 1000000
#define MAX_WORD_LENGTH 100
#define BUCKET_NUM 199999

/*
Convert string to lowercase: slow
from textbook p.546
*/
void lower1(char *s) {
  int len = strlen(s);
  for (long i = 0; i < len; i++) {
    if (s[i] >= 'A' && s[i] <= 'Z') {
      s[i] -= ('A' - 'a');
    }
  }
}

/*
Remove punctuation from string
*/
void removePunctuation(char *str) {
  int i, j = 0;
  int len = strlen(str);
  char noPunct[len + 1];  // +1 for the null terminator

  for (i = 0; i < len; i++) {
    if (!ispunct(str[i])) {
      noPunct[j++] = str[i];
    }
  }
  noPunct[j] = '\0';  // Adding the null terminator to the end of the string

  // Copying the modified string back to the original string
  strcpy(str, noPunct);
}

/*
Read each word from the file and lower the case of each word
*/
char **readWords() {
  FILE *file = fopen(TEXT_FILE, "r");

  char **words = (char **)malloc(MAX_WORD_COUNT * sizeof(char *));
  char word[MAX_WORD_LENGTH];
  int i = 0;

  while (fscanf(file, "%99s", word) == 1) {
    lower1(word);
    removePunctuation(word);
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
  char bigram[2]
             [MAX_WORD_LENGTH];  // bigram[0] = word[i], bigram[1] = word[i+1]
  int freq;
  struct Node *next;
} Node;

/*
hash function that uses shift and exclusive-or operations
*/
unsigned long hashFunction(char *word1, char *word2) {
  unsigned long hash = 5381;

  while (*word1) hash = ((hash << 5) + hash) + *word1++; /* hash * 33 + c */
  while (*word2) hash = ((hash << 5) + hash) + *word2++; /* hash * 33 + c */

  return hash;
}

/*
Insert bigram into hash table
*/
void insert2HashTable(Node **hashTable, char *word1, char *word2) {
  unsigned long bucketIndex = hashFunction(word1, word2) % BUCKET_NUM;

  if (hashTable[bucketIndex] == NULL) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    strcpy(newNode->bigram[0], word1);
    strcpy(newNode->bigram[1], word2);
    newNode->freq = 1;
    newNode->next = NULL;
    hashTable[bucketIndex] = newNode;
  } else {
    Node *current = hashTable[bucketIndex];
    while (current->next != NULL) {
      if (strcmp(current->bigram[0], word1) == 0 &&
          strcmp(current->bigram[1], word2) == 0) {
        current->freq++;
        return;
      }
      current = current->next;
    }
    if (strcmp(current->bigram[0], word1) == 0 &&
        strcmp(current->bigram[1], word2) == 0) {
      current->freq++;
      return;
    }
    Node *newNode = (Node *)malloc(sizeof(Node));
    strcpy(newNode->bigram[0], word1);
    strcpy(newNode->bigram[1], word2);
    newNode->freq = 1;
    newNode->next = NULL;
    current->next = newNode;
  }
}

/*
complete hash table
*/
void completeHashTable(Node **hashTable, char **words) {
  for (int i = 0; i < MAX_WORD_COUNT && words[i + 1] != NULL; i++) {
    // bigram = "word[i] word[i+1]"
    insert2HashTable(hashTable, words[i], words[i + 1]);
  }
}

/*
Copy nodes to 1d array for sorting bigram nodes
*/
void node2Array(Node **hashTable, Node **nodeArray, int *nodeCount) {
  int index = 0;
  for (int i = 0; i < BUCKET_NUM; i++) {
    Node *current = hashTable[i];
    while (current != NULL) {
      nodeArray[index] = current;
      index++;
      current = current->next;
    }
  }
  *nodeCount = index;
}

/*
compare function for qsort
*/
int compareNodes(const void *a, const void *b) {
  const Node *nodeA = *(const Node **)a;
  const Node *nodeB = *(const Node **)b;
  return nodeB->freq - nodeA->freq;
}

int main() {
  // read words from file and store in to words array
  char **words = readWords();

  // turn words into bigram node and store in to hash table
  Node **hashTable = (Node **)malloc(sizeof(Node *) * BUCKET_NUM);
  completeHashTable(hashTable, words);

  // copy nodes from hash table to 1d array
  Node **nodeArray = (Node **)malloc(MAX_WORD_COUNT * sizeof(Node *));
  int nodeCount = 0;
  node2Array(hashTable, nodeArray, &nodeCount);

  // sort bigram nodes
  qsort(nodeArray, nodeCount, sizeof(Node *), compareNodes);

  // print out top 10 bigrams
  printf("< Top 10 bigrams >\n\n");
  for (int i = 0; i < 10; i++) {
    if (nodeArray[i] == NULL) {
      break;
    }
    printf("# %d - %s %s : %d\n", i, nodeArray[i]->bigram[0],
           nodeArray[i]->bigram[1], nodeArray[i]->freq);
  }

  return 0;
}