#include "maze_solver.h"

int backtrack;
Stack* updateStack;
Stack* cellStack;
Coordinate intersection;

int peakRight() {
	int col = currentMazeLocation.x;
	int row = currentMazeLocation.y;
	switch (currentDirection)
	{
	case NORTH: col++; break;
	case EAST: row--; break;
	case SOUTH: col--; break;
	case WEST: row++; break;
	}
	return maze_memory[flattenCoords(col, row)];
}

int peakLeft() {
	int col = currentMazeLocation.x;
	int row = currentMazeLocation.y;
	switch (currentDirection)
	{
	case NORTH: col--; break;
	case EAST: row++; break;
	case SOUTH: col++; break;
	case WEST: row--; break;
	}
	return maze_memory[flattenCoords(col, row)];
}

int peakForward() {
	int col = currentMazeLocation.x;
	int row = currentMazeLocation.y;
	switch (currentDirection)
	{
	case NORTH: row++; break;
	case EAST: col++; break;
	case SOUTH: row--; break;
	case WEST: col--; break;
	}
	return maze_memory[flattenCoords(col, row)];
}

int log2Function(int x) {
	if (x == NORTH)
		return 0;
	if (x == EAST)
		return 1;
	if (x == SOUTH)
		return 2;
	if (x == WEST)
		return 3;
    return -1;
}

void updateDistances() {
	push(updateStack, &currentMazeLocation);
	Coordinate temp;

	while (!isEmpty(updateStack)) {
		pop(updateStack, &temp);

		if (checkIfGoalCoord(temp.x, temp.y))
			continue;

		int current_distance = maze_memory[flattenCoords(temp.x, temp.y)] >> 8;
		int min_distance = INT_MAX;
		int temp_walls = maze_memory[flattenCoords(temp.x, temp.y)];
		if (((temp_walls & NORTH) == 0) && (temp.y + 1 < MAZE_WIDTH)) {
            min_distance = min_distance < (maze_memory[flattenCoords(temp.x, temp.y + 1)] >> 8) ? min_distance : (maze_memory[flattenCoords(temp.x, temp.y + 1)] >> 8);
		}
		if (((temp_walls & SOUTH) == 0) && (temp.y - 1 >= 0)) {
            min_distance = min_distance < (maze_memory[flattenCoords(temp.x, temp.y - 1)] >> 8) ? min_distance : (maze_memory[flattenCoords(temp.x, temp.y - 1)] >> 8);
		}
		if (((temp_walls & EAST) == 0) && (temp.x + 1 < MAZE_WIDTH)) {
            min_distance = min_distance < (maze_memory[flattenCoords(temp.x + 1, temp.y)] >> 8) ? min_distance : (maze_memory[flattenCoords(temp.x + 1, temp.y)] >> 8);
		}
		if (((temp_walls & WEST) == 0) && (temp.x - 1 >= 0)) {
            min_distance = min_distance < (maze_memory[flattenCoords(temp.x - 1, temp.y)] >> 8) ? min_distance : (maze_memory[flattenCoords(temp.x - 1, temp.y)] >> 8);
		}

		if (min_distance != current_distance - 1) {
			current_distance = min_distance + 1;
			maze_memory[flattenCoords(temp.x, temp.y)] = (current_distance << 8) | (temp_walls & 0b11111111);

			if (temp.y + 1 < MAZE_WIDTH && !checkIfGoalCoord(temp.x, temp.y + 1)) {
				push(updateStack, &(Coordinate) { temp.x, temp.y + 1 });
			}
			if (temp.y - 1 >= 0 && !checkIfGoalCoord(temp.x, temp.y - 1)) {
				push(updateStack, &(Coordinate) { temp.x, temp.y - 1 });
			}
			if (temp.x + 1 < MAZE_WIDTH && !checkIfGoalCoord(temp.x + 1, temp.y)) {
				push(updateStack, &(Coordinate) { temp.x + 1, temp.y });
			}
			if (temp.x - 1 >= 0 && !checkIfGoalCoord(temp.x - 1, temp.y)) {
				push(updateStack, &(Coordinate) { temp.x - 1, temp.y });
			}
		}
	}
}

void turnToPreviousCell(mouse_action* actions, int* actionsCounter) {
	Coordinate prev;
	int goal_direction;

	if (peek(cellStack, &prev))
		return;

	if (currentMazeLocation.x < prev.x)
		goal_direction = EAST;
	else if (currentMazeLocation.x > prev.x)
		goal_direction = WEST;
	else if (currentMazeLocation.y < prev.y)
		goal_direction = NORTH;
	else if (currentMazeLocation.y > prev.y)
		goal_direction = SOUTH;

	if (currentDirection == goal_direction)
		return;

	if ((((currentDirection << 1) | (currentDirection >> 3)) & 0b1111) == goal_direction) {
		actions[*actionsCounter] = mouse_rotate_right;
	}
	else if ((((currentDirection >> 1) | (currentDirection << 3)) & 0b1111) == goal_direction) {
		actions[*actionsCounter] = mouse_rotate_left;
	}
	else {
		actions[*actionsCounter] = mouse_rotate_right;
		++(*actionsCounter);
		actions[*actionsCounter] = mouse_rotate_right;
	}
	++(*actionsCounter);
}

void initMazeSolver() {
	currentMazeLocation = START;
	currentDirection = NORTH;
	backtrack = 0;
	cellStack = createStack(sizeof(Coordinate), MAZE_WIDTH * MAZE_WIDTH);
	updateStack = createStack(sizeof(Coordinate), MAZE_WIDTH * MAZE_WIDTH * MAZE_WIDTH);
	intersection = (Coordinate){ -1, -1 };
	
	initMaze();
}

