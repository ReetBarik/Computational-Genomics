#include "Header.h"
#include "SuffixTree.c"
#include "Alignment.c"
#include "ReadMapping.c"

#include <errno.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include <time.h>


int readInput(const char ** argv) {
	int linenumber = 0;
	int readnum = 0;
	int currentRead  = 0;

	char paramLine[20];
	char inputName[64];
	char readsName[64];
	char alphaName[64];

	struct stat inputStruct;
	struct stat readsStruct;
	struct stat alphaStruct;

	unsigned int inputSize = inputStruct.st_size;
	unsigned int readsSize = readsStruct.st_size;
	unsigned int alphaSize = alphaStruct.st_size;

	stat(argv[1], &inputStruct);
	stat(argv[2], &readsStruct);
	stat(argv[3], &alphaStruct);

	strcpy(inputName, argv[1]);
	strcpy(readsName, argv[2]);
	strcpy(alphaName, argv[3]);

	FILE *inputFile;
	FILE *readsFile;
	FILE *alphaFile;
	FILE *configFile;

	if ((inputFile = fopen(inputName, "r")) == NULL) {
		printf("\nERROR: No sequence file found.\n");
		return -1;
	}

	if ((readsFile = fopen(readsName, "r")) == NULL) {
		printf("\nERROR: No reads sequence file found.\n");
		return -1;
	}

	if ((alphaFile = fopen(alphaName, "r")) == NULL) {
		printf("\nERROR: No alphabet file found.\n");
		return -1;
	}

	if ((configFile = fopen("parameters.config", "r")) == NULL) {
		printf("\nERROR: No config file found.\n");
		return -1;
	}

	sequence = (char*)malloc(inputSize);
	if (sequence == NULL) {
		printf("\nERROR: Cannot allocate memory for sequence.\n");
		return -1;
	}

	
	// buffer to allocate for reads
	alphabet = (char*)malloc(alphaSize);
	if (alphabet == NULL) {
		printf("\nERROR: Cannot allocate memory for alphabet.\n");
		return -1;
	}

	int i = 0;
	char inchar = '\0';

	// read the input file sequentially
	 do {
		inchar = fgetc(inputFile);
		if (inchar == '>') {
			inchar = fgetc(inputFile);
			while (inchar != '\n'  && inchar == '|') {
				if (i < 15) {
					sequenceName[i] = inchar;
					++i;
				}
				inchar = fgetc(inputFile);
			}
			sequenceName[i] = '\0';
		}
	} while (inchar != '\n' && inchar != EOF);

	// read the sequence from the input file 
	do {
		inchar = fgetc(inputFile);
		while (inchar != EOF) {
			if (inchar != ' ' && inchar != '\n') {
				sequence[sequenceLength] = inchar;
				++sequenceLength;
			}
			inchar = fgetc(inputFile);
		}
	} while (inchar != EOF);
	sequence[sequenceLength] = '$';
	sequenceLength++;
	sequence[sequenceLength] = '\0';

	// get maximum length
	do {
		inchar = fgetc(readsFile);
		++currentRead;
		if (inchar == '\n') {
			++linenumber;
			if (currentRead > readnum) {	
				readnum = currentRead;
				currentRead = 0;
			}
		}
	} while (inchar != EOF);

	fseek(readsFile, 0, SEEK_SET);

	//	allocate buffer for reads
	readBuffer = (char*)malloc(readsSize);
	if (readBuffer == NULL) {
		printf("\nERROR: Cannot allocate memory for reads sequence.\n");
		return (-1);
	}
	
	// read the set of reads from read file
	readsList = (char**)malloc(linenumber * sizeof(char*));
	if (readsList == NULL) {
		printf("\nERROR: Cannot allocate memory for readsList.\n");
		return (-1);
	}
	
	int currReadLen = 0;
	int maxReadLen = 0;
	
	// prepare readList
	do {
		inchar = fgetc(readsFile);
		if (inchar == '>') {		// read in a name
			inchar = fgetc(readsFile);
			readsList[readIndex] = &readBuffer[readsLength];
			++readIndex;
			while(inchar != '\n' && inchar != EOF) {
				readBuffer[readsLength] = inchar;
				++readsLength;
				inchar = fgetc(readsFile);
			}
		}
		else if(inchar != EOF){
			readsList[readIndex] = &readBuffer[readsLength];
			++totalReads;
			++readIndex;
			while (inchar != '\n' && inchar != EOF) {
				readBuffer[readsLength] = inchar;
				++readsLength;
				++currReadLen;
				inchar = fgetc(readsFile);
			}
		}
		if (currReadLen > maxReadLen) {
			maxReadLen = currReadLen;
			currReadLen = 0;
		}
		if (inchar == '\n') {
			readBuffer[readsLength] = '\0';
			++readsLength;
		}
	} while (inchar != EOF);
	readBuffer[readsLength] = '\0';

	// read the set of alphabets 
	do {
		inchar = fgetc(alphaFile);
		if (inchar != ' ' && inchar != '\n' && inchar != EOF) {
			alphabet[alphabetLength] = inchar;
			++alphabetLength;
		}
	} while (inchar != EOF);
	alphabet[alphabetLength] = '\0';

	if (configFile) {
		while(fgets(paramLine, 20, configFile)){ // maximum 10 characters required
			char *token = strtok(paramLine, " \t\n");
			if(strcmp(token, "match") == 0){
				token = strtok(NULL, "\t\n");
				ma = atoi(token);
			}
			else if(strcmp(token, "mismatch") == 0){
				token = strtok(NULL, " \t\n");
				mi = atoi(token);
			}
			else if(strcmp(token, "h") == 0){
				token = strtok(NULL, " \t\n");
				h = atoi(token);
			}
			else if(strcmp(token, "g") == 0){
				token = strtok(NULL, " \t\n");
				g = atoi(token);
			}
			else if(strcmp(token, "lambda") == 0){
				token = strtok(NULL, " \t\n");
				lambda = atoi(token);
			}
			else if(strcmp(token, "X") == 0){
				token = strtok(NULL, " \t\n");
				X = atoi(token);
			}
			else if(strcmp(token, "Y") == 0){
				token = strtok(NULL, " \t\n");
				Y = atoi(token);
			}
		}
		// printf("Scoring Parameters: Match = %d, Mismatch = %d, h = %d, g = %d, lambda = %d, X = %d, Y = %d\n\n", ma, mi, h, g, lambda, X, Y);
	}

	fclose(inputFile);
	fclose(readsFile);
	fclose(alphaFile);
	fclose(configFile);

	return (0);
}

