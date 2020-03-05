#include "mazeHandler.h"

int maze_memory[MAZE_WIDTH * MAZE_WIDTH];

int flattenCoords(int col, int row) {
	return row + MAZE_WIDTH * col;
}

void updateMaze(Coordinate* current, int walls) {
	maze_memory[flattenCoords(current->x, current->y)] = maze_memory[flattenCoords(current->x, current->y)] | walls;

	if (((walls & NORTH) != 0) && (current->y + 1 < MAZE_WIDTH)) {
		maze_memory[flattenCoords(current->x, current->y + 1)] = maze_memory[flattenCoords(current->x, current->y + 1)] | SOUTH;

	}
	if (((walls & SOUTH) != 0) && (current->y - 1 >= 0)) {
		maze_memory[flattenCoords(current->x, current->y - 1)] = maze_memory[flattenCoords(current->x, current->y - 1)] | NORTH;

	}
	if (((walls & EAST) != 0) && (current->x + 1 < MAZE_WIDTH)) {
		maze_memory[flattenCoords(current->x + 1, current->y)] = maze_memory[flattenCoords(current->x + 1, current->y)] | WEST;
	}
	if (((walls & WEST) != 0) && (current->x - 1 >= 0)) {
		maze_memory[flattenCoords(current->x -1, current->y)] = maze_memory[flattenCoords(current->x - 1, current->y)] | EAST;
	}
}

void initMaze(Coordinate goals[]) {
	int distance;
    int x;
    int y;
	for (x = 0; x < MAZE_WIDTH; x++) {
		for (y = 0; y < MAZE_WIDTH; y++) {
			distance = INT_MAX;
            int i;
			for (i = 0; i < sizeof(goals); i++) {
                int temp_distance = abs(x - goals[i].x) + abs(y - goals[i].y);
				distance = distance < temp_distance ? distance : temp_distance;
			}
			maze_memory[flattenCoords(x, y)] = distance << 8;
		}
	}
}

// ------------------------------------------------------------------------------------
// print functions for debugging

#if 0
// Printing for PC debugging

void printNorthWalls(int* maze_memory, int row) {
	for (int col = 0; col < MAZE_WIDTH; col++) {
		printf("o");
		if (maze_memory[flattenCoords(col, row)] & NORTH) {
			printf("---");
		}
		else {
			printf("   ");
		}
	}
	printf("o\n");
}

void printSouthWalls(int* maze_memory, int row) {
	for (int col = 0; col < MAZE_WIDTH; col++) {
		printf("o");
		if (maze_memory[flattenCoords(col, row)] & SOUTH) {
			printf("---");
		}
		else {
			printf("   ");
		}
	}
	printf("o\n");
}

void printMaze(int* maze_memory, Coordinate* current, int dir) {
	for (int row = MAZE_WIDTH - 1; row >= 0; row--) {
		printNorthWalls(maze_memory, row);
		for (int col = 0; col < MAZE_WIDTH; col++) {
			if (maze_memory[flattenCoords(col, row)] & WEST) {
				printf("|");
			}
			else {
				printf(" ");
			}

			if (col == current->x && row == current->y) {
				switch (dir) {
				case NORTH: printf("^"); break;
				case EAST: printf(">"); break;
				case SOUTH: printf("v"); break;
				case WEST: printf("<"); break;
				}
			}
			else if (maze_memory[flattenCoords(col, row)] & VISITED) {
				printf("*");
			}
			else {
				printf(" ");
			}

			int distance = maze_memory[flattenCoords(col, row)] >> 8;
			if (distance >= 100)
				printf("..");
			else
				printf("%2d", distance);
		}
		if (maze_memory[flattenCoords(MAZE_WIDTH - 1, row)] & EAST) {
			printf("|");
		}
		else {
			printf(" ");
		}
		printf("\n");
	}
	printSouthWalls(maze_memory, 0);
}

#else
// Logging for micro controller debugging

void printMaze(int* maze, Coordinate* current, int dir) {

}
#endif

