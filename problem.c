/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
    Implementation for module which contains  
        Problem 2-related data structures and 
        functions.
    
    Sample solution implemented by Grady Fitzpatrick
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "problem.h"
#include "problemStruct.c"
#include "solutionStruct.c"
#include "prefixTree.h"

/* Number of words to allocate space for initially. */
#define INITIALWORDSALLOCATION 64

/* Maximum length of a word. */
#define MAXWORDLENGTH 256

/* Denotes that the dimension has not yet been set. */
#define DIMENSION_UNSET (-1)

struct problem;
struct solution;

/* Sets up a solution for the given problem. */
struct solution *newSolution(struct problem *problem);

/* 
    Reads the given dict file into a list of words 
    and the given board file into a nxn board.
*/
struct problem *readProblemA(FILE *dictFile, FILE *boardFile){
    struct problem *p = (struct problem *) malloc(sizeof(struct problem));
    assert(p);

    /* Part B onwards so set as empty. */
    p->partialString = NULL;

    int wordCount = 0;
    int wordAllocated = 0;
    char *dictText = NULL;
    char **words = NULL;

    /* Read in text. */
    size_t allocated = 0;
    /* Exit if we read no characters or an error caught. */
    int success = getdelim(&dictText, &allocated, '\0', dictFile);

    if(success == -1){
        /* Encountered an error. */
        perror("Encountered error reading dictionary file");
        exit(EXIT_FAILURE);
    } else {
        /* Assume file contains at least one character. */
        assert(success > 0);
    }

    char *boardText = NULL;
    /* Reset allocated marker. */
    allocated = 0;
    success = getdelim(&boardText, &allocated, '\0', boardFile);

    if(success == -1){
        /* Encountered an error. */
        perror("Encountered error reading board file");
        exit(EXIT_FAILURE);
    } else {
        /* Assume file contains at least one character. */
        assert(success > 0);
    }

    /* Progress through string. */
    int progress = 0;
    /* Table string length. */
    int dictTextLength = strlen(dictText);

    /* Count words present. */
    while(progress < dictTextLength){
        char *word = NULL;
        int nextProgress;
        /* Read each value into the dictionary. */
        if(progress == 0){
            /* First line. */
            int wordNumberGuess;
            assert(sscanf(dictText + progress, "%d %n", &wordNumberGuess, &nextProgress) == 1);
            /* Will fail if integer missing from the start of the words. */
            assert(nextProgress > 0);
            if(wordNumberGuess > 0){
                wordAllocated = wordNumberGuess;
                words = (char **) malloc(sizeof(char *) * wordAllocated);
                assert(words);
            }
            progress += nextProgress;
            continue;
        } else {
            assert(sscanf(dictText + progress, "%m[^\n] %n", &word, &nextProgress) == 1);
            assert(nextProgress > 0);
            progress += nextProgress;
        }

        /* Check if more space is needed to store the word. */
        if(wordAllocated <= 0){
            words = (char **) malloc(sizeof(char *) * INITIALWORDSALLOCATION);
            assert(words);
            wordAllocated = INITIALWORDSALLOCATION;
        } else if(wordCount >= wordAllocated){
            words = (char **) realloc(words, sizeof(char *) * 
                wordAllocated * 2);
            assert(words);
            wordAllocated = wordAllocated * 2;
            /* Something has gone wrong if there's not sufficient word 
                space for another word. */
            assert(wordAllocated > wordCount);
        }

        words[wordCount] = word;
        wordCount++;
    }
    
    /* Done with dictText */
    if(dictText){
        free(dictText);
    }
    
    /* Now read in board */
    progress = 0;
    int dimension = 0;
    int boardTextLength = strlen(boardText);
    /* Count dimension with first line */
    while(progress < boardTextLength){
        /* Count how many non-space characters appear in line. */
        if(boardText[progress] == '\n' || boardText[progress] == '\0'){
            /* Reached end of line. */
            break;
        }
        if(isalpha(boardText[progress])){
            dimension++;
        }
        progress++;
    }

    assert(dimension > 0);

    /* Check each line has the correct dimension. */
    for(int i = 1; i < dimension; i++){
        int rowDim = 0;
        if(boardText[progress] == '\n'){
            progress++;
        }
        while(progress < boardTextLength){
            /* Count how many non-space characters appear in line. */
            if(boardText[progress] == '\n' || boardText[progress] == '\0'){
                /* Reached end of line. */
                break;
            }
            if(isalpha(boardText[progress])){
                rowDim++;
            }
            progress++;
        }
        if(rowDim != dimension){
            fprintf(stderr, "Row #%d had %d letters, different to Row #1's %d letters.\n", i + 1, rowDim, dimension);
            assert(rowDim == dimension);
        }
    }

