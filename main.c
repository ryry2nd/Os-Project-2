#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9

#define NUMJOBS SIZE * 3

typedef struct {
    int **board;
    int currThread; // the id that shows which part of the board to do
	int maxthread; // the maximum amount of threads being run
	int *isValid; // the final flag that says if the board is valid or not. if it is ever 0 the program terminates immediately
} ThreadArgument;

// Reads from the file and sends it to the board pointer
int readBoard(int **board){
    FILE *file = fopen("input.txt", "r"); // Open the file for reading
    if (file == NULL) { //confirm file opened successfully
        printf("Error opening file.\n");
        return 1;
    }

    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(fscanf(file, "%d", board[i] + j) != 1){
                printf("Error reading board from file.\n");
                fclose(file);
                return 1;
            }
        }
    }

    fclose(file);
    return 0;
}

// debug prints out the board
void printBoard(int **board){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            printf("%d ", board[i][j]); // Print each number in the row
        }

        printf("\n"); //move to next line after printing each row
    }
}

// uses malloc to allocate the memory for the board size
int **allocBoard() {
	// Allocate the pointer array first
	int **board = (int**)malloc(SIZE * sizeof(int*));

	// Then allocate each row
	for(int i = 0; i < SIZE; i++) {
		board[i] = (int*)malloc(SIZE * sizeof(int));
	}

	return board;
}

// frees all the memory the board uses without causing segfaults or leaks
void deallocBoard(int **board) {
	for(int i = 0; i < SIZE; i++) {
		free(board[i]); // free each row
	}
	free(board); // free the pointer array itself
}


//check each row of the board for validity
void *rowCheck(void *arg) {
    //convert void pointer to ThreadArgument pointer
    ThreadArgument *a = (ThreadArgument *) arg;

    int **board = a->board;

	int overallValid = 1;

    for(int row = 0; row < SIZE; row++){
        int seen[9] = {0}; //array to track seen numbers in the row
        int valid = 1;

        for(int col = 0; col < SIZE; col++){
            int value = board[row][col];

            if(value < 1 || value > 9){
                valid = 0;
                break;
            }

            //if we have seen the number already the row is invalid
            if(seen[value-1] == 1){
                valid = 0;
                break;
            }
            //mark as seen
            seen[value-1] = 1;
        }

        if(!valid){
			overallValid = 0;
		}
    }

	if(overallValid == 0){
		*a->isValid = 0;
	}

    return NULL;
}

//check each column of the board for validity
void *columnCheck(void *arg) {
    //convert void pointer to ThreadArgument pointer
    ThreadArgument *a = (ThreadArgument *) arg;

    int **board = a->board;

	int overallValid = 1;

    for(int col = 0; col < SIZE; col++){
        int seen[9] = {0}; //array to track seen numbers in the column
        int valid = 1;

        for(int row = 0; row < SIZE; row++){
            int value = board[row][col];

            if(value < 1 || value > 9){
                valid = 0;
                break;
            }

            //if we have seen the number already the column is invalid
            if(seen[value-1] == 1){
                valid = 0;
                break;
            }
            //mark as seen
            seen[value-1] = 1;
        }

        if(!valid){
            overallValid = 0;
        }
    }

	if(overallValid == 0){
		*a->isValid = 0;
	}

    return NULL;
}

//check each 3x3 box of the board for validity
void *boxCheck(void *arg) {
	ThreadArgument *a = (ThreadArgument *)arg;
	int **board = a->board;

	int overallValid = 1;

	for(int box = 0; box < SIZE; box++){
		int seen[9] = {0};
		int valid = 1;

		int startRow = (box/3) * 3;
		int startCol = (box % 3) * 3;

		//check 3 rows
		for(int row = startRow; row < startRow + 3; row++){
			//check 3 columns
			for(int col = startCol; col < startCol + 3; col++){
				int value = board[row][col];

				if(value < 1 || value > 9){
					valid = 0;
					break;
				}

				if(seen[value - 1] == 1){
					valid = 0;
					break;
				}
				seen[value - 1] = 1;
			}

			if(valid == 0){
				break;
			}
		}

		if(!valid){
			overallValid = 0;
		}
	}
	if(overallValid == 0){
		*a->isValid = 0;
	}

	return NULL;
}