// helper function to convert time into double
double double_time(struct timeval *atime)
{
	return ((atime->tv_sec + (atime->tv_usec/1000000.0)) * 1000.0);
}


// helper function to find difference between two timestamps
double diff_time(struct timeval *tstart, struct timeval *tstop)
{
	return (double_time(tstop) - double_time(tstart));
}


// free memory for tree
void deallocate_tree(Node *node) {
	int i;
	if (node){
		for(i=0; i < node->numberChildren; i++){
			deallocate_tree(node->children[i]);
		}
		free(node->children);
		free(node);
	}
}

void deallocate_memory(Node *node) {
	free(sequence);
	free(alphabet);
	free(A);
	deallocate_tree(node);
}

int main(int argc, const char *argv[]){
	int i, x;
	float construct_ST_time, prepare_ST_time;
	struct timeval tstart, tstop;

	if (argc < 4) {
		printf("\nERROR: Incorrect number of arguments.\n");
		return -1;
	}

	
    // printf("\nThe arguments are:\n\n");
	// printf("Reference File:\t%s\n", argv[1]);
    // printf("Reads File:\t%s\n", argv[2]);
	// printf("Input Alphabet:\t%s\n", argv[3]);

	readInput(argv);

	
	// Construct ST
	gettimeofday(&tstart, NULL);
	tree = buildTree();
	gettimeofday(&tstop, NULL);
	construct_ST_time =  diff_time(&tstart, &tstop);
	
	// Prepare ST
	gettimeofday(&tstart, NULL);
	x = prepareST(tree);
	gettimeofday(&tstop, NULL);
	prepare_ST_time =  diff_time(&tstart, &tstop);
	
	
	if(x == -1){
		printf("\nERROR: Couldn't prepare ST(ms)\n");
		return -1;
	}

	// mapReads
	printf("OUTPUT:\n\n");
	gettimeofday(&tstart, NULL);
	mapReads(tree);
	gettimeofday(&tstop, NULL);
	

	printf("\n--------------------------------------------------------------------------------------------------\n\n");

	printf("STATISTICS:\n\n");
	// Input Statistics
	printf("Length of Reference genome: %d\n", sequenceLength);
	printf("Number of reads in the input: %d\n", totalReads);
	
	// Timing Statistics
	printf("Execution time - ConstructST: %f(ms)\n",construct_ST_time);
	printf("Execution time - PrepareST: %f(ms)\n",prepare_ST_time);
	printf("Execution time - MapReads: %f (ms)\n", diff_time(&tstart, &tstop));

	// Other Statistics
	printf("Alignments performed per read: %f\n", (((float)countAlign)/((float)10000)));
	printf("Hit Rate: %f\n",(((float)countHits)/((float)10000))*100);

	deallocate_memory(tree);

    return 0;
}