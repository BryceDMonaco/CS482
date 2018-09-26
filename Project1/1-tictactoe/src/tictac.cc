#include <stdio.h>
#include <tictac_support.h>
#include <iostream>

void PrintBoard (int board[][3]);

int main(int argc, char* argv[])
{

	int board[3][3], move[3][3];

	// load in current board state
	load_board(argv[1], board);

	// make move
	make_move(board);
	
	// save board state
	save_board(argv[2], board);

	PrintBoard(board);

	return 0;
}

void PrintBoard (int board[][3])
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			std::cout << board[i][j] << "\t";

		}

		std::cout << std::endl;

	}

	return;

}

