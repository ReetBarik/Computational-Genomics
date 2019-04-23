#include "Header.h"

// extract sub string sequence[j - l ....... j + l]
char *extractSubString(int j, int len){
    int start = 0, end = sequenceLength, i = 0;
    char *sub_string = NULL;

    if(j - len > 0)                             // starting boundary condition
        start = j - len;        

    if(j + len < sequenceLength)                // ending boundary condition
        end = j + len;        

    sub_string = (char *)calloc((end - start + 1) , sizeof(char));

    if (sub_string == NULL){
        printf("\nERROR: Couldn't allocate substring!\n");
        exit(1);
    }
    
    for (i = start; i <= end; i++) {
        sub_string [i - start] = sequence[i];   // copy from reference genome
    }
    return sub_string;
}


int mapReads(Node *node){
    int i, j, start, end;
    int max_j, max_j_val = 0;
    int val;
    Node *subTreeRoot = NULL;
    char *subString;
    int k;
    int count = 0;

    // going through all reads
    for (i = 1; i < totalReads * 2; i += 2) {
        subTreeRoot = findLoc(node, readsList[i]);
        start = 0;
        end = sequenceLength - 2;
        if (subTreeRoot != NULL){               // lambda condition not satisfied
            max_j = A[subTreeRoot -> startLeafIndex];
            max_j_val = 0;

            // enumerating all the locations corresponding to the leaves
            for (j = subTreeRoot -> startLeafIndex; j <= subTreeRoot -> endLeafIndex; j++) {
                subString = extractSubString(A[j], strlen(readsList[i]));
                s1 = subString;
                s2 = readsList[i];

                // percent identity
                val = align(s1, s2);
                countAlign++;

                // check if % identity is more than X%
                if (val != -1){
                    if (val >= X){
                        if (val > max_j_val){
                            max_j_val = val;
                            max_j = A[i];
                        }
                    } 
                }
                for (k = 0; k < (int)strlen(s1) + 1; k++){
                    free(Matrix[k]);
                }
                free(Matrix);
            }

            if (max_j_val == 0) {
                printf("%s %s\n",readsList[i - 1], "No hits found");
            } else {
                if (max_j - strlen(readsList[i]) > 0)
                    start = max_j - strlen(readsList[i]);

                if (max_j + strlen(readsList[i]) < sequenceLength)
                    end = max_j + strlen(readsList[i]);
                printf("%s %d %d\n", readsList[i - 1], start, end);
                countHits++;

            }
        } else {
            printf("%s %s\n",readsList[i - 1], "No hits found");
        }        
    }
    return 0;
}


int mapReadsTest(Node *node){  // used to debug findLoc implementation
    int i, j, start, end;
    int max_j, max_j_val = 0;
    int val;
    Node *subTreeRoot = NULL;
    char *subString;
    int k;
    for (i = 1; i < totalReads * 2; i += 2) {
        subTreeRoot = findLoc(node, readsList[i]);
        printf("%d\n%d\n", subTreeRoot -> startLeafIndex, subTreeRoot -> endLeafIndex);
        printf("%d\n", subTreeRoot -> depth);
        printf("%c\n%c\n", sequence[subTreeRoot -> suffixHead], sequence[subTreeRoot -> suffixTail]); 
    }
    return 0;
}