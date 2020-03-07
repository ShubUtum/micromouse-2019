#ifndef MAZESOLVER_H
#define	MAZESOLVER_H

#include "coordinate.h"
#include "stack.h"
#include "mazeHandler.h"
#include "mouse.h"

#define FULL_EXPLORATION	1

void initMazeSolver();
void rightHandRule(mouse_action* actions, int walls);
void floodFill(mouse_action* actions, int walls);

void shortestPath(mouse_action* actions);

int isExplorationDone();

#endif	/* MAZESOLVER_H */