#include "Header.h"

// helper function to return max of 3 integers
int max(int a, int b, int c){
	return (a > b ? (a > c ? a : c) : (b > c ? b : c));
}

// helper function to reverse a string
char* stringReverse(char *str){

	char *start = str;
	char *end = start+strlen(str)-1;

	while(start<end)
	{
		char temp = *end;
		*end = *start;
		*start=temp;
		start++;
		end--;
	}
	return str;
}

int chooseDirection(int i, int j){
	return max(Matrix[i][j].s, Matrix[i][j].d, Matrix[i][j].i);
}

// calculate the substitution score
int substitution(char *s1, char *s2, int i, int j){
	//printf("%d\t%c\t%d\t%c\n",i - 1, s1[i - 1], j - 1, s2[j - 1]);
	if (s1[i - 1] != s2[j - 1]) 
		return chooseDirection(i-1,j-1) + mi;
	else
		return chooseDirection(i-1,j-1) + ma;	
}

// dynamically allocate the memory for DP_cell matrix
int allocate(int m, int n){
	
	int i;	
	DP_cell **mat = (DP_cell **)malloc(m * sizeof(DP_cell *));
	if (mat == 0){
		printf("Error allocating memory");
		return -1;
	}
	for (i = 0; i < m; i++){
		mat[i] = (DP_cell *)malloc(n * sizeof(DP_cell));
		if(mat[i] == NULL){
			printf("Error allocating memory");
			return -1;
		}
	}
	Matrix = mat;
	return 0;
}

// initialise row 0 and col 0 of matrix
void initialiseMatrix(int m, int n){

	int i, j;

	Matrix[0][0].s = 0;
	Matrix[0][0].d = 0;
	Matrix[0][0].i = 0;
	
	for (i = 1; i <= m; i++){
		if (!SW){
			// global alignment
			Matrix[i][0].s = INT_MIN - (h + (2 * ((m > n) ? m : n) * g));
			Matrix[i][0].d = h + (g * (i));
			Matrix[i][0].i = INT_MIN - (h + (2 * ((m > n) ? m : n) * g));
		}
		else {
			// local alignment
			Matrix[i][0].s = 0;
			Matrix[i][0].d = 0;
			Matrix[i][0].i = 0;
		}
	}
	for (j = 1; j <= n; j++){
		if (!SW){
			// global alignment
			Matrix[0][j].s = INT_MIN - (h + (2 * ((m > n) ? m : n) * g));
			Matrix[0][j].d = INT_MIN - (h + (2 * ((m > n) ? m : n) * g));
			Matrix[0][j].i = h + (g * (j));
		}
		else {
			// local alignment
			Matrix[0][j].s = 0;
			Matrix[0][j].d = 0;
			Matrix[0][j].i = 0;
		}
	}
	return;
}

