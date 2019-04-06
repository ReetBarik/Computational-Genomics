#include "Header.h"
#include "SuffixTree.c"

#include <errno.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>


int readInput(const char ** argv) {

	char paramLine[20];
    // get total number of lines in read
	int numlines = 0;
	int longRead = 0;
	int curRead  = 0;

	char inputname[64];
	char readsname[64];
	char alphaname[64];

	struct stat input_st;
	struct stat reads_st;
	struct stat alpha_st;

	stat(argv[1], &input_st);
	stat(argv[2], &reads_st);
	stat(argv[3], &alpha_st);

	unsigned int input_size = input_st.st_size;
	unsigned int reads_size = reads_st.st_size;
	unsigned int alpha_size = alpha_st.st_size;

	strcpy(inputname, argv[1]);
	strcpy(readsname, argv[2]);
	strcpy(alphaname, argv[3]);

	FILE *inputfile;
	FILE *readsfile;
	FILE *alphafile;
	FILE *configfile;

	if ((inputfile = fopen(inputname, "r")) == NULL) {
		printf("\nERROR: No sequence file found.\n");
		return -1;
	}

	if ((readsfile = fopen(readsname, "r")) == NULL) {
		printf("\nERROR: No reads sequence file found.\n");
		return -1;
	}

	if ((alphafile = fopen(alphaname, "r")) == NULL) {
		printf("\nERROR: No alphabet file found.\n");
		return -1;
	}

	if ((configfile = fopen("parameters.config", "r")) == NULL) {
		printf("\nERROR: No config file found.\n");
		return -1;
	}

	sequence = (char*)malloc(input_size);
	if (sequence == NULL) {
		printf("\nERROR: Cannot allocate memory for sequence.\n");
		return -1;
	}

	// readBuffer allocation is in the read section	

	alphabet = (char*)malloc(alpha_size);
	if (alphabet == NULL) {
		printf("\nERROR: Cannot allocate memory for alphabet.\n");
		return -1;
	}

	int i = 0;
	char inchar = '\0';

	// read in name of sequence
	 do {
		inchar = fgetc(inputfile);
		if (inchar == '>') {
			inchar = fgetc(inputfile);
			while (inchar != '\n'  && inchar == '|') {
				if (i < 15) {
					sequenceName[i] = inchar;
					++i;
				}
				inchar = fgetc(inputfile);
			}
			sequenceName[i] = '\0';
		}
	} while (inchar != '\n' && inchar != EOF);

	// read in the sequence and ++sequenceLength
	do {
		inchar = fgetc(inputfile);
		while (inchar != EOF) {
			if (inchar != ' ' && inchar != '\n') {
				sequence[sequenceLength] = inchar;
				++sequenceLength;
			}
			inchar = fgetc(inputfile);
		}
	} while (inchar != EOF);
	sequence[sequenceLength] = '$';
	sequenceLength++;
	sequence[sequenceLength] = '\0';

	do {
		inchar = fgetc(readsfile);
		++curRead;
		if (inchar == '\n') {
			++numlines;
			if (curRead > longRead) {	// also get max line length
				longRead = curRead;
				curRead = 0;
			}
		}
	} while (inchar != EOF);

	fseek(readsfile, 0, SEEK_SET);	// reset to beginning

	// allocate reads indexing array
	readBuffer = (char*)malloc(reads_size);
	if (readBuffer == NULL) {
		printf("\nERROR: Cannot allocate memory for reads sequence.\n");
		return (-1);
	}
	
	// read in reads
	readsList = (char**)malloc(numlines * sizeof(char*));
	if (readsList == NULL) {
		printf("\nERROR: Cannot allocate memory for readsList.\n");
		return (-1);
	}
	
	int readIndex = 0;
	int currReadLen = 0;
	int maxReadLen = 0;
	
	// read in reads, names, and make a readsList
	do {
		inchar = fgetc(readsfile);
		if (inchar == '>') {		// read in a name
			inchar = fgetc(readsfile);
			readsList[readIndex] = &readBuffer[readsLength];
			++readIndex;
			while(inchar != '\n' && inchar != EOF) {
				readBuffer[readsLength] = inchar;
				++readsLength;
				inchar = fgetc(readsfile);
			}
		}
		else if(inchar != EOF){
			readsList[readIndex] = &readBuffer[readsLength];
			++readIndex;
			while (inchar != '\n' && inchar != EOF) {
				readBuffer[readsLength] = inchar;
				++readsLength;
				++currReadLen;
				inchar = fgetc(readsfile);
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

	// read in alphabet
	do {
		inchar = fgetc(alphafile);
		if (inchar != ' ' && inchar != '\n' && inchar != EOF) {
			alphabet[alphabetLength] = inchar;
			++alphabetLength;
		}
	} while (inchar != EOF);
	alphabet[alphabetLength] = '\0';

	if (configfile) {
		while(fgets(paramLine, 20, configfile)){ // maximum 10 characters required
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
		}
		printf("Scoring Parameters: Match = %d, Mismatch = %d, h = %d, g = %d, lambda = %d\n\n", ma, mi, h, g, lambda);
	}

	fclose(inputfile);
	fclose(readsfile);
	fclose(alphafile);
	fclose(configfile);

	return (0);
}


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

	Node *tree;

	if (argc < 4) {
		printf("\nERROR: Incorrect number of arguments.\n");
		return -1;
	}

    printf("\nThe arguments are:\n\n");
	printf("Reference File:\t%s\n", argv[1]);
    printf("Reads File:\t%s\n", argv[2]);
	printf("Input Alphabet:\t%s\n", argv[3]);

	readInput(argv);

	tree = buildTree();

	if(prepareST(tree) == -1){
		printf("\nERROR: Couldn't prepare ST\n");
		return -1;
	}

	printf("%d\n%d",tree -> startLeafIndex, tree -> endLeafIndex);
	deallocate_memory(tree);

    return 0;
}