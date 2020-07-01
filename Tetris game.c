#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <Windows.h>

//

#define TRUE 1

#define FALSE 0

//

#define COLOR_CODE "\x1b["

#define RED 31

#define GREEN 32

#define YELLOW 33

#define BLUE 34

#define MAGENTA 35

#define CYAN 36

#define NO_COLOR 0

#define ANSI_COLOR_RESET "\x1b[0m"

#define EMPTY 0

#define FULL 219

//

#define ROW 25        

#define COL 12

#define ROW_2 9

#define COL_2 9

int row;

int col;

typedef struct
{
	unsigned char color;
	unsigned char state;
} field;

field playField[ROW][COL];

field previewField[ROW_2][COL_2];


//

#define ROT_0 0

#define ROT_90 1

#define ROT_180 2

#define ROT_270 3

typedef struct
{
	int el1;
	int el2;
	int el3;
	int el4;
} rotationMatrix;

rotationMatrix rot[4] = { { 1, 0, 0, 1 }, { 0, -1, 1, 0 }, {-1, 0, 0, -1}, { 0, 1, -1, 0} };

//

#define S_BLOCK 0

#define Z_BLOCK 1

#define L_BLOCK 2

#define J_BLOCK 3

#define T_BLOCK 4

#define O_BLOCK 5

#define I_BLOCK 6


#define S_COLOR GREEN

#define Z_COLOR RED

#define L_COLOR BLUE

#define J_COLOR RED

#define T_COLOR MAGENTA

#define O_COLOR YELLOW

#define I_COLOR CYAN


typedef struct
{
	int el1;
	int el2;
	int el3;
} blockElY;

typedef struct
{
	int el1;
	int el2;
	int el3;
} blockElX;

blockElY ElY[7] = { {-1, -1, 0}, {-1, -1, 0}, {0, 0, -1}, {0, 0, -1 }, {-1, 0, 0}, {-1, -1, 0}, {0, 0, 0 } };
blockElX ElX[7] = { {1, 0, -1 }, {-1, 0, 1 }, {-1, 1, 1}, {1, -1, -1}, {0, -1, 1}, {0, 1, 1  }, {-1, 1, 2} };

int offsetY[7][4] = { {0}, {0}, {0}, {0}, {0}, {0, 0, -1, -1}, {-1, 0, 0, -1} };
int offsetX[7][4] = { {0}, {0}, {0}, {0}, {0}, {0, 1, 1, 0  }, {0, 0, 1, 1  } };

int y;
int x;
int degrees;

int currentBlock;

int blockColor;

int yPrev = 4;
int xPrev = 4;
int degreesPrev = ROT_90;

int previewBlock;

int blockColorPrev;

//

#define ROT_CLOCKWISE 1

#define ROT_COUNTER_CLOCKWISE 2

#define LEFT 3

#define RIGHT 4

#define DOWN 5

#define NONE -1

//

int game = TRUE;

int fieldChange = FALSE;

int blockActive = FALSE;

int bagElement = 0;

int blockPermutations[7] = { 0 };

unsigned int score = 0;

unsigned int level = 1;

unsigned int totalClearedRows = 0;

//

void genField();

void printField();

void blockSpawn();

void blockClear();

int blockCollide(int yNew, int xNew);

void rotateBlockClockwise();

void rotateBlockCounterClockwise();

void blockFall();

void moveBlockLeft();

void moveBlockRight();

void queBlock();

void activateBlock();

void previewBlockSpawn();

void previewBlockClear();

int rowCheck();

void blockGravity(int firstRow, int lastRow);

void updateScore(int numClearedRows);

int getUserInput();

//

int main()
{
	genField();

	queBlock();
	activateBlock();
	queBlock();
	blockActive = TRUE;

	printField();

	int numClearedRows;
	int input;
	int timer = 0;
	while (game == TRUE)
	{
		input = getUserInput();

		switch (input)
		{
		case ROT_CLOCKWISE:
			rotateBlockClockwise();
			break;

		case ROT_COUNTER_CLOCKWISE:
			rotateBlockCounterClockwise();
			break;

		case LEFT:
			moveBlockLeft();
			break;

		case RIGHT:
			moveBlockRight();
			break;

		case DOWN:
			blockFall();
			break;

		case NONE:
			break;

		default:
			break;

		}

		int reduction;
		if (level <= 20)
		{
			reduction = 1000 * level;
		}
		else
		{
			reduction = 20000 + (250 * (level - 20));
		}

		if (timer >= 21000 - reduction)
		{
			blockFall();
			timer = 0;
		}
		timer++;

		if (blockActive == FALSE)
		{
			numClearedRows = rowCheck();
			activateBlock();
			queBlock();

			updateScore(numClearedRows);
		}

		if (fieldChange == TRUE)
		{
			printField();
			fieldChange = FALSE;
		}
	}
	return 0;
}

