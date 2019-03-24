#ifndef header
#define header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node{		
	unsigned int id;
	unsigned int depth;
	unsigned int numChildren;
	unsigned int suffixHead;
	unsigned int suffixTail;
	struct Node *parent;
	struct Node *suffixLink;
	struct Node **children;
}Node;

unsigned int alphabetLen = 0;
unsigned int inputLen = 0;
unsigned int inodes = 0;
unsigned int leafs = 1;
unsigned int maxDepth = 0;
unsigned int stringDepth = 0;
Node *maxDepthNode = NULL;

char *abuff;
char *ibuff;
char iname[16];


Node *buildTree();

int dfs( Node *node );

int dfs_children( Node *node );

int bwt( Node *node );

#endif	