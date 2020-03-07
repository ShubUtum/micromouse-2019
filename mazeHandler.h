#ifndef MAZEHANDLER_H
#define	MAZEHANDLER_H

#include "coordinate.h"
#include <stdlib.h>
#include <limits.h>

#define NORTH         0x01
#define EAST          0x02
#define SOUTH         0x04
#define WEST          0x08
#define VISITED		  0x10
#define IN_STACK	  0x20

#define MAZE_WIDTH	16
#define MAZE_HEIGHT	0 //TODO

extern const Coordinate GOALS[4];
extern const Coordinate START;

extern int maze_memory[MAZE_WIDTH * MAZE_WIDTH];
extern Coordinate currentMazeLocation;
extern int currentDirection;

int flattenCoords(int x, int y);
int checkIfGoalCoord(int x, int y);

void updateMaze(Coordinate* current, int walls);
void initMaze();

void printMaze(int* maze, Coordinate* current, int dir);

#endif	/* MAZEHANDLER_H */