void genField()
{

	for (col = 0; col < COL; col++)
	{
		playField[0][col].color = NO_COLOR;
		playField[0][col].state = FULL;
	}

	for (row = 0; row < ROW - 1; row++) // Play field gen
	{
		playField[row][0].color = NO_COLOR;
		playField[row][0].state = FULL;

		playField[row][COL - 1].color = NO_COLOR;
		playField[row][COL - 1].state = FULL;
		for (col = 1; col < COL - 1; col++)
		{
			playField[row][col].color = NO_COLOR;
			playField[row][col].state = EMPTY;
		}
	}

	for (col = 0; col < COL; col++)
	{
		playField[ROW - 1][col].color = NO_COLOR;
		playField[ROW - 1][col].state = FULL;
	}

	//

	for (col = 0; col < COL_2; col++) // Preview field gen
	{
		previewField[0][col].color = NO_COLOR;
		previewField[0][col].state = FULL;
	}

	for (row = 1; row < ROW_2 - 1; row++)
	{
		previewField[row][0].color = NO_COLOR;
		previewField[row][0].state = FULL;

		previewField[row][COL_2 - 1].color = NO_COLOR;
		previewField[row][COL_2 - 1].state = FULL;
		for (col = 1; col < COL_2 - 1; col++)
		{
			previewField[row][col].color = NO_COLOR;
			previewField[row][col].state = EMPTY;
		}
	}

	for (col = 0; col < COL_2; col++)
	{
		previewField[ROW_2 - 1][col].color = NO_COLOR;
		previewField[ROW_2 - 1][col].state = FULL;
	}
}

void printField()
{
	printf("\x1b[?25l");

	HANDLE hOut;
	COORD position;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	position.X = 0;
	position.Y = 0;
	SetConsoleCursorPosition(hOut, position);

	printf("\n\n\n\n\n\n");

	//
	
	printf("\tSCORE: %u\tLEVEL %i", score, level);
	printf("\n\n");

	//

	printf("\t");
	for (col = 0; col < COL; col++)
	{
		printf("%c", FULL);
		printf("%c", FULL);
	}
	printf("\n");

	for (row = 4; row < ROW; row++)
	{
		printf("\t");
		for (col = 0; col < COL; col++)
		{
			printf(""COLOR_CODE"%im", playField[row][col].color);
			printf("%c", playField[row][col].state);
			printf("%c", playField[row][col].state);
		}
		printf("\n");
	}

	//

	printf("\n");
	for (row = 0; row < ROW_2; row++)
	{
		printf("\t");
		for (col = 0; col < COL_2; col++)
		{
			printf(""COLOR_CODE"%im", previewField[row][col].color);
			printf("%c", previewField[row][col].state);
			printf("%c", previewField[row][col].state);
		}
		printf("\n");
	}

	printf(""ANSI_COLOR_RESET"");
}

void blockSpawn()
{
	playField[y + offsetY[currentBlock][degrees]][x + offsetX[currentBlock][degrees]].state = FULL;

	playField[(rot[degrees].el1 * (ElY[currentBlock].el1)) + (rot[degrees].el2 * (ElX[currentBlock].el1)) + y + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el1)) + (rot[degrees].el4 * (ElX[currentBlock].el1)) + x + offsetX[currentBlock][degrees]].state = FULL;

	playField[(rot[degrees].el1 * (ElY[currentBlock].el2)) + (rot[degrees].el2 * (ElX[currentBlock].el2)) + y + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el2)) + (rot[degrees].el4 * (ElX[currentBlock].el2)) + x + offsetX[currentBlock][degrees]].state = FULL;

	playField[(rot[degrees].el1 * (ElY[currentBlock].el3)) + (rot[degrees].el2 * (ElX[currentBlock].el3)) + y + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el3)) + (rot[degrees].el4 * (ElX[currentBlock].el3)) + x + offsetX[currentBlock][degrees]].state = FULL;

	//

	playField[y + offsetY[currentBlock][degrees]][x + offsetX[currentBlock][degrees]].color = blockColor;

	playField[(rot[degrees].el1 * (ElY[currentBlock].el1)) + (rot[degrees].el2 * (ElX[currentBlock].el1)) + y + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el1)) + (rot[degrees].el4 * (ElX[currentBlock].el1)) + x + offsetX[currentBlock][degrees]].color = blockColor;

	playField[(rot[degrees].el1 * (ElY[currentBlock].el2)) + (rot[degrees].el2 * (ElX[currentBlock].el2)) + y + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el2)) + (rot[degrees].el4 * (ElX[currentBlock].el2)) + x + offsetX[currentBlock][degrees]].color = blockColor;

	playField[(rot[degrees].el1 * (ElY[currentBlock].el3)) + (rot[degrees].el2 * (ElX[currentBlock].el3)) + y + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el3)) + (rot[degrees].el4 * (ElX[currentBlock].el3)) + x + offsetX[currentBlock][degrees]].color = blockColor;

	fieldChange = TRUE;
}

