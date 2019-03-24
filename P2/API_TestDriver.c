#include "API_Impl.c"
#include "API_Header.h"

#include <errno.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int get_memory_usage_kb(long* vmrss_kb, long* vmsize_kb)
{
    /* Get the the current process' status file from the proc filesystem */
    FILE* procfile = fopen("/proc/self/status", "r");

    long to_read = 8192;
    char buffer[to_read];
    int read = fread(buffer, sizeof(char), to_read, procfile);
    fclose(procfile);

    short found_vmrss = 0;
    short found_vmsize = 0;
    char* search_result;

    /* Look through proc status contents line by line */
    char delims[] = "\n";
    char* line = strtok(buffer, delims);

    while (line != NULL && (found_vmrss == 0 || found_vmsize == 0) )
    {
        search_result = strstr(line, "VmRSS:");
        if (search_result != NULL)
        {
            sscanf(line, "%*s %ld", vmrss_kb);
            found_vmrss = 1;
        }

        search_result = strstr(line, "VmSize:");
        if (search_result != NULL)
        {
            sscanf(line, "%*s %ld", vmsize_kb);
            found_vmsize = 1;
        }

        line = strtok(NULL, delims);
    }

    return (found_vmrss == 1 && found_vmsize == 1) ? 0 : 1;
}


int setUp(const char ** argv) {

	char inputname[64];
	char alphaname[64];

	struct stat input_st;
	struct stat alpha_st;

	stat(argv[1], &input_st);
	stat(argv[2], &alpha_st);

	int input_size = input_st.st_size;
	int alpha_size = alpha_st.st_size;

	strcpy(inputname, argv[1]);
	strcpy(alphaname, argv[2]);

	FILE *inputfile;
	FILE *alphafile;

	if ((inputfile = fopen(inputname, "r")) == NULL) {
		if (errno == ENOENT) {
			printf("\nERROR: No sequence file found.\n");
			return -1;
		}
		else {
			printf("\nERROR: Unhandled sequence read error.\n");
			return -1;
		}
	}

	if ((alphafile = fopen(alphaname, "r")) == NULL) {
		if (errno == ENOENT) {
			printf("\nERROR: No sequence file found.\n");
			return -1;
		}
		else {
			printf("\nERROR: Unhandled sequence read error.\n");
			return -1;
		}
	}

	ibuff = (char*)malloc(input_size);
	if (ibuff == NULL) {
		printf("\nERROR: Cannot allocate memory for sequence.\n");
		return -1;
	}

	abuff = (char*)malloc(alpha_size);
	if (abuff == NULL) {
		printf("\nERROR: Cannot allocate memory for alphabet.\n");
		return -1;
	}

	int i = 0;

	int ibytes = input_size;
	int abytes = alpha_size;
	char inchar = '\0';

	// read in name of sequence
	 do {
		inchar = fgetc(inputfile);
		if (inchar == '>') {
			inchar = fgetc(inputfile);
			while (inchar != '\n'  && inchar == '|') {
				if (i < 15) {
					iname[i] = inchar;
					++i;
				}
				inchar = fgetc(inputfile);
			}
			iname[i] = '\0';
		}
	} while (inchar != '\n' && inchar != EOF);

	// read in the sequence and ++inputLen
	do {
		inchar = fgetc(inputfile);
		while (inchar != EOF) {
			if (inchar != ' ' && inchar != '\n') {
				ibuff[inputLen] = inchar;
				++inputLen;
				//++i;
				//ibytes--;
			}
			inchar = fgetc(inputfile);
		}
	} while (inchar != EOF); /*ibytes > 0 && */
	ibuff[inputLen] = '$';
	inputLen++;
	ibuff[inputLen] = '\0';

	// read in alphabet
	do {
		inchar = fgetc(alphafile);
		if (inchar != ' ' && inchar != '\n' && inchar != EOF) {
			abuff[alphabetLen] = inchar;
			++alphabetLen;
		}
	} while (abytes > 0 && inchar != EOF);
	abuff[alphabetLen] = '\0';

	fclose(inputfile);
	fclose(alphafile);

	return (0);
}


