/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
     Implementation for module which contains Prefix Trie 
        data structures and functions.
*/
#include "prefixTree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct prefixTree *newPrefixTree(){
    struct prefixTree *retTree = (struct prefixTree *)malloc(sizeof(struct prefixTree));;
    retTree->isEndOfWord = UNMARKED;

    for(int i = 0; i < CHILD_COUNT; i++){
        retTree->children[i] = NULL;
    }

    return retTree;
}

struct prefixTree *addWordToTree(struct prefixTree *pt, char *word){
    struct prefixTree *curr = pt;

    for(unsigned int i = 0; i < strlen(word); i++){
        unsigned char c = (unsigned char) word[i];
        if(curr->children[c] == NULL){
            curr->children[c] = newPrefixTree();
        }
        curr = curr->children[c];
    }
    curr->isEndOfWord = MARKED;

    return pt;
}

struct prefixTree *addWordsToTree(struct prefixTree *pt, char **words, int wordCount){
    for(int i = 0; i < wordCount; i++){
        pt = addWordToTree(pt, words[i]);
    }

    return pt;
}

/*
    Used in Part D to add words to the tree only if they are unique.
    i.e. does not contain any repeated characters.
*/
struct prefixTree *addUniquesToTree(struct prefixTree *pt, char **words, int wordCount){
    int isCharPresent[CHILD_COUNT] = {0}; // mask to check if a character is present in a word
    int isUnique = 1;

    for(int i = 0; i < wordCount; i++){
        for(unsigned int j = 0; j < strlen(words[i]); j++){
            unsigned char c = (unsigned char) words[i][j];
            if(isCharPresent[c] == 0){
                isCharPresent[c] = 1;
            } else {
                isUnique = 0;
                break;
            }
        }

        if(isUnique){
            pt = addWordsToTree(pt, words, wordCount);
        }

        isUnique = 1;
    }

    return pt;
}