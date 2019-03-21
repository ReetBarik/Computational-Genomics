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

	unsigned int input_size = input_st.st_size;
	unsigned int alpha_size = alpha_st.st_size;

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

	unsigned int i = 0;

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

void cleanupTime(void) {
	free(ibuff);
	free(abuff);
}

int main (int argc, const char *argv[])
{
	struct timeval tstart, tstop;

	long vmrss, vmsize, startMem, endMem;

	Node *tree;


	if (argc < 3) {
		printf("\nERROR: Incorrect number of arguments.\n");
		return -1;
	}

	printf("\nThe arguments are:\n\n");
	printf("Input File:\t%s\n", argv[1]);
	printf("Input Alphabet:\t%s\n", argv[2]);

	
	if (setUp(argv) < 0) 
		return -1;
	
	
	get_memory_usage_kb(&vmrss, &vmsize);
    startMem = vmrss;
	gettimeofday(&tstart, NULL);
	tree = buildTree();		// build ST
	gettimeofday(&tstop, NULL);
    get_memory_usage_kb(&vmrss, &vmsize);
	endMem = vmrss;
	printf("\nST Construction Time: %f ms\n", diff_time(&tstart, &tstop));
	printf("ST Construction Space: \t%6ld (KB)\n", endMem - startMem);

	
	/* bwt(tree);	
	printf("\n");
	dfs(tree); */

	doNotBeLikeFirefox(tree);
	cleanupTime();
	return (0);
}