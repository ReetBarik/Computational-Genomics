#ifndef header
#define header

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node{		
	int id;
	int depth;
	int numChildren;
	int suffixHead;
	int suffixTail;
	struct Node *parent;
	struct Node *suffixLink;
	struct Node **children;
}Node;

int alphabetLen = 0;
int inputLen = 0;
int inodes = 0;
int leafs = 1;
int maxDepth = 0;
int stringDepth = 0;
Node *maxDepthNode = NULL;

char *abuff;
char *ibuff;
char iname[16];


Node *buildTree();

int dfs( Node *node );

int display_children( Node *node );

int bwt( Node *node , FILE *ptr );

#endif	