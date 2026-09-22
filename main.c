#include <stdio.h>
#include <stdlib.h>

#define SIZE 9

int readBoard(int board[SIZE][SIZE]){
    FILE *file = fopen("input.txt", "r"); // Open the file for reading
    if (file == NULL) { //confirm file opened successfully
        printf("Error opening file.\n");
        return 0; // Return 0 to indicate failure
    }
    
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(fscanf(file, "%d", &board[i][j]) != 1){
                printf("Error reading board from file.\n");
                fclose(file); // Close the file before returning
                return 0; // Return 0 to indicate failure
            }
        }
    }

    fclose(file); // Close the file after reading
    return 1; // Return 1 to indicate success
}

void printBoard(int board[SIZE][SIZE]){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            printf("%d ", board[i][j]); // Print each number in the row
        }

        printf("\n"); //move to next line after printing each row
    }
}
int main(int argc, char *argv[]){
    //use command line to check which version to run using 1 or 2
    if((argv[1], 1) == 0){
        printf("Running version 1\n");
    }
    else if((argv[1], 2) == 0){
        printf("Running version 2\n");
    }
    int board[SIZE][SIZE]; //declare a 2D array to hold the Sudoku board

    if(readBoard(board) == 0){ //read board from file and check for success
        printf("Failed to read the board from the file.\n");
        return 1; // Exit the program if reading the board failed
    }

    printBoard(board);

    return 0; // Exit the program successfully
}