void blockClear()
{
	playField[y + offsetY[currentBlock][degrees]][x + offsetX[currentBlock][degrees]].state = EMPTY;

	playField[(rot[degrees].el1 * (ElY[currentBlock].el1)) + (rot[degrees].el2 * (ElX[currentBlock].el1)) + y + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el1)) + (rot[degrees].el4 * (ElX[currentBlock].el1)) + x + offsetX[currentBlock][degrees]].state = EMPTY;

	playField[(rot[degrees].el1 * (ElY[currentBlock].el2)) + (rot[degrees].el2 * (ElX[currentBlock].el2)) + y + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el2)) + (rot[degrees].el4 * (ElX[currentBlock].el2)) + x + offsetX[currentBlock][degrees]].state = EMPTY;

	playField[(rot[degrees].el1 * (ElY[currentBlock].el3)) + (rot[degrees].el2 * (ElX[currentBlock].el3)) + y + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el3)) + (rot[degrees].el4 * (ElX[currentBlock].el3)) + x + offsetX[currentBlock][degrees]].state = EMPTY;

	//

	playField[y + offsetY[currentBlock][degrees]][x + offsetX[currentBlock][degrees]].color = NO_COLOR;

	playField[(rot[degrees].el1 * (ElY[currentBlock].el1)) + (rot[degrees].el2 * (ElX[currentBlock].el1)) + y + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el1)) + (rot[degrees].el4 * (ElX[currentBlock].el1)) + x + offsetX[currentBlock][degrees]].color = NO_COLOR;

	playField[(rot[degrees].el1 * (ElY[currentBlock].el2)) + (rot[degrees].el2 * (ElX[currentBlock].el2)) + y + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el2)) + (rot[degrees].el4 * (ElX[currentBlock].el2)) + x + offsetX[currentBlock][degrees]].color = NO_COLOR;

	playField[(rot[degrees].el1 * (ElY[currentBlock].el3)) + (rot[degrees].el2 * (ElX[currentBlock].el3)) + y + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el3)) + (rot[degrees].el4 * (ElX[currentBlock].el3)) + x + offsetX[currentBlock][degrees]].color = NO_COLOR;

	fieldChange = TRUE;
}

int blockCollide(int yNew, int xNew)
{
	if (
		playField[yNew + offsetY[currentBlock][degrees]][xNew + offsetX[currentBlock][degrees]].state == EMPTY

		&& playField[(rot[degrees].el1 * (ElY[currentBlock].el1)) + (rot[degrees].el2 * (ElX[currentBlock].el1)) + yNew + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el1)) + (rot[degrees].el4 * (ElX[currentBlock].el1)) + xNew + offsetX[currentBlock][degrees]].state == EMPTY

		&& playField[(rot[degrees].el1 * (ElY[currentBlock].el2)) + (rot[degrees].el2 * (ElX[currentBlock].el2)) + yNew + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el2)) + (rot[degrees].el4 * (ElX[currentBlock].el2)) + xNew + offsetX[currentBlock][degrees]].state == EMPTY

		&& playField[(rot[degrees].el1 * (ElY[currentBlock].el3)) + (rot[degrees].el2 * (ElX[currentBlock].el3)) + yNew + offsetY[currentBlock][degrees]]
		[(rot[degrees].el3 * (ElY[currentBlock].el3)) + (rot[degrees].el4 * (ElX[currentBlock].el3)) + xNew + offsetX[currentBlock][degrees]].state == EMPTY
		)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void rotateBlockClockwise()
{
	int clear;
	blockClear();

	degrees--;
	if (degrees < ROT_0)
	{
		degrees = ROT_270;
	}

	if ((clear = blockCollide(y, x)) == FALSE)
	{
		blockSpawn();
	}
	else
	{
		degrees++;
		if (degrees > ROT_270)
		{
			degrees = ROT_0;
		}
		blockSpawn();
	}
}

