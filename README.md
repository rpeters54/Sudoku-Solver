# Sudoku-Solver

Reads in an unsolved sudoku puzzle from infile,
solves it, and then outputs the solution to outfile.
The amount of guesses used to solve the puzzle is
also specified below the puzzle solution in outfile.

Infile Criteria:
- Must be properly formatted (examples given in repo)
- Should have a valid solution, Unsolvable puzzles 
  will throw an error

usage: command [infile] [outfile] 
(if outfile not specified, output is sent to stdout)
