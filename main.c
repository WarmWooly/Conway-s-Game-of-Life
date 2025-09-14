/*
Author: Alexander Shapiro
Date: 1/21/25
Purpose: It's just Conway's Game of Life
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Constant values
#define UNDERPOPULATION_COUNT 1 // The maximum number of cells to die of underpopulation (1 by default)
#define OVERPOPULATION_COUNT 4 // The minimum number of cells to die of overpopulation (4 by default)
#define REPRODUCTION_COUNT 3 // The exact number of cells needed to birth a new cell (3 by default)
#define SIMULATION_SPEED 100000000 // The speed of the simulation (100000000 by default)
#define SIMULATION_SPEED_MODIFIER 1.0 // A user-friendly modifier value for the simulation speed (1.0 by default)
#define BOARD_HEIGHT 10 // Height of the game board (10 by default; 35 for fullscreen)
#define BOARD_WIDTH 10 // Width of the game board (10 by default; 66 for fullscreen)
#define SHOW_DEAD_CELLS 0 // Shows dead cells with '░' if 1 or not if 0 (0 by default)
#define DISPLAY_STATS 1 // Displays the simulation stats if 1 or not if 0 (1 by default)
#define MAX_SIMULATION_STEPS -1 // Number of steps when the simulation ends, -1 for no limit (-1 by default)
#define READ_FROM_FILE 0 // Determines if a data file is read to set the starting conditions if 1 or not if 0 (0 by default)

// PRE-CONDITION: N/A
// POST-CONDITION: Fills a board maxtrix with cells from start.txt
void read_board(int board[BOARD_HEIGHT][BOARD_WIDTH]);

// PRE-CONDITION: N/A
// POST-CONDITION: Fills a board maxtrix with cells randomly alive or dead
void generate_board(int board[BOARD_HEIGHT][BOARD_WIDTH]);

// PRE-CONDITION: A valid board state is present
// POST-CONDITION: Updates the current step with the previous step based on the rules of Conway's Game of Life
void update_board(int currentStepBoard[BOARD_HEIGHT][BOARD_WIDTH], int previousStepBoard[BOARD_HEIGHT][BOARD_WIDTH]);

// PRE-CONDITION: N/A
// POST-CONDITION: Returns the number of cells surrounding the selected cell
int surrounding_cells(int board[BOARD_HEIGHT][BOARD_WIDTH], int x, int y);

// PRE-CONDITION: N/A
// POST-CONDITION: Returns the number of living cells on the board
int living_cells(int board[BOARD_HEIGHT][BOARD_WIDTH]);

// PRE-CONDITION: N/A
// POST-CONDITION: Prints the board to the screen
void print_board(int board[BOARD_HEIGHT][BOARD_WIDTH], int step);

// PRE-CONDITION: DISPLAY_STATS == 1 during print step
// POST-CONDITION: Prints the simulation stats to the screen
void display_stats(int board[BOARD_HEIGHT][BOARD_WIDTH], int step);

int main(int argc, char* argv[]) {
	// Seeds the RNG with the current time
	srand(time(0));

	// Create empty arrays to store the current and previous game step
	int currentStepBoard[BOARD_HEIGHT][BOARD_WIDTH], previousStepBoard[BOARD_HEIGHT][BOARD_WIDTH];
	int gameStep = 0; // Keeps track of steps since simulation start

	if (READ_FROM_FILE) { // Populate board from start.txt
		read_board(currentStepBoard);
	} else { // Randomly populate the board with cells
		generate_board(currentStepBoard);
	}

	int betweenStep = INT_MAX;

	while(MAX_SIMULATION_STEPS >= gameStep || MAX_SIMULATION_STEPS == -1) { // Runs to the step count
		if (betweenStep < (SIMULATION_SPEED / SIMULATION_SPEED_MODIFIER)) {
			betweenStep++;
		} else {
			print_board(currentStepBoard, gameStep);
			gameStep++;
			//previousStepBoard = currentStepBoard;
			update_board(currentStepBoard, previousStepBoard);
			betweenStep = 0;
		}
	}

	return 0;
}

void read_board(int board[BOARD_HEIGHT][BOARD_WIDTH]) {
	FILE* fp;
	fp = fopen("start.txt", "r");
	if (fp == NULL) {
		printf("Unable to open start.txt\n");
		exit(1);
	}

	int eof;

	for (int heightIndex = 0; heightIndex < BOARD_HEIGHT; heightIndex++) {
		for (int widthIndex = 0; widthIndex < BOARD_WIDTH; widthIndex++) {
			char character_holder;
			eof = fscanf(fp, " %c ", &character_holder); // Gets the 0s (dead) or 1s (alive) from start.txt for the board
			board[heightIndex][widthIndex] = (character_holder == '1') ? 1 : 0;
			if (eof < 1) {
				printf("Invalid number of cells in start.txt (only found %d out of %d)\n", heightIndex * BOARD_WIDTH + widthIndex, BOARD_WIDTH * BOARD_HEIGHT);
				exit(1);
			}
		}
	}

	fclose(fp);
	return;
}

void generate_board(int board[BOARD_HEIGHT][BOARD_WIDTH]) {
	for (int heightIndex = 0; heightIndex < BOARD_HEIGHT; heightIndex++) {
		for (int widthIndex = 0; widthIndex < BOARD_WIDTH; widthIndex++) {
			board[heightIndex][widthIndex] = rand() % 2; // Randomly sets the tile's value to 0 (dead) or 1 (alive)
		}
	}
}

void update_board(int currentStepBoard[BOARD_HEIGHT][BOARD_WIDTH], int previousStepBoard[BOARD_HEIGHT][BOARD_WIDTH]) {
	for (int heightIndex = 0; heightIndex < BOARD_HEIGHT; heightIndex++) {
		for (int widthIndex = 0; widthIndex < BOARD_WIDTH; widthIndex++) {
			previousStepBoard[heightIndex][widthIndex] = currentStepBoard[heightIndex][widthIndex]; // Sets the previous board
		}
	}

	for (int heightIndex = 0; heightIndex < BOARD_HEIGHT; heightIndex++) {
		for (int widthIndex = 0; widthIndex < BOARD_WIDTH; widthIndex++) {
			int living_cells = surrounding_cells(previousStepBoard, widthIndex, heightIndex); // Gets surrounding cells
			if (previousStepBoard[heightIndex][widthIndex] && (living_cells <= UNDERPOPULATION_COUNT || living_cells >= OVERPOPULATION_COUNT)) { // Kill cells from over/underpopulation
				currentStepBoard[heightIndex][widthIndex] = 0;
			} else if (!previousStepBoard[heightIndex][widthIndex] && living_cells == REPRODUCTION_COUNT) { // Birth cells through reproduction
				currentStepBoard[heightIndex][widthIndex] = 1;
			}
		}
	}

	return;
}

int surrounding_cells(int board[BOARD_HEIGHT][BOARD_WIDTH], int x, int y) {
	int cells_found = 0;
	for (int heightCheck = -1; heightCheck <= 1; heightCheck++) {
		for (int widthCheck = -1; widthCheck <= 1; widthCheck++) {
			if (heightCheck != 0 || widthCheck != 0) { // Avoid counting self
				int tempHeightCheck = heightCheck, tempWidthCheck = widthCheck;
				// Loop around edges of the board
				if (y + tempHeightCheck >= BOARD_HEIGHT) { tempHeightCheck = 1 - BOARD_HEIGHT; }
				else if (y + tempHeightCheck < 0) { tempHeightCheck = BOARD_HEIGHT - 1; }
				if (x + tempWidthCheck >= BOARD_WIDTH) { tempWidthCheck = 1 - BOARD_WIDTH; }
				else if (x + tempWidthCheck < 0) { tempWidthCheck = BOARD_WIDTH - 1; }
				if (board[y + tempHeightCheck][x + tempWidthCheck]) { cells_found++; } // Mark cell as alive
			}
		}
	}
	return cells_found;
}

int living_cells(int board[BOARD_HEIGHT][BOARD_WIDTH]) {
	int cells = 0;
	for (int heightIndex = 0; heightIndex < BOARD_HEIGHT; heightIndex++) {
		for (int widthIndex = 0; widthIndex < BOARD_WIDTH; widthIndex++) {
			if (board[heightIndex][widthIndex]) { cells++; } // Counts the living cell
		}
	}
	return cells;
}

void print_board(int board[BOARD_HEIGHT][BOARD_WIDTH], int step) {
	system("cls"); // Clears the current screen
	if (DISPLAY_STATS) { display_stats(board, step); } // Display the simulation's stats if true

	for (int heightIndex = 0; heightIndex < BOARD_HEIGHT; heightIndex++) {
		for (int widthIndex = 0; widthIndex < BOARD_WIDTH; widthIndex++) {
			if (board[heightIndex][widthIndex]) { printf("%c%c", 219, 219); } // Prints the alive cell with '██'
			else {
				if (SHOW_DEAD_CELLS) { printf("%c%c", 176, 176); } // Prints the dead cell with '░░'
				else { printf("  "); } // Keeps dead cells hidden
			} 
		}
		printf("\n"); // Creates a new line for the next row
	}

	return;
}

void display_stats(int board[BOARD_HEIGHT][BOARD_WIDTH], int step) {
	printf("There are %d cells alive on game step %d\n", living_cells(board), step); // Displays the cell count
	return;
}
