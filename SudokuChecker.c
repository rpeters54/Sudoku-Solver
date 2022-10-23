/*
 * SudokuChecker:
 * Solves sudoku puzzles using a combination of logical deduction and guessing
 *
 * Created by: Riley Peters
 * Version 1.3
 */

#include <stdio.h>
#include <stdlib.h>
#include "SudokuChecker.h"

#define STD_OUT 2
#define FILE_OUT 3

/*
 * Global Variables:
 * 1. board:
 * 9x9 array of Cells that stores the
 * data required to solve the sudoku puzzle
 * 2. paths:
 * 27x9 array of Cell pointers configured to show
 * all possible sets of cells.
 * 0-8 rows, 9-17 columns, 18-26 squares
 */
Cell board[Rows][Columns];
Cell *paths[27][9];
int guesses = 0;

/*
 * sudokuChecker:
 * - Takes in an input and output file
 * - if no output is specified, stdout is used
 * - Reads in a sudoku puzzle from infile
 * - Computes the solution and outputs it to outfile
 * - Solution file also includes the amount of guesses
 *   used to reach that solution
 */
int main(int argc, char *argv[]) {
    FILE *infile, *outfile;
    switch(argc) {
        case STD_OUT:
            outfile = stdout;
            break;
        case FILE_OUT:
            outfile = fopen(argv[2], "w");
            if (!outfile) {
                perror("Invalid Output File");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr, "Invalid Arguments");
            usage(argv[0]);
            break;
    }
    infile = fopen(argv[1], "r");
    if (!infile) {
        perror("Invalid Input File");
        exit(EXIT_FAILURE);
    }

    if (initBoard(infile) == -1) {
        fprintf(stderr, "Error: Improper Input Formatting\n"
                        "Make sure each cell is indicated by a number (1-9)\n"
                        "if filled, or space if empty, "
                        "followed by a comma.\n");
        exit(EXIT_FAILURE);
    }

    definePaths();
    if (!solve() && testBoard()) {
        outputBoard(outfile);
    } else {
        fprintf(stderr, "Could Not Compute a Solution");
    }

    return 0;
}

void usage(char *arg) {
    fprintf(stderr, "%s infile outfile", arg);
}

/*
 * Loads Sudoku board from an input text file
 * 1. Opens the file location defined at the top of the program
 * 2. Parses file for a numbers and commas
 * 3. For each board location, only the first numerical character is considered
 * 4. If an improper amount of characters is received, the program terminates
 *      with error code -1;
 */
int initBoard(FILE *file) {
    int i, c, modified, row, column;
    i = modified = 0;
    while(i < BoardSize && (c = fgetc(file)) != EOF) {
        row = i / 9;
        column = i % 9;
        if (modified == 0 && (c >= '1' && c <= '9')) {
            Cell currentCell;
            currentCell.value = (c - '0');
            currentCell.row = row;
            currentCell.col = column;
            board[row][column] = currentCell;
            modified = 1;
        } else if (c == ',') {
            if (modified == 0) {
                Cell currentCell;
                currentCell.value = 0;
                currentCell.row = row;
                currentCell.col = column;
                board[row][column] = currentCell;
            }
            modified = 0;
            i++;
        }
    }
    fclose(file);

    if (i != BoardSize) {
        return -1;   //error: board not filled
    }
    return 0;
}

void outputBoard(FILE *file) {
    int r, c;
    for (r = 0; r < Rows; r++) {
        for (c = 0; c < Columns; c++) {
            fprintf(file, "%d, ", board[r][c].value);
        }
        fputc('\n', file);
    }
    fprintf(file, "\nGuesses: %d\n", guesses);
    fclose(file);

}

void definePaths() {
    int i, r, c, s, p;
    i = 0;
    for (r = 0; r < Rows; r++) {
        for (c = 0; c < Columns; c++) {
            paths[i][c] = &board[r][c];
        }
        i++;
    }

    for (c = 0; c < Columns; c++) {
        for (r = 0; r < Rows; r++) {
            paths[i][r] = &board[r][c];
        }
        i++;
    }

    for (s = 0; s < Squares; s++) {
        for (p = 0; p < 9; p++) {
            paths[i][p] = &board[getRow(s, p)][getCol(s, p)];
        }
        i++;
    }
}

/*
 * Prints each board position, along with its value and notes
 *
 */
