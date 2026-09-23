#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9

typedef struct {
    int **board;
    int num;
} ThreadArgument;

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

void printBoard(int **board){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            printf("%d ", board[i][j]); // Print each number in the row
        }

        printf("\n"); //move to next line after printing each row
    }
}

int **allocBoard() {
	// Allocate the pointer array first
	int **board = (int**)malloc(SIZE * sizeof(int*));

	// Then allocate each row
	for(int i = 0; i < SIZE; i++) {
		board[i] = (int*)malloc(SIZE * sizeof(int));
	}

	return board;
}

void deallocBoard(int **board) {
	for(int i = 0; i < SIZE; i++) {
		free(board[i]); // free each row
	}
	free(board); // free the pointer array itself
}

void *threadFuncTest(void *arg) {
	ThreadArgument *a = (ThreadArgument *) arg;
	int **board = a->board;
	int num = a->num;

	printBoard(board);
	printf("The Argument is %d\n", num);
	return NULL;
}

//check each row of the board for validity
void *rowCheck(void *arg){
    //convert void pointer to ThreadArgument pointer
    ThreadArgument *a = (ThreadArgument *) arg;
    
    int **board = a->board;

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
        
        if(valid){
            printf("Row %d is valid\n", row + 1);
        }
        else{
            printf("Row %d is invalid\n", row + 1);
        }
        
    }

    return NULL;
}

int main(int argc, char **argv){
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

	ThreadArgument args = {};

	args.board = board;
	args.num = 5; 

	pthread_t tid;
	pthread_create(&tid, NULL, threadFuncTest, (void*)&args);
	pthread_join(tid, NULL);
    
    pthread_t rowThread;
    pthread_create(&rowThread, NULL, rowCheck, (void*)&args);
    pthread_join(rowThread, NULL);

	deallocBoard(board);
    return 0;
}