#include "API_Header.h"

int pretty_dfs = 0;

Node *makeNode(int id, Node *parent,int suffixHead,int suffixTail,int stringDepth ){
	
	Node *newnode = (Node *)malloc(sizeof(Node));
	if (newnode == NULL) {
		printf("\nERROR: Could not allocate memory for new node\n");
		exit (1);
	}
	if (suffixHead > suffixTail){
		printf("Error: Could not make Node with ID: %d", id);
		exit (-1);
	}

	newnode->id = id;
	newnode->suffixLink = NULL;
	newnode->parent = parent == NULL ? newnode : parent;
	newnode->suffixHead = suffixHead;
	newnode->suffixTail = suffixTail;
	newnode->numChildren = 0;
	newnode->children = calloc(1, alphabetLen * sizeof(Node *));
	newnode->depth = stringDepth;
	return (newnode);
}


// add a child to the given node and sort the children alphabetically
void addChild( Node *parent, Node *child ){
	parent -> children[parent -> numChildren] = child;
	parent -> numChildren++;
	child -> parent = parent;
	// Bubble sort the children
	Node *temp;
	for(int i=0; i < parent -> numChildren - 1; ++i ){
		for(int j=0; j < parent -> numChildren - 1 - i; ++j ){
			if( ibuff[parent -> children[j] -> suffixHead] > ibuff[parent -> children[j + 1] -> suffixHead]) {
				temp = parent -> children[j + 1];
				parent -> children[j + 1] = parent -> children[j];
				parent -> children[j] = temp;
			}
		}
	}
	return;
}



// find the child that matches the first character of the suffix
Node *matchChild( Node *n, int suffix, int *i ){
	Node *current = NULL;
	//at node n check all children's first char
	//at the child
	for (*i = 0; *i < n -> numChildren && n -> numChildren > 0; *i+=1){
		current = n -> children[*i];
		if (ibuff[current -> suffixHead] == ibuff[suffix]){
			return (current);
		}
	}
	// No child exists with the first character of suffix
	return (NULL);
}

// split the current nodes parent edge with the suffix return the leaf
Node *splitEdge( Node *current, int head, int tail ){
	int i = 0, z = 0;
	matchChild(current -> parent, head, &z);
	for( i = current -> suffixHead; i <= tail; ++i ) {  //(int)strlen(suffix); ++i ){
		if( ibuff[i] != ibuff[head + (i - current -> suffixHead)] ){

			Node *newInode = makeNode( inputLen + inodes + 1, current -> parent, current -> suffixHead, i - 1, current -> parent -> depth + i - current -> suffixHead);
			inodes ++;
			stringDepth += newInode -> depth;
			if (newInode -> depth > maxDepth){
				maxDepth = newInode -> depth;
				maxDepthNode = newInode;
			}
			// need to set the current children to the new inodes children
			addChild(newInode, current);
			newInode -> parent -> children[z] = newInode;
			current -> suffixHead = newInode -> suffixTail + 1;
			//current->suffixTail = current->suffixTail;
			Node *newLeaf = makeNode( leafs, newInode, head + newInode -> suffixTail - newInode -> suffixHead + 1, tail, tail - (head + newInode -> suffixTail - newInode -> suffixHead) + newInode -> depth);
			leafs ++;
			addChild(newInode, newLeaf);
			return (newLeaf);
		}
	}
	perror("ERROR: Couldn't split edge\n");
	exit(1);
	return (current);
}


// given a node and a suffix find the end of the suffix by traversing down
// returns the parent that mismatches
Node *hop( Node *n,int head, int tail){ 
	int numChild = 0, i = 0, min;
	Node *a = matchChild(n, head, &numChild);
	// if there isnt a child that matches return that node
	if( a == NULL){
		//if ( strlen(beta) == 1 )
		//	return n;
		return (n);
	}
	//x = (int)strlen(beta);
	//y = (int)strlen(a->parentEdgeLabel);
	min = ((tail - head + 1) < (a -> suffixTail - a -> suffixHead + 1)) ? (tail - head + 1) : (a -> suffixTail - a -> suffixHead + 1);
	for(i = 0; i < min; i++){
		if( ibuff[head + i] != ibuff[a -> suffixHead + i] ){
			return (n);
		}
	}
	// not an ending leaf and the for loop has gone through the string
	return (hop( a, head + i, tail));
}

Node *findPath( Node *v, int head ){
	int childNum, tail = inputLen - 1;
	Node *hopped = hop(v, head, inputLen - 1);
	int hops = hopped -> depth - v -> depth;
	Node *child = matchChild(hopped, head + hops, &childNum);
	if ( child == NULL){
		child = makeNode(leafs, hopped,	head + hops, tail, hopped -> depth + (tail - head) + 1);
		addChild(hopped, child);
		leafs ++;
	} else { // a child exists
		child = splitEdge(child, head + hops, tail);
	}
	return child;
}

