#ifndef header
#define header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int *A = NULL;
int nextIndex = 0;

Node *buildTree();
void DFS_PrepareST(Node *T);

#endif