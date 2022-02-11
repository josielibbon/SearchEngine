#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include <ctype.h>

struct HTable** table; //table
const int MAX_LINE_LENGTH = 1000; //max line length in the doc

//make the hashtable
void initializeTable(int bucketNum) {
  
  int k;
  table = malloc(sizeof(struct HTable*) * bucketNum);
  
  for (k = 0; k < bucketNum; k++) {        
      //allocate space for table                 
      table[k] = malloc(sizeof(struct HTable));
      //allocate space for root node
      table[k]->root = malloc(sizeof(struct Node));   
      //set node properties
      table[k]->root->text = NULL;                     
      table[k]->root->next = NULL;
      table[k]->size = 0;
  }
}


//notInTable to see if hashtable contains a word from a doc at a specific bucket
int find(int bucketNum, char* word, char* docID) {

  int i;
  struct Node* curNode;
    curNode = table[bucketNum]->root;
    
    for (i = 0; i < table[bucketNum]->size; ++i) {
      curNode = curNode->next;
      if (!strcmp(curNode->text, word) && (!strcmp(curNode->documentID, docID))) {
        return 1; //if in the table, return 1
      }
    }
    return 0; //else return 0
}

//find a the position of a word from a doc at a specific bucket
int findPosition(int bucketNum, char* word, char* docID) {

  int i;
  struct Node* curNode;
    curNode = table[bucketNum]->root;
    
    for (i = 0; i < table[bucketNum]->size; ++i) {
      curNode = curNode->next;
      if (!strcmp(curNode->text, word) && (!strcmp(curNode->documentID, docID))) {
        return i; //return the index in the linked list the word is at
      }
    }
    return 0;
}

//makes hashcode for a word
int hash_code(char* word, int buckets) {
  
  int S = 0;         
  int b, i;             
  
  for (i = 0; word[i] != '\0'; i++) {
    S += word[i]; //sum the ASCII values
  }
  b = S % buckets;
  return b; //return hashcode
}

//inserts a node into the table and a bucket
//in the slack it was said we could change the parameters if we want, so i made the parameters be the node and bucket number for this function
void hash_table_insert(struct Node* insertNode, int bucketNum) {

  if(table[bucketNum]->size != 0) {
    insertNode->next = table[bucketNum]->root->next;

    table[bucketNum]->root->next = insertNode;
  } 
  else{
    table[bucketNum]->root->next = insertNode;
  }
    //increment size of list in table and number of occurrences for word
    (table[bucketNum]->size)++;
    (insertNode->count)++;


}

//loops to remove all iterations of a word from the table
void hash_table_remove(int bucketNum, char* word){
  struct Node* curNode = table[bucketNum]->root;
  int i, d, k;
  d = table[bucketNum]->size;
  for (i = 0; i < d; ++i) {
    curNode = curNode->next;
    if (!strcmp(curNode->text, word)){
      k = findPosition(bucketNum, curNode->text, curNode->documentID);
      hash_table_removeatPosition(bucketNum, k);
    }
  }
}

//gets number of times a word will have to be removed from the linked list in the hashtable
int num_of_removes(int bucketNum, char* word){
  struct Node* curNode = table[bucketNum]->root;
  int i, d, k;
  k = 0;
  d = table[bucketNum]->size;
  for (i = 0; i < d; ++i) {
    curNode = curNode->next;
    if (!strcmp(curNode->text, word)){
      k++;
      }
  }
  return k;
}

