#ifndef ENGINE_H_
#define ENGINE_H_
#include <stdio.h>

//hashtable
typedef struct HTable {
    struct Node* root;
    int size;
} linked;

//node
struct Node {
    char* text;
    char* documentID;
    int count;
    struct Node* next;
};

void initializeTable(int bNum);
struct Node* create_new_node(char* word, char* docID);
int getCount(char* word, char* docID, int bNum);
void readFile(FILE* doc, char* docID, int buckets);
int contains(int bNum, char* word, char* docID);
int hash_code(char* word, int buckets);
void hash_table_insert(struct Node* insertNode, int bNum);
int find(int bNum, char* word, char* docID);
void hash_table_removeatPosition(int bNum, int i);
void hash_table_remove(int bNum, char* word);
int findPosition(int bNum, char* word, char* docID);
int num_of_removes(int bNum, char* word);
extern struct HTable** table;
extern const int MAX_LINE_LENGTH;

#endif
