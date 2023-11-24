#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEXT_FILE "shakespeare.txt"
#define MAX_WORD_COUNT 1000000
#define MAX_WORD_LENGTH 100
#define BUCKET_NUM 199999

/*
library functino wrapping
*/
int Strcmp(char *s1, char *s2) { return strcmp(s1, s2); }

void Strcpy(char *s1, char *s2) { strcpy(s1, s2); }

int Ispunct(int _c) { return ispunct(_c); }

void Qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *)) {
  qsort(base, nmemb, size, compar);
}

/*
Convert string to lowercase using pointer arithmetic and functinal programming
*/
void lower3(char *str) {
  for (; *str != '\0'; str++) {
    *str = (*str >= 'A' && *str <= 'Z') ? *str - ('A' - 'a') : *str;
  }
}

/*
Remove punctuation from string using pointer arithmetic and functinal
programming
*/
void removePunctuation(char *str) {
  char *noPunct = str;

  for (; *str != '\0'; str++) {
    *noPunct = (ispunct(*str)) ? *noPunct : *str;
    noPunct += (ispunct(*str)) ? 0 : 1;
  }

  *noPunct = '\0';
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
    removePunctuation(word);  // remove punctuation from word first
    lower3(word);             // convert word to lowercase second
    words[i] = (char *)malloc(MAX_WORD_LENGTH * sizeof(char));
    Strcpy(words[i], word);
    i++;
  }

  words[i] = NULL;  // set last element to NULL for checking end of array
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
Hash function for bigram nodes in hash table (djb2) using XOR and shift operator
*/
unsigned long hashFunction(char *word1, char *word2) {
  unsigned long hash = 5381;

  // hash = hash * 33 ^ ASCII(word)
  while (*word1) hash = ((hash << 5) + hash) + *word1++;
  while (*word2) hash = ((hash << 5) + hash) + *word2++;

  return hash;
}

/*
Insert bigram into hash table
*/
void insert2HashTable(Node **hashTable, char *word1, char *word2) {
  unsigned long bucketIndex = hashFunction(word1, word2) % BUCKET_NUM;
  if (hashTable[bucketIndex] == NULL) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    Strcpy(newNode->bigram[0], word1);
    Strcpy(newNode->bigram[1], word2);
    newNode->freq = 1;
    newNode->next = NULL;
    hashTable[bucketIndex] = newNode;
  } else {
    Node *current = hashTable[bucketIndex];
    while (current->next != NULL) {
      if (Strcmp(current->bigram[0], word1) == 0 &&
          Strcmp(current->bigram[1], word2) == 0) {
        current->freq++;
        return;
      }
      current = current->next;
    }
    if (Strcmp(current->bigram[0], word1) == 0 &&
        Strcmp(current->bigram[1], word2) == 0) {
      current->freq++;
      return;
    }
    Node *newNode = (Node *)malloc(sizeof(Node));
    Strcpy(newNode->bigram[0], word1);
    Strcpy(newNode->bigram[1], word2);
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

/*
main function
*/
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
  Qsort(nodeArray, nodeCount, sizeof(Node *), compareNodes);

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