#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include "hashtable.h"

 const int MAX_DIR_LENGTH = 50; //max directory length
 const int MAX_PATH_LENGTH = 200; //max path length
 double* ranking;        // array for ranking scores
 int docs = 0;           // number of docs
 char** docNames;         // array for doc names
 int b;                  // number of buckets
 int firstCall;              // firstCall call to main for bucket query
 char* path = NULL;
 char* dir = NULL;

//prototypes
void resetRanking (void);
int dFreq(char* word, int bucketNum);
double tf_idf (char* word, int i, int bucketNum, double numberOfDocs);
void rank(char* query);
void bubbleSort(void);
void printRelevance(void);
void read_query(char* query);
void stop_word(void);
void training(void);
int main(void);


//finds # of doc the word appears in
int dFreq(char* word, int bucketNum) {

  int i;
  int freq = 0;
  struct Node* curNode;
  curNode = table[bucketNum]->root;

    for (i = 0; i < table[bucketNum]->size; ++i) {
      curNode = curNode->next;
      if (!strcmp(curNode->text, word)) {
        ++freq;
      }
    }
    return freq;
}

//finds the tf-idf for word
double tf_idf (char* word, int i, int bucketNum, double numberOfDocs) {
    int tf, df;
    double idf, tfIdf;

    //get term frequency
    if (find(bucketNum, word, docNames[i])){
        tf = getCount(word, docNames[i], bucketNum); 
    }            
    else{
        tf = 0;
    }
    //get document frequency
    df = dFreq(word, bucketNum);                         

    //get inverse document frequency
    if (df == 0){
        idf = log(numberOfDocs / 1);                     
    }
    else{
        idf = log(numberOfDocs / df);
    }  
    //get term frequencey-inverse document frequency
    tfIdf = (((double) tf) * (idf));                       
    return tfIdf;
}

//finds the total rank score for all words in the query
void rank(char* query) {
    int i;

    char* curWord = strtok(query, " ");
    while (curWord != NULL) {
        for(i = 0; i < docs; ++i) {
            ranking[i] += tf_idf(curWord, i, hash_code(curWord, b), docs);
        }
        curWord = strtok(NULL, " ");
    }
}

//sorts the ranking array and doclist name array from highest to lowest in accordance with each other
void bubbleSort(void) {
    int i, j, l;
    double temp1;
    char* temp2;
    l = 0;
    

    for (i = 1; i < docs; ++i) {
      for (j = 0; j < docs-1; ++j) {
         if (ranking[j] < ranking[j+1]) {
            temp1 = ranking[j];
            temp2 = docNames[j];
            ranking[j] = ranking[j+1];
            docNames[j] = docNames[j+1];
            ranking[j+1] = temp1;
            docNames[j+1] = temp2;
         }
      }
    }

        //prints ranking to terminal
        for (j = 0; j < docs; ++j){
              if(ranking[j] == 0){
                l++;
              }
              if(l == docs){
              printf("No Match Found\n");
              return;
            }
          }
         for (j = 0; j < docs; ++j){
            printf("%s\n", docNames[j]);
          }
      
}

//sorts ranking results and prints them to fP
void printRelevance(void) {
    int i;
    bubbleSort();
    FILE * fP;
    
    //open the file in "a" mode
    fP = fopen("search_scores.txt", "a");

    // if couldn't open the file
    if(fP == NULL)
    {
        // file not created
        printf("File creation failed.\n");
        exit(EXIT_FAILURE);
    }
    
    // write data to file
    for (i = 0; i < docs; ++i) {
        
          fprintf(fP, "%s:%f\n", docNames[i], ranking[i]);
    }

    //close fP to save the new data
    fclose(fP);
}

//get search query and call ranking function
void read_query(char *query) {

    int len = strlen(query);
    if (len > 0 && query[len - 1] == '\n'){
        query[len-1] = '\0';
    }
 
    rank(query);
    free(query);
    
}

