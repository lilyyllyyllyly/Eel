#include <stdio.h>
#include <raylib.h>

#define WIDTH  600
#define HEIGHT 600
#define FPS 30
#define PROC_TIME 0.2

#define COLS 15
#define ROWS 15

#define PLAYER_CLR BLACK
#define APPLE_CLR  ORANGE
#define BACK_CLR   DARKBLUE

#define BASE_LEN 3

#define LOST 1
#define WON  2

int cellw = WIDTH/COLS, cellh = HEIGHT/ROWS;

typedef struct {
	int col, row;
	Color color;
} Cell;

Cell snake[COLS*ROWS];
int length = BASE_LEN;
int dirX, dirY;

double lastProc;
int lastKey = KEY_RIGHT, currDir;

Cell apple;

void Initialize() {
	SetTraceLogLevel(LOG_WARNING); /* getting rid of annoying init info */
	InitWindow(WIDTH, HEIGHT, "snake :3");
	SetTargetFPS(FPS);

	for (int i = 0; i < length; ++i) {
		snake[i] = (Cell) { .col = COLS/2, .row = ROWS/2, .color = PLAYER_CLR };
	}

	apple = (Cell) { .col = GetRandomValue(0, COLS-1), .row = GetRandomValue(0, ROWS-1), .color = APPLE_CLR };
}

void HandleKeys() {
	int key;
	while ((key = GetKeyPressed())) {
		switch (key) case KEY_UP: case KEY_DOWN: case KEY_LEFT: case KEY_RIGHT: lastKey = key;
	}
}

int Process() {
	double time = GetTime();
	if (time - lastProc < PROC_TIME) return 0;

	// Storing head position before move
	Cell last = snake[0];

	// Moving head
	if (lastKey == KEY_DOWN  && currDir != KEY_UP)    currDir = KEY_DOWN;
        if (lastKey == KEY_UP    && currDir != KEY_DOWN)  currDir = KEY_UP;
        if (lastKey == KEY_RIGHT && currDir != KEY_LEFT)  currDir = KEY_RIGHT;
        if (lastKey == KEY_LEFT  && currDir != KEY_RIGHT) currDir = KEY_LEFT;
	snake[0].col += (currDir == KEY_RIGHT) - (currDir == KEY_LEFT);
	snake[0].row += (currDir == KEY_DOWN)  - (currDir == KEY_UP);

	// -- Death check -- //
	// Left Screen
	if (snake[0].col >= COLS || snake[0].col < 0 || snake[0].row >= ROWS || snake[0].row < 0) {
		return LOST;
	}
	// Self-intersection
	for (int i = 1; i < length; ++i) {
		if (snake[i].col == snake[0].col && snake[i].row == snake[0].row) {
			return LOST;
		}
	}
	// ----------------- //

	// Moving rest of the body
	Cell temp;
	for (int i = 1; i < length; ++i) {
		temp = snake[i];
		snake[i] = last;
		last = temp;
	}
	
	// Eat apple
	if (snake[0].col == apple.col && snake[0].row == apple.row) {
		// Grow
		if (length < COLS*ROWS) {
			++length;
			snake[length - 1] = snake[length - 2];
		}

		// Reposition apple until it is in a valid position
		apple.col = GetRandomValue(0, COLS-1);
		apple.row = GetRandomValue(0, ROWS-1);
		
		int valid = 0, ogCol = apple.col, ogRow = apple.row;
		while (!valid) {
			for (int i = 0; i < length; ++i) {
				// Check if apple doesnt overlap with current snake segment
				valid = (apple.col != snake[i].col || apple.row != snake[i].row);
				if (valid) continue; // If it doesn't, check the next segment

				// If it does, move it to next position (wrapping)
				int newPos = (COLS*apple.row + apple.col + 1) % (COLS*ROWS);
				apple.row = newPos/COLS;
				apple.col = newPos%COLS;

				// If it got back to the start, the snake is covering the whole screen, so the player won
				if (apple.row == ogRow && apple.col == ogCol) {
					return WON;
				}
				break;
			}
		}
	}

	lastProc = GetTime();
	return 0;
}

void Draw() {
	BeginDrawing();

	ClearBackground(BACK_CLR);

	// Apple
	DrawRectangle(apple.col * cellw, apple.row * cellh, cellw, cellh, apple.color);

	// Snake
	for (int i = 0; i < length; ++i) {
		DrawRectangle(snake[i].col * cellw, snake[i].row * cellh, cellw, cellh, snake[i].color);
	}

	EndDrawing();
}

int main() {
	Initialize();
	
	while (!WindowShouldClose()) {
		HandleKeys();
		switch (Process()) {
			case LOST:
				printf("ded\n");
				goto end;
			case WON:
				printf("won\n");
				goto end;
		}
		Draw();
	}
	end:
}
