void insert2HashTable(Node **hashTable, char *word1, char *word2) {
  unsigned long bucketIndex = hashFunction(word1, word2) % BUCKET_NUM;
  Node *newNode = (Node *)malloc(sizeof(Node));
  strcpy(newNode->bigram[0], word1);
  strcpy(newNode->bigram[1], word2);
  newNode->freq = 1;
  newNode->next = NULL;

  while (hashTable[bucketIndex] != NULL) {
    if (Strcmp(hashTable[bucketIndex]->bigram[0], word1) == 0 &&
        Strcmp(hashTable[bucketIndex]->bigram[1], word2) == 0) {
      hashTable[bucketIndex]->freq++;
      free(newNode);
      return;
    }
    bucketIndex = (bucketIndex + 1) % BUCKET_NUM;
  }

  hashTable[bucketIndex] = newNode;
}