int isExplorationDone() {
	return isEmpty(cellStack);
}

void rightHandRule(mouse_action* actions, int walls) {
	int actionsCounter = 0;

	if ((maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] & VISITED) == 0) {
		if (backtrack)
			backtrack = 0;
		maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] = maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] | VISITED;
		push(cellStack, &currentMazeLocation);
	}

	if (!backtrack) {
		updateMaze(&currentMazeLocation, (((walls << log2Function(currentDirection)) | (walls >> (4 - log2Function(currentDirection)))) & 0b1111));
		updateDistances();
	}

	if (((walls & RIGHT) == 0) && ((peakRight() & VISITED) == 0)) {
		actions[actionsCounter] = mouse_rotate_right;
		actionsCounter++;
	}
	else if (((walls & FORWARD) == 0) && ((peakForward() & VISITED) == 0)) {
		currentDirection = currentDirection;
	}
	else if (((walls & LEFT) == 0) && ((peakLeft() & VISITED) == 0)) {
		actions[actionsCounter] = mouse_rotate_left;
		actionsCounter++;
	}
	else {
		if (!backtrack) {
			actions[actionsCounter] = mouse_rotate_right;
			actionsCounter++;			
			actions[actionsCounter] = mouse_rotate_right;
			actionsCounter++;
			backtrack = 1;
			pop(cellStack, NULL);
		}
		else {
			pop(cellStack, NULL);
			turnToPreviousCell(actions, &actionsCounter);
		}
	}

	actions[actionsCounter] = mouse_move_fw;
	actionsCounter++;
}

void floodFill(mouse_action* actions, int walls) {
	int actionsCounter = 0;

	if (!FULL_EXPLORATION) {
		if ((maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] & IN_STACK) == 0) {
			maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] = maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] | IN_STACK;
			push(cellStack, &currentMazeLocation);
		}
		else {
			Coordinate prev;
			do {
				pop(cellStack, &prev);
				maze_memory[flattenCoords(prev.x, prev.y)] = maze_memory[flattenCoords(prev.x, prev.y)] & ~IN_STACK;
			} while (!(prev.x == currentMazeLocation.x && prev.y == currentMazeLocation.y));

			maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] = maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] | IN_STACK;
			push(cellStack, &currentMazeLocation);
		}
	}
	else {
		if (!backtrack || (maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] & VISITED) == 0) {
			push(cellStack, &currentMazeLocation);
		}
	}

	if ((maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] & VISITED) == 0) {
		backtrack = 0;

		maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] = maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] | VISITED;
		updateMaze(&currentMazeLocation, (((walls << log2Function(currentDirection)) | (walls >> (4 - log2Function(currentDirection)))) & 0b1111));
		updateDistances();
	}
	else {
		backtrack = 1;
		intersection = (Coordinate){ -1, -1 };
	}

	int distance = INT_MAX;
	int turn = BACK_TRACK;
	int temp_dist;

	if (backtrack) {
		if (((walls & FORWARD) == 0) && ((peakForward() & VISITED) == 0)) {
			turn = FORWARD;
			distance = peakForward() >> 8;
		}
		if (((walls & RIGHT) == 0) && ((peakRight() & VISITED) == 0)) {
			temp_dist = peakRight() >> 8;
			if (distance > temp_dist) {
				turn = RIGHT;
				distance = temp_dist;
			}
		}
		if (((walls & LEFT) == 0) && ((peakLeft() & VISITED) == 0)) {
			temp_dist = peakLeft() >> 8;
			if (distance > temp_dist) {
				turn = LEFT;
				distance = temp_dist;
			}
		}

		if (!FULL_EXPLORATION) {
			if (!((maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] >> 8) > distance)) {
				//if (!((maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] >> 8) + 1 >= distance)) {
				//if (!((maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] >> 8) >= distance)) {
				turn = BACK_TRACK;
			}
			else {
				intersection = currentMazeLocation;
			}
		}
	}

	if (!backtrack) {
		if ((walls & FORWARD) == 0) {
			turn = FORWARD;
			distance = peakForward() >> 8;
		}
		if ((walls & RIGHT) == 0) {
			temp_dist = peakRight() >> 8;
			if (distance > temp_dist) {
				turn = RIGHT;
				distance = temp_dist;
			}
		}
		if ((walls & LEFT) == 0) {
			temp_dist = peakLeft() >> 8;
			if (distance > temp_dist) {
				turn = LEFT;
				distance = temp_dist;
			}
		}

		if (!FULL_EXPLORATION) {
			if (intersection.x != -1 && intersection.y != -1) {
				//if ((maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] >> 8) > (maze_memory[flattenCoords(intersection.x, intersection.y)] >> 8)) {
				if ((maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] >> 8) >= (maze_memory[flattenCoords(intersection.x, intersection.y)] >> 8)) {
					// backtrack = 1;
					intersection = (Coordinate){ -1, -1 };
					turn = BACK_TRACK;
				}
			}
		}
	}


	switch (turn)
	{
	case FORWARD:
		currentDirection = currentDirection;
		break;
	case RIGHT:
		actions[actionsCounter] = mouse_rotate_right;
		actionsCounter++;
		break;
	case LEFT:
		actions[actionsCounter] = mouse_rotate_left;
		actionsCounter++;
		break;
	case BACK_TRACK:
		pop(cellStack, NULL);
		maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] = maze_memory[flattenCoords(currentMazeLocation.x, currentMazeLocation.y)] & ~IN_STACK;
		backtrack = 1;
		turnToPreviousCell(actions, &actionsCounter);
		break;
	default:
		break;
	}

	actions[actionsCounter] = mouse_move_fw;
	actionsCounter++;
}