Node *nodeHops(Node *vPrime, Node *u, int betaHead, int betaEnd, int suffix){
	int r = 0, childNum = 0, betaLen = (betaEnd - betaHead + 1);
	Node *currNode = vPrime;
	Node *e = NULL, *i = NULL, *v = NULL;

	while(r <= (betaLen)){ // r <= beta len
		// let e be the edge under currnode that starts with the character b[r]
		e = matchChild(currNode, betaHead + r, &childNum);
		if(e){ // if e exists
			int edgeLen = (e -> suffixTail - e -> suffixHead + 1);
			if( edgeLen + r > betaLen ){ // beta will get exhausted and hence split edge required in the middle of the edge
				i = splitEdge(e, suffix + currNode -> depth, inputLen - 1);
				v = i -> parent;
				u -> suffixLink = v;
				return i;
			} else if( edgeLen + r == betaLen ){ // end of beta and end of edge coincides
				v = e;
				u->suffixLink = v;
				int k = u -> depth;
				i = findPath(v, suffix + k -1);
				return i;
			} else { // edge will exhausted and beta will overflow to the next edge 
				r += edgeLen;
				currNode = e;
				continue;
			}
		} else { // this case shouldn't arise as leaf formation is taken care of by splitEdge
			printf("ERROR: Don't know why this error occurs");
			exit(-1);
		}
	}
	return i;
}


Node *insert( int i, Node *root, Node *leaf ){

/* 	if (leaf == NULL){
		printf("ERROR: Leaf returned null: i = %d",i);
		return (NULL);
	} */
	Node *u = leaf->parent;
	int Case = -1;

	if( u->suffixLink != NULL )
		if ( u != root )			// case IA
			Case = 0;
		else 						// case IB
			Case = 1;
	else if ( u->parent != root )	// case IIA
			Case = 2;
	else							// case IIB
		Case = 3;

	switch(Case){
		// IA: suffix link for u is known and u is not the root
		case 0:
			{
				int cAlphaLen = u -> depth;
				Node *v = u -> suffixLink;
				return findPath(v, i + cAlphaLen - 1); 
				//}
				break;
			}
		// IB: suffix link for u is known and u is the root
		case 1:
			{
				return findPath(u, i);
				break;
			}
		// IIA: suffix link for u is unknown and u' is not the root
		case 2:
			{
				Node *uPrime = u -> parent;
				int betaHead = u -> suffixHead, betaTail = u -> suffixTail; //betaHead to betaTail contains Beta
				Node *vPrime = uPrime -> suffixLink;
				return nodeHops(vPrime, u, betaHead, betaTail, i);
				break;
			}
		// IIB: suffix link for u is unknown and u' is the root
		case 3:
			{
				Node *uPrime = u -> parent;
				int betaHead = u -> suffixHead, betaTail = u -> suffixTail; //betaHead to betaTail contains Beta
				int betaPrimeHead = betaHead + 1;                           //betaPrimeHead to betaTail contains BetaPrime
				if (betaTail == betaHead){ // the lenght of u is 1
					u -> suffixLink = uPrime;
					return findPath(uPrime, i);
				} else {
					return nodeHops(uPrime, u, betaPrimeHead, betaTail, i);
				}
				break;
			}
		default:
			printf("ERROR: Couldn't insert\n");
			exit(1);
	}
	return 0;
}


Node *buildTree(){
	Node *root = makeNode(0, NULL, 0, 0, 0);
	root -> suffixLink = root;
	Node *leaf = root;
	for(int i=0; i < inputLen; i++ ){
		leaf = insert( i, root, leaf);
		if (leaf == NULL)
			return NULL;
	}
	return root;
}


// depth first search - preorder
int dfs( Node *node ){
	int i;
	pretty_dfs ++;
	if (pretty_dfs % 10 == 0) 
		printf("Depth: %d\n", node->depth);
	else
		printf("Depth: %d\t", node->depth);
	
	for ( i = 0; i < node -> numChildren; ++i)
	{
		dfs(node -> children[i]);
	}
	return (0);
}


int display_children(Node *node){
	printf("Enumerating children of Node ID: %d from left to right:\n", node -> id);
    for(int i = 0; i < node -> numChildren; i++)
    {
        printf("Child Number: %d, ID: %d ; ", i, node -> children[i] -> id);
    }
	printf("\n");
    return 0;
}

/* Function: bwt()
 * Input:
 * 		*node: pointer to node in tree
 * Output:
 * 		int: returns 0 for success, -1 for failure
 * Summary: Burrows Wheeler Transform. Given an input string, construct a BWT
 * 		and print it out.  Basically, go to leafs and find representative val.
 */


int bwt( Node *node ){
	int i;
	for( i=0; (i < node->numChildren) && (node->children[i] != NULL); i++ ){
		bwt( node->children[i] );
	}
	if (node->numChildren == 0)
	{
		int bwtval;
		bwtval = node->id - 1;
		printf("%c ", (bwtval > 0 ? ibuff[bwtval-1] : ibuff[inputLen - 1]));
	}
	return (0);
}
