/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
     Header for module which contains Prefix Trie 
        data structures and functions.
*/
struct prefixTree;

#ifndef PREFIXTREESTRUCT
#define PREFIXTREESTRUCT
#define CHILD_COUNT (1 << 8)
#define UNMARKED (0)
#define MARKED (1)

struct prefixTree;

struct prefixTree {
    /* FILL IN: IMPLEMENT */
};

#endif

struct prefixTree *newPrefixTree();

struct prefixTree *addWordToTree(struct prefixTree *pt, char *word);