//removes a node at a certain position in the linked list in hashtable
void hash_table_removeatPosition(int bucketNum, int i){

    struct Node *head = table[bucketNum]->root;
    // if linked list is empty
    if (head == NULL){
        return;
    }

    struct Node *temp = table[bucketNum]->root;

    // if head needs to be removed
    if (i == 0){
        // change head
        table[bucketNum]->root = temp->next;    
        // free old head
        free(temp);   
        (table[bucketNum]->size)--;          
        return;
    }
    int j, p;
    p = 0;
    p = (table[bucketNum]->size) - 1;
    if (i == p){

        struct Node *toDelete = table[bucketNum]->root;
        struct Node *secondLastNode = table[bucketNum]->root;

         //go to last node of the list
         while(toDelete->next != NULL)
         {
             secondLastNode = toDelete;
             toDelete = toDelete->next;
         }

         if(toDelete == table[bucketNum]->root)
         {
             table[bucketNum]->root = NULL;
         }
         else
         {
             //disconnect second last node from last node
             secondLastNode->next = NULL;
         }

        //delete last node
         free(toDelete);
        (table[bucketNum]->size)--;  
      return;
    }
    else{
        // find previous node of the node to be deleted
        for(j = 0; temp != NULL && j < i; j++){
            temp = temp->next;
        }

        // if the position is more than number of nodes
        if (temp == NULL || temp->next == NULL){
            return;
        }
    
        // node temp->next is the node to be deleted
        struct Node *yes = temp->next->next;
        free(temp->next);
        // unlink the deleted node from list
        temp->next = yes; 

        (table[bucketNum]->size)--; 
        return;
    } 
}

//notInTable to see if hashtable contains a word from a doc at a specific bucket and increments its count if it does
int contains(int bucketNum, char* word, char* docID) {

  int i;
  struct Node* curNode;
    curNode = table[bucketNum]->root;

    for (i = 0; i < table[bucketNum]->size; ++i) {
      curNode = curNode->next;
      if (!strcmp(curNode->text, word) && (!strcmp(curNode->documentID, docID))) {
        (curNode->count)++;
        return 0;
      }
    }
    return 1;
}

//create a new node
struct Node* create_new_node(char* word, char* docID) {
    //allocate space for node and set properties
    struct Node* node;
    node = malloc(sizeof(struct Node));
    node->text = word;
    node->documentID = docID;
    node->count = 0;
    node->next = NULL;
    return node;
}

//get the number of times a word from a doc is in the table
int getCount(char* word, char* docID, int bucketNum) {
  int i;
  struct Node* curNode;
    curNode = table[bucketNum]->root;
    
    for (i = 0; i < table[bucketNum]->size; ++i) {
      curNode = curNode->next;
      if (!strcmp(curNode->text, word) && (!strcmp(curNode->documentID, docID))) {
        return  curNode->count; //return count
      }
    }
    return 0; //word isn't in table
}

//reads in a file and populates the hashtable with its words
void readFile(FILE* doc, char* docID, int buckets) {
  
  char* word;
  int bucketNum;
  int notInTable;
  char line[MAX_LINE_LENGTH];
  int i = 0;
  char* temp;
  struct Node* wordNode;

  while(fgets(line, sizeof(line), doc)) {

    word = strtok(line, " ");
    while(word != NULL) {
      int len = strlen(word);
      if(len > 0 && word[len-1] == '\n') {
        word[len-1] = '\0';
        --len;
      }       
      if(len > 0 && (word[len - 1] == '.' ||  word[len - 1] == ',' ||  word[len - 1] == '!' ||  word[len - 1] == '?' ||  word[len - 1] == ':' ||  word[len - 1] == ';')) {
        word[len - 1] = '\0';
      }      
      temp = (char*) malloc((strlen(word)+1) * sizeof(char));
      strcpy(temp, word);

      //get hashcode
      bucketNum = hash_code(temp, buckets);

      //check if the list at bucket is empty
      if(table[bucketNum]->size != 0){
        notInTable = contains(bucketNum, temp, docID);
      }
      else{
        notInTable = 1;
      }

      //if word is not already in table add it
      if(notInTable) {
        //create node for word
        wordNode = create_new_node(temp, docID);
        //add to hashtable
        hash_table_insert(wordNode, bucketNum);
        }
          word = strtok(NULL, " ");
          ++i;
    }
  }
  free(temp);
}