double double_time(struct timeval *atime)
{
	return ((atime->tv_sec + (atime->tv_usec/1000000.0)) * 1000.0);
}

double diff_time(struct timeval *tstart, struct timeval *tstop)
{
	return (double_time(tstop) - double_time(tstart));
}

void deallocateMemory(Node *node) {
	free(ibuff);
	free(abuff);
	int i;//,j;
	if (node){
		for(i=0; i < node->numChildren; i++){
			deallocateMemory(node->children[i]);
		}
		//for(j=0;j<alphabetLen;j++)
		//	free(node->children[j]);
		//	node->children[j] = NULL;
		free(node->children);
		free(node);
	}
}


/* void exactMatchingRepeat() {
	
	Node *n = maxDepthNode;
	while(n -> parent -> id != 0){
		n = n -> parent;
	}

	printf("Exact Matching Repeat: ");
	for (int i = n -> suffixHead; i <= maxDepthNode -> suffixTail; i++){
		printf("%c", ibuff[i]);
	}
	printf("\n");
} */

void exactMatchingRepeat() {
	int i;
	Node *n = maxDepthNode -> children[0];
	printf("Exact Matching Repeat: ");
	for (i = n -> id - 1; i < maxDepthNode -> depth + n -> id - 1; i++){
		printf("%c", ibuff[i]);
	}
	printf("\n");
	printf("Exact Matching Repeat positions: ");
	for (i = 0; i < maxDepthNode -> numChildren; i++){
		printf("%d, ", maxDepthNode -> children[i] -> id);
	}
	printf("\n");
}


void printStatistics(Node *tree, char *outputFile) {

	struct timeval tstart, tstop;
	long vmrss, vmsize, startMem, endMem;
	char *x = "_BWT.txt";

	get_memory_usage_kb(&vmrss, &vmsize);
    startMem = vmrss;
	gettimeofday(&tstart, NULL);
	tree = buildTree();		// build ST
	gettimeofday(&tstop, NULL);
    get_memory_usage_kb(&vmrss, &vmsize);
	endMem = vmrss;
	printf("\nSUFFIX TREE CONSTRUCTION STATISTICS:\n");
	printf("\nInput size: %ld (bytes)\n", sizeof(char *) * strlen(ibuff));
	printf("Suffix Tree Construction Time: %f (ms)\n", diff_time(&tstart, &tstop));
	printf("Suffix Tree Construction Space: %6ld (KB)\n", endMem - startMem);
	printf("Implementation Constant (bytes consumed by code for every input byte): %ld\n", ((endMem - startMem) * 1024) / (sizeof(char *) * strlen(ibuff)));
	printf("Number of internal nodes: %d\n", inodes + 1);
	printf("Number of leafs: %d\n", leafs - 1);
	printf("Total number of nodes: %d\n", inodes + leafs);
	printf("Size of the tree: %ld (bytes)\n", (inodes + leafs) * sizeof(tree));
	printf("Average string depth of internal nodes: %d\n", (stringDepth / (inodes + 1)));
	printf("String depth of deepest internal node: %d\n", maxDepth);
	exactMatchingRepeat();

	FILE *ptr = fopen(strcat(outputFile, x), "a");
	bwt(tree, ptr);
	fclose(ptr);
	
	/* printf("Pre-order DFS traversal:\n");
	dfs(tree); 
	printf("\n");
	display_children(tree); // to enumerate children of root from left to right 
	printf("\n"); */
}


int main (int argc, const char *argv[])
{

	Node *tree;
	char *outputFile;

	if (argc < 3) {
		printf("\nERROR: Incorrect number of arguments.\n");
		return -1;
	}

	printf("\nThe arguments are:\n\n");
	printf("Input File:\t%s\n", argv[1]);
	printf("Input Alphabet:\t%s\n", argv[2]);

	
	if (setUp(argv) < 0) 
		return -1;

	strcpy(outputFile, argv[1]);
	printStatistics(tree, outputFile);

	deallocateMemory(tree);
	return (0);
}