// retrace to form the alignment strings
int retrace(){
	
	int i , j;
	
	// start retrace from
	if(SW){
		// local alignment
		i = maxRowIndex;
		j = maxColIndex;
	}
	else{
		// global alignment
		i = strlen(s1);
		j = strlen(s2);
	}

	int ctr = 0, total, penalty, moveTo, numMa = 0, numMi = 0, numGa = 0, numOp = 0;		
	char str1[i + j], str2[i + j], str0[i + j];	//str1 to store sequence 1, str2 to store sequence 2, str0 to store '-'/'|'/' '	

	
	if(max(Matrix[i][j].s, Matrix[i][j].d, Matrix[i][j].i) == Matrix[i][j].s)
		moveTo = 1;
	else if(max(Matrix[i][j].s, Matrix[i][j].d, Matrix[i][j].i) == Matrix[i][j].d)
		moveTo = 2;
	else
		moveTo = 0;

	while(i > 0 || j > 0){
		if(SW){
			if(chooseDirection(i,j) == 0)
				break;
		}
		
		// deletion case
		if(moveTo == 2){
			// deletion case
			if (Matrix[i][j].d == (Matrix[i-1][j].d + g)){
				moveTo = 2;
			}
			// substitution case
			else if (Matrix[i][j].d == (Matrix[i-1][j].s + h + g)){
				moveTo = 1;
				numOp++;
			}
			// insertion case
			else {
				moveTo = 0;
				numOp++;
			}
			i--; // 1 char from sequence 1 is consumed
			str1[ctr] = s1[i];
			str2[ctr] = '-';
			str0[ctr] = ' ';
			numGa++;

		} 
		// substitution case
		else if(moveTo == 1){

			if(s1[i-1] != s2[j-1]){
				penalty = mi;
				numMi++;
			} 
			else{
				penalty = ma;
				numMa++;
			}
			// deletion
			if (Matrix[i][j].s == (Matrix[i-1][j-1].d + penalty)){
				moveTo = 2;
			}
			// substitution case
			else if (Matrix[i][j].s == (Matrix[i-1][j-1].s + penalty)){
				moveTo = 1;
			}
			// insertion case
			else {
				moveTo = 0;
			}
			i--; // 1 char from sequence 1 is consumed
			j--; // 1 char from sequence 2 is consumed
			str1[ctr] = s1[i];
			str2[ctr] = s2[j];
			if(penalty == ma)
				str0[ctr] = '|'; 
			else
				str0[ctr] = ' ';
		} 
		// insertion case
		else if(moveTo == 0){
			// deletion case
			if (Matrix[i][j].i == (Matrix[i][j-1].d + g + h)){					
				moveTo = 2;
				numOp++;
			}
			// substitution case
			else if (Matrix[i][j].i == (Matrix[i][j-1].s + g + h) ){					
				moveTo = 1;
				numOp++;
			}
			// insertion case
			else {
				moveTo = 0;
			}
			j--; // char from sequence 2 is consumed
			str1[ctr] = '-';
			str2[ctr] = s2[j];
			str0[ctr] = ' ';
			numGa++;
		}
		else
			return -1;
		
		ctr ++;
	}

	str1[ctr] = 0;
	str0[ctr] = 0;
	str2[ctr] = 0;

	// reverse the strings because they were read in reverse order
	stringReverse(str1);
	stringReverse(str0);
	stringReverse(str2);

	// total alignment length
	total = numMa + numMi + numGa;

    if (((total * 100) / strlen(s2)) < Y)			// LengthCoverage ≥ Y%
        return -1;
	return numMa * 100 / total;
}

// alignment algo for the 2 sequences
int align(char *seq1, char *seq2){

	int m = strlen(seq1);
	int n = strlen(seq2);
	int i, j;
	maxRowIndex = 0 , maxColIndex = 0;

	// allocate the matrix
	if (allocate(m+1, n+1) < 0)
		return -1;		

	// Initialise Row 1 and Column 1
	initialiseMatrix(m, n);

	for (i = 1; i <= m; i++){
		for (j = 1; j <= n; j++){
			
			if (SW) {
				// substitution
				Matrix[i][j].s = substitution(seq1, seq2, i, j) > 0 ? substitution(seq1, seq2, i, j) : 0;
				// deletion
				Matrix[i][j].d = (max(Matrix[i-1][j].s + h + g, Matrix[i-1][j].i + h + g, Matrix[i-1][j].d + g)) > 0 ? (max(Matrix[i-1][j].s + h + g, Matrix[i-1][j].i + h + g, Matrix[i-1][j].d + g)) : 0;
				// insertion
				Matrix[i][j].i = (max(Matrix[i][j-1].s + h + g, Matrix[i][j-1].i + g, Matrix[i][j-1].d + h + g)) > 0 ? (max(Matrix[i][j-1].s + h + g, Matrix[i][j-1].i + g, Matrix[i][j-1].d + h + g)) : 0;
			}
			else {
				// substitution
				Matrix[i][j].s = substitution(seq1, seq2, i, j);				
				// deletion
				Matrix[i][j].d = max(Matrix[i-1][j].s + h + g, Matrix[i-1][j].i + h + g, Matrix[i-1][j].d + g);
				// insertion
				Matrix[i][j].i = max(Matrix[i][j-1].s + h + g, Matrix[i][j-1].i + g, Matrix[i][j-1].d + h + g);
			}
		}
	}

	
	if (SW){

		// Contribution of Waterman to this field is the ZERO!!
		for (i = 1; i <= m; i++){
			for (j = 1; j <= n; j++){

				// Storing the index where the max score was encountered
				if (chooseDirection(i,j) > chooseDirection(maxRowIndex, maxColIndex)){
					maxRowIndex = i;
					maxColIndex = j;
				}
			}
		}
		// Local alignment retrace
		return retrace();
	}
	// Global alignment retrace
	return retrace();
}