    /* Define board. */
    char *boardFlat = (char *) malloc(sizeof(char) * dimension * dimension);
    assert(boardFlat);
    
    /* Reset progress. */
    progress = 0;
    for(int i = 0; i < dimension; i++){
        for(int j = 0; j < dimension; j++){
            int nextProgress;
            assert(sscanf(boardText + progress, "%c %n", &boardFlat[i * dimension + j], &nextProgress) == 1);
            progress += nextProgress;
        }
    }

    char **board = (char **) malloc(sizeof(char **) * dimension);
    assert(board);
    for(int i = 0; i < dimension; i++){
        board[i] = &boardFlat[i * dimension];
    }

    // fprintf(stderr, "\n");

    /* The number of words in the text. */
    p->wordCount = wordCount;
    /* The list of words in the dictionary. */
    p->words = words;

    /* The dimension of the board (number of rows) */
    p->dimension = dimension;

    /* The board, represented both as a 1-D list and a 2-D list */
    p->boardFlat = boardFlat;
    p->board = board;

    /* For Part B only. */
    p->partialString = NULL;

    p->part = PART_A;

    return p;
}

struct problem *readProblemB(FILE *dictFile, FILE *boardFile, 
    FILE *partialStringFile){
    /* Fill in Part A sections. */
    struct problem *p = readProblemA(dictFile, boardFile);

    char *partialString = NULL;

    /* Part B has a string that is partially given - we assume it follows 
        word conventions for the %s specifier. */
    assert(fscanf(partialStringFile, "%ms", &partialString) == 1);
    
    p->part = PART_B;
    p->partialString = partialString;

    return p;
}

struct problem *readProblemD(FILE *dictFile, FILE *boardFile){
    /* Interpretation of inputs is same as Part A. */
    struct problem *p = readProblemA(dictFile, boardFile);
    
    p->part = PART_D;
    return p;
}

/*
    Outputs the given solution to the given file. If colourMode is 1, the
    sentence in the problem is coloured with the given solution colours.
*/
void outputProblem(struct problem *problem, struct solution *solution, 
    FILE *outfileName){
    assert(solution);
    switch(problem->part){
        case PART_A:
        case PART_D:
            assert(solution->foundWordCount == 0 || solution->words);
            for(int i = 0; i < solution->foundWordCount; i++){
                fprintf(outfileName, "%s\n", solution->words[i]);
            }
            break;
        case PART_B:
            assert(solution->foundLetterCount == 0 || solution->followLetters);
            for(int i = 0; i < solution->foundLetterCount; i++){
                if(isalpha(solution->followLetters[i])){
                    fprintf(outfileName, "%c\n", solution->followLetters[i]);
                } else {
                    fprintf(outfileName, " \n");
                }
            }
            break;
    }
}

/*
    Frees the given solution and all memory allocated for it.
*/
void freeSolution(struct solution *solution, struct problem *problem){
    if(solution){
        if(solution->followLetters){
            free(solution->followLetters);
        }
        if(solution->words){
            free(solution->words);
        }
        free(solution);
    }
}

/*
    Frees the given problem and all memory allocated for it.
*/
void freeProblem(struct problem *problem){
    if(problem){
        if(problem->words){
            for(int i = 0; i < problem->wordCount; i++){
                if(problem->words[i]){
                    free(problem->words[i]);
                }
            }
            free(problem->words);
        }
        if(problem->board){
            free(problem->board);
        }
        if(problem->boardFlat){
            free(problem->boardFlat);
        }
        if(problem->partialString){
            free(problem->partialString);
        }
        free(problem);
    }
}

/* Sets up a solution for the given problem */
struct solution *newSolution(struct problem *problem){
    struct solution *s = (struct solution *) malloc(sizeof(struct solution));
    assert(s);
    s->foundWordCount = 0;
    s->words = NULL;
    s->foundLetterCount = 0;
    s->followLetters = NULL;
    
    return s;
}