void rotateBlockCounterClockwise()
{
	int clear;
	blockClear();

	degrees++;
	if (degrees > ROT_270)
	{
		degrees = ROT_0;
	}

	if ((clear = blockCollide(y, x)) == FALSE)
	{
		blockSpawn();
	}
	else
	{
		degrees--;
		if (degrees < ROT_0)
		{
			degrees = ROT_270;
		}
		blockSpawn();
	}
}

void blockFall()
{
	int clear;
	int yNew = y + 1;

	blockClear();
	if ((clear = blockCollide(yNew, x)) == FALSE)
	{
		y = yNew;
		blockSpawn();
	}
	else
	{
		blockSpawn();
		blockActive = FALSE;
	}
}

void moveBlockLeft()
{
	int clear;
	int xNew = x - 1;

	blockClear();
	if ((clear = blockCollide(y, xNew)) == FALSE)
	{
		x = xNew;
		blockSpawn();
	}
	else
	{
		blockSpawn();
	}
}

void moveBlockRight()
{
	int clear;
	int xNew = x + 1;

	blockClear();
	if ((clear = blockCollide(y, xNew)) == FALSE)
	{
		x = xNew;
		blockSpawn();
	}
	else
	{
		blockSpawn();
	}
}

void queBlock()
{
	srand((int)time(NULL));

	int bagCreate = 0;
	int currentElement = 0;
	int existingElements = 0;

	if (bagElement == 7 || bagElement == 0)
	{
		bagCreate = 1;
		currentElement = 0;
		existingElements = 0;

		bagElement = 0;
	}

	while (bagCreate == 1)
	{
		int check = 0;

		blockPermutations[currentElement] = (rand() % 7);

		int i;
		for (i = 0; i < existingElements; i++)
		{
			if (blockPermutations[currentElement] == blockPermutations[i])
			{
				check = 1;
			}
		}

		if (check != 1)
		{
			currentElement++;
			existingElements++;
		}

		if (existingElements == 7)
		{
			bagCreate = 0;
		}
	}

	previewBlock = blockPermutations[bagElement];
	bagElement++;

	switch (previewBlock)
	{
	case S_BLOCK:
		blockColorPrev = S_COLOR;
		break;

	case Z_BLOCK:
		blockColorPrev = Z_COLOR;
		break;

	case L_BLOCK:
		blockColorPrev = L_COLOR;
		break;

	case J_BLOCK:
		blockColorPrev = J_COLOR;
		break;

	case T_BLOCK:
		blockColorPrev = T_COLOR;
		break;

	case O_BLOCK:
		blockColorPrev = O_COLOR;
		break;

	case I_BLOCK:
		blockColorPrev = I_COLOR;
		break;

	default:
		break;
	}

	previewBlockSpawn();
}

void activateBlock()
{
	int clear;
	previewBlockClear();

	y = 5;
	x = 5;
	degrees = ROT_0;
	currentBlock = previewBlock;
	blockColor = blockColorPrev;

	if ((clear = blockCollide(y, x)) == FALSE)
	{
		blockActive = TRUE;
		blockSpawn();
	}
	else if ((clear = blockCollide(y - 1, x)) == FALSE)
	{
		y--;
		blockActive = TRUE;
		blockSpawn();
	}
	else if ((clear = blockCollide(y - 2, x)) == FALSE)
	{
		
		y -= 2;
		blockActive = TRUE;
		blockSpawn();
	}
	else
	{
		game = 0;
	}
}

