//
// Created by Riley Peters on 10/22/22.
//

#ifndef SUDOKU_SUDOKUCHECKER_H
#define SUDOKU_SUDOKUCHECKER_H

#define Columns 9
#define Rows 9
#define Squares 9
#define BoardSize 81

typedef struct Cell Cell;

/*
 * Cell struct:
 * - value -> stores the 1-9 value visible on the sudoku board (0 if empty)
 * - row -> the row which the cell is located in (0-8)
 * - col -> the column which the cell is located in (0-8)
 * - noteCount -> If the cell's value is unknown (0), noteCount
 *   measures how many numbers could be placed in that location
 * - notes -> Array of size 9 that stores either a 1 or 0 in each location
 *   to keep track of what numbers could viably go in the cell. Note that
 *   the values are equal to the index in the array + 1, because the
 *   indices go from 0 to 8.
 */
struct Cell {
    int value;
    int row;
    int col;
    int noteCount;
    int notes[9];
};

/*
 * Function Headers
 */

int initBoard(FILE *file);    //loader functions
void outputBoard(FILE *file);
void definePaths();
void usage(char *arg);

int testBoard();    //completion test functions
int trivialTest();
int setTest();

void printBoard();  //debugging/printer functions
void printCell(struct Cell *cell);

int annotateBoard(); //solver functions
int solve();
int solveLoop();

int solveAll();

int solveSet(struct Cell *cells[9]);
int singleNoteSolve(struct Cell *cells[9]);
int singleCellSolve(struct Cell *cells[9]);

void assignValue(struct Cell *cell, int val);

void setEmpty(struct Cell *cell);  //initialize and delete cell note values
void setFull(struct Cell *cell);

int updateAll();

void updateCell(struct Cell *cell);

int updateSet(int set);
int singleCheck(struct Cell *cells[9]);
void initUpdater(struct Cell *cells[9], int updater[9]);
int clearConflicts(struct Cell *cells[9], const int updater[9]);
int groupCheck(struct Cell *cells[9]);
int groupCancel(struct Cell *cells[9], int *indices, int noteCount);


int getRow(int square, int pos);    //math expressions to convert between (row, col) coords
int getCol(int square, int pos);    //to (square, pos) coords
int getSq(int row, int col);        //(row, col) coords are used whenever possible
int equalsArr(const int *arr1, const int *arr2, int maxLength);
int inArray(int num, const int *arr, int length);

int guess();
struct Cell *bestGuess();

void copyBoard(struct Cell mainBoard[9][9], struct Cell boardCopy[9][9]);
void copyArr(int *import, int *export, int size);
void copyCell(struct Cell *import, struct Cell *export);

#endif //SUDOKU_SUDOKUCHECKER_H
