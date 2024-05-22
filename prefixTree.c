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

    for(int i = 0; i < strlen(word); i++){
        if(curr->children[word[i]] == NULL){
            curr->children[word[i]] = newPrefixTree();
        }
        curr = curr->children[word[i]];
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