void printBoard() {
    int r, c, s, p, n;
    printf("Row-wise:\n");
    for (r = 0; r < Rows; r++) {
        for (c = 0; c < Columns; c++) {
            printf("(row: %d, column: %d) = %d\n\t", r, c, board[r][c].value);
            for (n = 0; n < 9; n++) {
                if (board[r][c].notes[n] != 0) {
                    printf("%d, ", n + 1);
                }
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("Column-wise:\n");
    for (c = 0; c < Columns; c++) {
        for (r = 0; r < Rows; r++) {
            printf("(column: %d, row: %d) = %d\n\t", c, r, board[r][c].value);
            for (n = 0; n < 9; n++) {
                if (board[r][c].notes[n] != 0) {
                    printf("%d, ", n + 1);
                }
            }
            printf("\n");
        }
        printf("\n");
    }
    printf("Square-wise:\n");
    for (s = 0; s < Squares; s++) {
        for (p = 0; p < 9; p++) {
            printf("(square: %d, pos: %d) = %d\n\t", s,
                   p, board[getRow(s, p)][getCol(s, p)].value);
            for (n = 0; n < 9; n++) {
                if (board[getRow(s, p)][getCol(s, p)].notes[n] != 0) {
                    printf("%d, ", n + 1);
                }
            }
            printf("\n");
        }
        printf("\n");
    }
    for (r = 0; r < Rows; r++) {
        for (c = 0; c < Columns; c++) {
            printf("%d, ", board[r][c].value);
        }
        printf("\n");
    }
}

void printCell(Cell *cell) {
    int i;
    printf("(row: %d, col: %d)\n", cell -> row, cell -> col);
    printf("Value: %d\n", cell -> value);
    for (i = 0; i < 9; i++) {
        if (cell -> notes[i] == 1) {
            printf("%d, ", i + 1);
        }
    }
    printf("\n");
}

/*
 * Runs multiple tests to confirm if a board is solved
 */
int testBoard() {
    return (trivialTest() && setTest());
}

int trivialTest() {
    int r, c;
    for (r = 0; r < Rows; r++) {
        for (c = 0; c < Columns; c++) {
            if (board[r][c].value == 0) {
                return 0;
            }
        }
    }
    return 1;
}

int setTest() {
    int i, n;
    for (i = 0; i < 27; i++) {
        int nums[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        for (n = 0; n < 9; n++) {
            nums[paths[i][n] -> value - 1] += 1;
        }
        for (n = 0; n < 9; n++) {
            if (nums[n] != 1) {
                return 0;
            }
        }
    }
    return 1;
}

int annotateBoard() {
    int r, c;
    for (r = 0; r < Rows; r++) {
        for (c = 0; c < Columns; c++) {
            if (board[r][c].value != 0) {
                setEmpty(&board[r][c]);
            } else {
                setFull(&board[r][c]);
            }
        }
    }
    return 0;
}

/*
 * Main Solver Function:
 * Runs a series of helper functions until all locations on the board are full
 * Once complete, returns 0
 */
int solve() {
    annotateBoard();
    return solveLoop();
}

int solveLoop() {
    while(trivialTest() == 0) {             //while empty locations remain:
        if (!updateAll() && !solveAll()) {  //try to reduce notes and solve cells
            return guess();
        }
        //printBoard();
    }
    return 0;  //solved without needing to guess
}

/*
 * SolveAll Functions:
 * Runs solve functions for all rows, columns, or squares
 * Used to improve readability
 */

int solveAll() {
    int i, check;
    check = 0;
    for (i = 0; i < 27; i++) {
        check += solveSet(paths[i]);
    }
    return check;
}

/*
 * Solve Functions:
 * 1. Run quick solve that checks if any of the Cells contain
 * only 1 value in their note array. If they do, the cell is
 * assigned a value and the solve method restarts.
 * 2. Run secondary solve that checks if there are any notes
 * that only appear in one cell
 * 3. Repeat function if either solve updates a value
 */
int solveSet(Cell *cells[9]) {
    int check = 0;
    RESTART:
    if (singleNoteSolve(cells)) {
        check = 1;
        goto RESTART;
    }
    if (singleCellSolve(cells)) {
        check = 1;
        goto RESTART;
    }
    return check;
}

int singleNoteSolve(Cell *cells[9]) {
    int i, n, check;
    check = 0;
    for (i = 0; i < 9; i++) {
        if (cells[i] -> noteCount == 1) {
            for (n = 0; n < 9; n++) {
                if (cells[i] -> notes[n]) {
                    break;
                }
            }
           // printf("SingleSolve\n");
            assignValue(cells[i], n + 1);
            check = 1;
        }
    }
    return check;
}

int singleCellSolve(Cell *cells[9]) {
    int n, i, loc, note, count, check;
    count = check = 0;
    for (n = 0; n < 9; n++) {
        for (i = 0; i < 9; i++) {
            if (cells[i] -> notes[n]) {
                count++;
                loc = i;
                note = n;
                if (count > 1) {
                    break;
                }
            }
        }
        if (count == 1) {
            assignValue(cells[loc], note + 1);
            check = 1;
        }
        count = 0;
    }
    return check;
}

/*
 * assignValue:
 *
 */
void assignValue(Cell *cell, int val) {
    //printCell(cell);
    cell -> value = val;
    setEmpty(cell);
    //printCell(cell);
    updateCell(cell);
}

/*
 * updateAll:
 * - updates all rows, columns, and squares
 */

int updateAll() {
    int i, check;
    check = 0;
    for (i = 0; i < 27; i++) {
        check += updateSet(i);
    }
    return check;
}

/*
 * updateCell:
 * - Updates the row, column, and square associated with a cell
 * - Used by the solver functions to update the cells surround a solved cell
 */
void updateCell(Cell *cell) {
    updateSet(cell -> row);
    updateSet(cell -> col + 9);
    updateSet(getSq(cell -> row, cell -> col) + 18);
}

/*
 *
 */
int updateSet(int set) {
    if (singleCheck(paths[set])) {
        return 1;
    }
    if (groupCheck(paths[set])) {
        return 1;
    }
    return 0;
}

int singleCheck(Cell *cells[9]) {
    int updater[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    initUpdater(cells, updater);
    return clearConflicts(cells, updater);
}

void initUpdater(Cell *cells[9], int updater[9]) {
    int i;
    for (i = 0; i < 9; i++) {
        if (cells[i] -> value != 0) {
            updater[cells[i] -> value - 1] = 0;
        }
    }
}

int clearConflicts(Cell *cells[9], const int updater[9]) {
    int i, n;
    int check = 0;
    for (i = 0; i < 9; i++) {
        if (cells[i] -> value == 0) {
            for (n = 0; n < 9; n++) {
                if (updater[n] == 0 && cells[i] -> notes[n] == 1) {
                    //printCell(cells[i]);
                    cells[i] -> notes[n] = 0;
                    cells[i] -> noteCount -= 1;
                    check = 1;
                    //printCell(cells[i]);
                }
            }
        }
    }
    return check;
}

int groupCheck(Cell *cells[9]) {
    int i, n, count, check;
    check = 0;
    count = 1;
    for (i = 0; i < 9; i++) {
        if (cells[i] -> noteCount > 1) {
            int *indices = (int*) malloc(cells[i] -> noteCount * sizeof(int));
            indices[0] = i;
            for (n = i + 1; n < 9; n++) {
                if(cells[i] -> noteCount == cells[n] -> noteCount &&
                   equalsArr(cells[i] -> notes, cells[n] -> notes, 9)) {
                    indices[count] = n;
                    count++;
                }
                if (count == cells[i] -> noteCount) {
                    check += groupCancel(cells, indices, cells[i] -> noteCount);
                }
                if ((9 - n) < (cells[i] -> noteCount - count)) {
                    break;
                }
            }
            count = 1;
            free(indices);
        }
    }
    return check;
}

int groupCancel(Cell *cells[9], int *indices, int noteCount) {
    //create an array noteValues that stores all of the groups note values
    int *noteValues = (int*) malloc(noteCount * sizeof(int));
    int i, n, count, sum, check;
    count = sum = check = 0;
    for (i = 0; i < 9; i++) {
        if (cells[indices[0]] -> notes[i]) {
            noteValues[count] = i;
            count++;
        }
    }
    /*
     * For cells that are not filled nor contained in the indices array:
     * - Set each note value stored in noteValues to zero.
     * - Use the "sum" variable to track if any values have been updated.
     * - Sum is used by the main update method to determine whether it has
     *   completed, or if it should run a more complex update algorithm.
     */
    for (i = 0; i < 9; i++) {
        if (cells[i] -> value == 0 &&
            !inArray(i, indices, noteCount)) {
            for (n = 0; n < noteCount; n++) {
                sum += cells[i] -> notes[noteValues[n]];
                cells[i] -> notes[noteValues[n]] = 0;
            }
            cells[i] -> noteCount -= sum;
            if (sum != 0) {
                check = 1;
            }
            sum = 0;
        }
    }
    free(noteValues);
    return check;
}

/*
 * Guess Functions
 */

/*
 * guess:
 * - Used to solve the board when all other solve functions are unsuccessful
 * Description:
 * 1. Copy the current board so that the guess can be reversed if incorrect
 * 2. Select a cell with the minimum amount of possible numbers (reduces guessing)
 * 3. For each possible guess, update the cell with the guessed value and reenter
 * the solve loop.
 *      - If the solve loop finds the solution, it will return a zero and break out of the guess function
 *      - If a solution is not found for a particular guess, the board is reset and the next guess is attempted
 *      - If no guess is viable, the funtion returns a -1. This usually happens if a prior guess was incorrect
 *      - Guess may be called recursively, if the board reaches another point where solve methods are unsuccessful
 */
int guess() {
    int i, solveVal;
    Cell boardCopy[9][9];
    copyBoard(board, boardCopy);
    Cell *cellGuess = bestGuess();
    for (i = 0; i < 9; i++) {
        if (cellGuess -> notes[i]) {
            guesses++;
            assignValue(cellGuess, i + 1);
            //printBoard();
            solveVal = solveLoop();
            if (solveVal == 0) {
                return 0;
            }
            copyBoard(boardCopy, board);
        }
    }
    return -1;
}

/*
 * bestGuess:
 * - Returns a pointer to the Cell bst suited for guessing
 * - The best Cell is one that has as few possible values as possible
 */
Cell *bestGuess() {
    int r, c, row, col, minNotes;
    minNotes = 10;
    for (r = 0; r < Rows; r++) {
        for (c = 0; c < Columns; c++) {
            if(board[r][c].value == 0 && board[r][c].noteCount < minNotes) {
                minNotes = board[r][c].noteCount;
                row = r;
                col = c;
            }
            if (minNotes == 2) {
                break;
            }
        }
    }
    return &board[row][col];
}

/*
 * Get Position Methods
 */

/*
 * getRow:
 * Returns the row of the cell associated with a certain square and position
 */
int getRow(int square, int pos) {
    return 3 * (square / 3) + (pos / 3);
}

/*
 * getCol:
 * Returns the column of the cell associated with a certain square and position
 */
int getCol(int square, int pos) {
    return 3 * (square % 3) + (pos % 3);
}

/*
 * getSq:
 * Returns the value of the square the cell is located in given the row and column
 */
int getSq(int row, int col) {
    return 3 * (row / 3) + (col / 3);
}

/*
 * Notes Initialize/Clear Functions
 */

/*
 * setEmpty:
 * - Sets the note array of the specified cell to all zeroes
 * - Sets the noteCount to zero
 */
void setEmpty(Cell *cell) {
    int i;
    for (i = 0; i < 9; i++) {
        cell -> notes[i] = 0;
    }
    cell -> noteCount = 0;
}

/*
 * setFull:
 * - Sets the note array of the specified cell to all ones
 * - Sets the noteCount to 9 (filled completely)
 */
void setFull(Cell *cell) {
    int i;
    for (i = 0; i < 9; i++) {
        cell -> notes[i] = 1;
    }
    cell -> noteCount = 9;
}

/*
 * Array Helper Functions
 */

/*
 * equalsArr: Checks if the data in arr1 is the same as arr2.
 * Assumes that arr1 and arr2 are integer arrays of equal length.
 * Returns 1 if equal, and 0 if not.
 */
int equalsArr(const int *arr1, const int *arr2, int length) {
    int i;
    for (i = 0; i < length; i++) {
        if (arr1[i] != arr2[i]) {
            return 0;
        }
    }
    return 1;
}

/*
 * inArray: Searches for a specified number in an array of known length (linear search)
 * Returns 1 if the number appears in the array, and 0 if not.
 */
int inArray(int num, const int *arr, int length) {
    int i;
    for (i = 0; i < length; i++) {
        if (num == arr[i]) {
            return 1;
        }
    }
    return 0;
}

/*
 * Copy Functions:
 * Used during the guess() function to save the current board before making a guess
 */

/*
 * copyBoard: Copies the cells from mainBoard to boardCopy
 * - note that the cells of boardCopy are distinct from mainBoard, they are NOT pointers
 * to mainBoard
 */
void copyBoard(Cell mainBoard[9][9], Cell boardCopy[9][9]) {
    int r, c;
    for (r = 0; r < Rows; r++) {
        for (c = 0; c < Columns; c++) {
            copyCell(&mainBoard[r][c], &boardCopy[r][c]);
        }
    }
}

/*
 * copyCell: copies the data from one cell to another.
 * Used by the copyBoard function
 */
void copyCell(Cell *import, Cell *export) {
    export -> value = import -> value;
    export -> row = import -> row;
    export -> col = import -> col;
    export -> noteCount = import -> noteCount;
    copyArr(import -> notes, export -> notes, 9);
}

/*
 * copyArr: copies the data from one integer array to another of the same size.
 * Used by the copyCell function to copy notes from one cell to another
 */
void copyArr(int *import, int *export, int size) {
    int i;
    for (i = 0; i < size; i++) {
        export[i] = import[i];
    }
}