//remove stop words from hashtable
void stop_word(void){
    int i,j, df, d, n;
    struct Node* curNode;
    n = 0;
    
    for(j = 0; j < b; j++){
        curNode = table[j]->root;
        d = table[j]->size;
        for (i = 0; i < d; ++i) {
            curNode = curNode->next;
            if(curNode == NULL){
                continue;
            }
            df = dFreq(curNode->text, j); 
                if(df == docs){
                    n = num_of_removes(j, curNode->text);
                    hash_table_remove(j, curNode->text);
                    d -= n;
                    if(((d-i) > 1)){
                        i = 0;
                        curNode = table[j]->root;
                        continue;
                    }
                    else{
                        break;
                    }        
                }      
            }
        }
        
    }

//to reset ranking array
void resetRanking (void) {
    int i;
    for (i = 0; i < docs; ++i)
        ranking[i] = 0.0;
}

//training - populates hashtable with fP contents
void training(void) {

    char* toNull = "";
    char* fileName;
    int len;
    DIR* FD;
    struct dirent* in_file;
    FILE* cur_file;
    char temp[50];

    docNames = malloc(sizeof(char*) * docs);
    
    //open directory
    if (!dir) {                                            
        while (!dir) {
            dir = malloc(sizeof(char) * MAX_DIR_LENGTH);
        
            if (NULL == (FD = opendir ("p5docs"))) {
                fprintf(stderr, "Error : Failed to open directory - %s\n", strerror(errno));
                dir = NULL;
            }
        }
    } 
    
    //open path
    if (!path) {                                            
        path = malloc(sizeof(char) * MAX_PATH_LENGTH);
 
        path = "p5docs/";
    
    //scan in the directory files
    while ((in_file = readdir(FD))) {
        strcpy(temp, toNull);                               
        
        //make sure it is a text file
        if (!((len = strlen(in_file->d_name)) > 3 && !strcmp(in_file->d_name + len - 4, ".txt")) || !strcmp(in_file->d_name, "search_scores.txt")){
            continue;
        }
        else{
            //increase number of docs
            ++docs;                                        
        }  
        //name of the current file
        fileName = in_file->d_name;                        
        docNames[docs-1] = malloc(strlen(fileName) + 1);
        docNames[docs-1] = fileName;

        strcat(temp, path);                            
        strcat(temp, fileName);                      
        
        //open the current file
        cur_file = fopen(temp, "r");
            
        //read in all the words
        readFile(cur_file, fileName, b);
        }
            
        //close the file
        fclose(cur_file);
    }
    //close the directory
    closedir(FD);
    ranking = malloc(sizeof(double) * docs);  
    //remove the stop words             
    stop_word();
} 


int main(void) {
  
  //if it is the firstCall call to main
  if (!firstCall) {

    char *query = malloc(sizeof(char) * MAX_LINE_LENGTH);
    //get number of buckets
    printf("How many buckets?:\n");
    scanf("%d", &b);
 
    //train and populate table
    initializeTable(b);
    training();
    firstCall = 1;

    //firstCall prompt for query or X
    printf("Enter Search String or X to Exit:\n");
    getchar();
    fgets(query,1000,stdin);

    int len = strlen(query);
    if (len > 0 && query[len - 1] == '\n'){
        query[len-1] = '\0';
    }
    if(!strcmp(query, "X")){
        exit(0);
    }

    read_query(query);
    printRelevance();
    resetRanking();
    main();
    free(table);
  
  }

    //clear the input
    fflush(stdin); 
    printf("Enter Search String or X to Exit:\n");

    char *query = malloc(sizeof(char) * MAX_LINE_LENGTH);
    fgets(query,1000,stdin);

    int len = strlen(query);
    if (len > 0 && query[len - 1] == '\n'){
        query[len-1] = '\0';
    }
    if(!strcmp(query, "X")){

        exit(0);
    }

    read_query(query);
    printRelevance();
    resetRanking();
    main();
    free(table);
  
}