void previewBlockSpawn()
{
	previewField[yPrev + offsetY[previewBlock][degreesPrev]][xPrev + offsetX[previewBlock][degreesPrev]].state = FULL;

	previewField[(rot[degreesPrev].el1 * (ElY[previewBlock].el1)) + (rot[degreesPrev].el2 * (ElX[previewBlock].el1)) + yPrev + offsetY[previewBlock][degreesPrev]]
		[(rot[degreesPrev].el3 * (ElY[previewBlock].el1)) + (rot[degreesPrev].el4 * (ElX[previewBlock].el1)) + xPrev + offsetX[previewBlock][degreesPrev]].state = FULL;

	previewField[(rot[degreesPrev].el1 * (ElY[previewBlock].el2)) + (rot[degreesPrev].el2 * (ElX[previewBlock].el2)) + yPrev + offsetY[previewBlock][degreesPrev]]
		[(rot[degreesPrev].el3 * (ElY[previewBlock].el2)) + (rot[degreesPrev].el4 * (ElX[previewBlock].el2)) + xPrev + offsetX[previewBlock][degreesPrev]].state = FULL;

	previewField[(rot[degreesPrev].el1 * (ElY[previewBlock].el3)) + (rot[degreesPrev].el2 * (ElX[previewBlock].el3)) + yPrev + offsetY[previewBlock][degreesPrev]]
		[(rot[degreesPrev].el3 * (ElY[previewBlock].el3)) + (rot[degreesPrev].el4 * (ElX[previewBlock].el3)) + xPrev + offsetX[previewBlock][degreesPrev]].state = FULL;

	//

	previewField[yPrev + offsetY[previewBlock][degreesPrev]][xPrev + offsetX[previewBlock][degreesPrev]].color = blockColorPrev;

	previewField[(rot[degreesPrev].el1 * (ElY[previewBlock].el1)) + (rot[degreesPrev].el2 * (ElX[previewBlock].el1)) + yPrev + offsetY[previewBlock][degreesPrev]]
		[(rot[degreesPrev].el3 * (ElY[previewBlock].el1)) + (rot[degreesPrev].el4 * (ElX[previewBlock].el1)) + xPrev + offsetX[previewBlock][degreesPrev]].color = blockColorPrev;

	previewField[(rot[degreesPrev].el1 * (ElY[previewBlock].el2)) + (rot[degreesPrev].el2 * (ElX[previewBlock].el2)) + yPrev + offsetY[previewBlock][degreesPrev]]
		[(rot[degreesPrev].el3 * (ElY[previewBlock].el2)) + (rot[degreesPrev].el4 * (ElX[previewBlock].el2)) + xPrev + offsetX[previewBlock][degreesPrev]].color = blockColorPrev;

	previewField[(rot[degreesPrev].el1 * (ElY[previewBlock].el3)) + (rot[degreesPrev].el2 * (ElX[previewBlock].el3)) + yPrev + offsetY[previewBlock][degreesPrev]]
		[(rot[degreesPrev].el3 * (ElY[previewBlock].el3)) + (rot[degreesPrev].el4 * (ElX[previewBlock].el3)) + xPrev + offsetX[previewBlock][degreesPrev]].color = blockColorPrev;

	fieldChange = TRUE;
}