/* 
    Compare two words, first by alphabetical order, then by length 
*/
int cmpWords(const void *a, const void *b) {
    char *word1 = *(char **)a;
    char *word2 = *(char **)b;

    int cmp = strcmp(word1, word2);
    if (cmp != 0) {
        return cmp; // If words are not the same, sort them alphabetically
    } else {
        return strlen(word1) - strlen(word2); // If words are the same, sort them by length
    }
}

/*
    DFS function to traverse the prefix tree and find words on the board.
    (x, y) is the current position on the board.
    word is the current word being built.
    visited is a 2D array to keep track of which positions have been visited.
*/
void dfs(struct problem *p, struct prefixTree *node, struct solution *s, 
            int x, int y, char *word, int visited[p->dimension][p->dimension]) {
    /* Mark the current position as visited. */
    visited[x][y] = 1;

    /* Add the current character to the word. */ 
    char c = tolower(p->board[x][y]);
    int len = strlen(word);
    word[len] = c;
    word[len + 1] = '\0';

    /* If the current node in the prefix tree represents the end of a word, add the word to the solution. */
    if (node->isEndOfWord) {
        /* Check if the word is already in the solution. */
        int isDuplicate = 0;
        for (int i = 0; i < s->foundWordCount; i++) {
            if (strcmp(s->words[i], word) == 0) {
                isDuplicate = 1;
                break;
            }
        }

        /* If the word is not a duplicate, add it to the solution. */
        if (!isDuplicate) {
            s->foundWordCount++;
            s->words = (char **) realloc(s->words, sizeof(char *) * s->foundWordCount);
            s->words[s->foundWordCount - 1] = strdup(word); //strdup is equivalent to malloc then strcpy
        }
    }

    /* Explore all children of the current node in the prefix tree. */
    for (int i = 0; i < CHILD_COUNT; i++) {
        /* If the character edge exists... */
        if (node->children[i]) {
            /* Find the next character on the board that is adjacent to the current position. */
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = x + dx, ny = y + dy;
                    /* If the next position is within the board, not visited, and matches 
                        the next letter in the prefix tree, continue the DFS from there. */
                    if (nx >= 0 && nx < p->dimension && ny >= 0 && ny < p->dimension 
                        && !visited[nx][ny] && tolower(p->board[nx][ny]) == i) {
                        dfs(p, node->children[i], s, nx, ny, word, visited);
                    }
                }
            }
        }
    }
    /* Backtracking, removing the current character and unmark as visited. */ 
    word[len] = '\0';
    visited[x][y] = 0;
}

/*
    Solves the given problem according to Part A's definition
    and places the solution output into a returned solution value.

    Method:
    1. Traverse the prefix tree using Depth-First Search (DFS).
    2. For each new word search (starting from the tree root), locate the 
       corresponding character on the board.
    3. Continue traversing the tree. If the next character in the tree is not 
       adjacent to the last found character on the board, initiate a new search.
    4. During traversal, if a character node with isEndOfWord marked is 
       encountered, add the current word to the solution set.
*/
struct solution *solveProblemA(struct problem *p){
    struct solution *s = newSolution(p);
    struct prefixTree *pt = newPrefixTree();
    addWordsToTree(pt, p->words, p->wordCount);

    /* Initialise the word and visited array to be used in DFS. */
    char word[MAXWORDLENGTH + 1] = "";
    int visited[p->dimension][p->dimension];
    memset(visited, 0, sizeof(visited)); // Set all values to 0

    /* Start the DFS from each character (root edges) in the prefix tree that exists on the board */
    for (int i = 0; i < CHILD_COUNT; i++) {
        /* If it is a valid edge (character exists) in the prefix tree... */
        if (pt->children[i]) {
            /* Find the character on the board. */
            for (int x = 0; x < p->dimension; x++) {
                for (int y = 0; y < p->dimension; y++) {
                    /* If the character on the board matches the character in the prefix tree, perform DFS */
                    if (tolower(p->board[x][y]) == i) {
                        dfs(p, pt->children[i], s, x, y, word, visited);
                    }
                }
            }
        }
    }

    /* Sort the words in the solution alphabetically and by length. */
    qsort(s->words, s->foundWordCount, sizeof(char *), cmpWords);

    return s;
}

struct solution *solveProblemB(struct problem *p){
    struct solution *s = newSolution(p);
    /* Fill in: Part B */

    return s;
}

struct solution *solveProblemD(struct problem *p){
        struct solution *s = newSolution(p);
    /* Fill in: Part D */

    return s;
}

