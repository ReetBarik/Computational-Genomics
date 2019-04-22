#ifndef header
#define header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

typedef struct Node{		
	int id;
	int depth;
	int numberChildren;
	int suffixHead;
	int suffixTail;
    int startLeafIndex; 
    int endLeafIndex; 
	struct Node *parent;
	struct Node *suffixLink;
	struct Node **children;
}Node;

typedef struct DP_cell{
	int s; int i; int d;
} DP_cell;


DP_cell **Matrix;                       // stores the dynamically computed matrix
int SW = 1;                             // Smith-Waterman or Needleman-Wunsch
int ma, mi, h, g, lambda, X, Y;         // scoring parameters
int maxRowIndex = 0 , maxColIndex = 0;  // stores index of max value to back track from
char *s1;                               // stores sequence 1
char *s2;                               // stores sequence 2

int alphabetLength = 0;					// size of alphabet 
int sequenceLength = 0;					// length of sequence 
int internalNodes = 0;					// # internal nodes
int leafs = 1;							// # leaves
Node *maxDepthNode = NULL;				// deepest internal node
int maxDepth = 0;						// depth of the deepest internal node
int stringDepth = 0;					// running sum of depth of each internal node
char *alphabet;							// alphabet
char *sequence;							// input sequence
char sequenceName[16];

char *readBuffer;
char **readsList;	
int readsLength = 0;
int totalReads = 0;
int readIndex = 0;

Node *tree;
int *A = NULL;
int nextIndex = 0;

Node *buildTree();
void DFS_PrepareST(Node *T);

#endif