// returns 1 if valid, returns 0 if not valid
int rowcheck(int **board, int rownum) {
	printf("Row: %d ", rownum + 1);
	return 1;
}
int colcheck(int **board, int colnum) {
	printf("Col: %d ", colnum + 1);
	return 1;
}
int boxcheck(int **board, int boxnum) {
	printf("Box: %d ", boxnum + 1);
	return 1;// fun math fact the number of boxes is always equal to SIZE
}

// each thread runs this. it splits each job as even as possible for the amount of threads
void *workerThread(void *arg) {
	ThreadArgument *a = (ThreadArgument *)arg;
	int **board = a->board;
	int *isValid = a->isValid;
	int currThread = a->currThread;
	int maxThread = a->maxthread;

	int base_size = NUMJOBS / maxThread;
    int remainder = NUMJOBS % maxThread;

    int start = currThread * base_size + (currThread < remainder ? currThread : remainder); // The start has to add the remainders assigned to previous threads
    int size = base_size + (currThread < remainder ? 1 : 0); // The size has to add remainders to the size in the first threads

	if (size == 0) {
		// too many threads allocated for the work done.
		return NULL;
	}

	printf("Thread %d running: ", currThread); // debug statement

	for (int i = start; i < start + size && i < NUMJOBS; i++) {
		if (!*isValid) {
			return NULL; // if the board is no longer valid there isn't any point in continuing
		}
		int check = 1;
		if (i < SIZE) {
			check = rowcheck(board, i);
		}
		else if (i < SIZE * 2) {
			check = colcheck(board, i - SIZE);
		}
		else if (i < SIZE * 3) {
			check = boxcheck(board, (i - SIZE * 2));
		}
		if (!check) {
			*isValid = 0;
			return NULL;
		}
	}

	printf("\n");

	return NULL;
}

// it takes in the board and the max amount of threads you want and returns if it is valid or not
int checkBoard(int **board, int maxThreads) {
	if (maxThreads <= 0) {
		printf("maxThreads can only be 1 or higher\n");
		return -1;
	}

	pthread_t threadPids[maxThreads];
	ThreadArgument threadArgs[maxThreads];

	int isValid = 1;

	int i;

	for (i = 0; i < maxThreads; i++) {
		threadArgs[i].board = board;
		threadArgs[i].isValid = &isValid;
		threadArgs[i].currThread = i;
		threadArgs[i].maxthread = maxThreads;
		pthread_create(&threadPids[i], NULL, workerThread, &threadArgs[i]);
	}

	for (i = 0; i < maxThreads; i++) {
		if (threadPids[i] != 0) {
			pthread_join(threadPids[i], NULL);
		}
	}

	return isValid;
}


int main(int argc, char **argv) {
    //use command line to check which version to run using 1 or 2
	int version;
	if (argc < 2) {
		printf("Warning: invalid option. Type 1 or 2\n");
		version = 1;
	}
	else{
		version = atoi(argv[1]);
		if (version != 1 && version != 2) {
			printf("Warning: invalid option. Type 1 or 2\n");
			version = 1;
		}
	}

	printf("Running version %d\n", version);

    int **board = allocBoard();

    if(readBoard(board)){ //read board from file and check for success
        printf("Failed to read the board from the file.\n");
        return 1;
    }

	int isValid = checkBoard(board, 9);

	if (isValid == 0) {
		printf("Board is considered invalid\n");
	}
	else if (isValid == 1) {
		printf("Board is considered valid\n");
	}


	deallocBoard(board);
    return 0;
}