void previewBlockClear()
{
	previewField[yPrev + offsetY[previewBlock][degreesPrev]][xPrev + offsetX[previewBlock][degreesPrev]].state = EMPTY;

	previewField[(rot[degreesPrev].el1 * (ElY[previewBlock].el1)) + (rot[degreesPrev].el2 * (ElX[previewBlock].el1)) + yPrev + offsetY[previewBlock][degreesPrev]]
		[(rot[degreesPrev].el3 * (ElY[previewBlock].el1)) + (rot[degreesPrev].el4 * (ElX[previewBlock].el1)) + xPrev + offsetX[previewBlock][degreesPrev]].state = EMPTY;

	previewField[(rot[degreesPrev].el1 * (ElY[previewBlock].el2)) + (rot[degreesPrev].el2 * (ElX[previewBlock].el2)) + yPrev + offsetY[previewBlock][degreesPrev]]
		[(rot[degreesPrev].el3 * (ElY[previewBlock].el2)) + (rot[degreesPrev].el4 * (ElX[previewBlock].el2)) + xPrev + offsetX[previewBlock][degreesPrev]].state = EMPTY;

	previewField[(rot[degreesPrev].el1 * (ElY[previewBlock].el3)) + (rot[degreesPrev].el2 * (ElX[previewBlock].el3)) + yPrev + offsetY[previewBlock][degreesPrev]]
		[(rot[degreesPrev].el3 * (ElY[previewBlock].el3)) + (rot[degreesPrev].el4 * (ElX[previewBlock].el3)) + xPrev + offsetX[previewBlock][degreesPrev]].state = EMPTY;

	//

	previewField[yPrev + offsetY[previewBlock][degreesPrev]][xPrev + offsetX[previewBlock][degreesPrev]].color = NO_COLOR;

	previewField[(rot[degreesPrev].el1 * (ElY[previewBlock].el1)) + (rot[degreesPrev].el2 * (ElX[previewBlock].el1)) + yPrev + offsetY[previewBlock][degreesPrev]]
		[(rot[degreesPrev].el3 * (ElY[previewBlock].el1)) + (rot[degreesPrev].el4 * (ElX[previewBlock].el1)) + xPrev + offsetX[previewBlock][degreesPrev]].color = NO_COLOR;

	previewField[(rot[degreesPrev].el1 * (ElY[previewBlock].el2)) + (rot[degreesPrev].el2 * (ElX[previewBlock].el2)) + yPrev + offsetY[previewBlock][degreesPrev]]
		[(rot[degreesPrev].el3 * (ElY[previewBlock].el2)) + (rot[degreesPrev].el4 * (ElX[previewBlock].el2)) + xPrev + offsetX[previewBlock][degreesPrev]].color = NO_COLOR;

	previewField[(rot[degreesPrev].el1 * (ElY[previewBlock].el3)) + (rot[degreesPrev].el2 * (ElX[previewBlock].el3)) + yPrev + offsetY[previewBlock][degreesPrev]]
		[(rot[degreesPrev].el3 * (ElY[previewBlock].el3)) + (rot[degreesPrev].el4 * (ElX[previewBlock].el3)) + xPrev + offsetX[previewBlock][degreesPrev]].color = NO_COLOR;

	fieldChange = TRUE;
}

int rowCheck()
{
	int cleared = 0;
	int firstClearedRow = 0;
	int lastClearedRow;
	int numClearedRows = 0;
	int row = y + 2;
	if (row >= ROW - 1)
	{
		row = ROW - 2;
	}
	int rowCheck = 0;

	while (row > (y + 1) - 4)
	{
		for (col = 1; col < COL - 1; col++)
		{
			if (
				playField[row][col].state == FULL
				)
			{
				rowCheck++;
			}
		}

		if (rowCheck == 10)
		{
			for (col = 1; col < COL - 1; col++)
			{
				playField[row][col].state = EMPTY;
				playField[row][col].color = NO_COLOR;
			}
			numClearedRows++;

			if (firstClearedRow == 0)
			{
				firstClearedRow = row;
			}
			lastClearedRow = row;
			cleared = 1;

		}
		rowCheck = 0;
		row--;
	}

	if (cleared == 1)
	{
		blockGravity(firstClearedRow, lastClearedRow);
		return numClearedRows;
	}

	return 0;
}

void blockGravity(int firstRow, int lastRow)
{
	int rowCheck = 0;

	while (rowCheck != 10)
	{
		rowCheck = 0;
		lastRow--;

		for (col = 1; col < COL - 1; col++)
		{
			if (playField[lastRow][col].state == EMPTY)
			{
				rowCheck++;
			}
		}

		if (rowCheck != 10)
		{
			for (col = 1; col < COL - 1; col++)
			{
				playField[firstRow][col].state = playField[lastRow][col].state;
				playField[firstRow][col].color = playField[lastRow][col].color;

				playField[lastRow][col].state = EMPTY;
				playField[lastRow][col].color = NO_COLOR;
			}
			firstRow--;
		}
	}
}

void updateScore(int numClearedRows)
{
	totalClearedRows += numClearedRows;

	switch (numClearedRows)
	{
	case 1:
		score += 40 * (level);
		break;

	case 2:
		score += 100 * (level);
		break;

	case 3:
		score += 300 * (level);
		break;

	case 4:
		score += 1200 * (level);
		break;
		
	default:
		break;
	}

	if (totalClearedRows > (4 * level) && level < 23)
	{
		level++;
	}
}

int getUserInput()
{
	if (_kbhit())
	{
		char input = _getch();
		switch (input)
		{
		case 'w':
		case 'W':
			return ROT_CLOCKWISE;

		case 's':
		case 'S':
			return ROT_COUNTER_CLOCKWISE;

		case 'a':
		case 'A':
			return LEFT;

		case 'd':
		case 'D':
			return RIGHT;

		case ' ':
			return DOWN;

		default:
			return NONE;

		}
	}
	else
	{
		return NONE;
	}
}