#include <tictac_support.h>
#include <stdio.h>
#include <iostream>

using namespace std;

bool IsGameOver (int board[][3]);
bool IsGameOver (int** board); //An overload which converts the int** board to board[][3]
int GetMovesRemaining (int board[][3]);
int GetMovesRemaining (int** board); //An overload which converts the int** board to board[][3]
int FindNextBestMove (int** board, int turn, bool myTurn, int& turnCount);
int* BoardToArray (int board[][3]);
void CopyBoardPtr (int** from, int** to);

/**
	make_move: takes a board state and makes a legal 
	(hopefully optimal) move

	args:
		int [][3] board: 3x3 array of ints representing the 
		board state. The values of board are altered based
		on the move
			0: empty
			1: x
		 -1: o
		
	returns (int):
		the number of steps it took to choose the best move
		(current implementation returns 1 by default, 0 if no move made)
**/

int make_move(int board[][3])
{
	// figure out what move it is
	int state = 0;
	for( int i = 0; i < 3; i++ )
		for( int j = 0; j < 3; j++ )
			state += board[i][j];

	state = -state;
	if( state == 0 )
		state = 1;

	int stepsTaken = 0;

	if (!IsGameOver(board)) //If the game has not been won and spaces remain
	{
		//cout << "The game is not over, making a move" << endl;

		int bestMoveX = -9999;
		int bestMoveY = -9999; 
		int bestScore = -9999;

		for (int i = 0; i < 3; i++)
		{
			//cout << "MM i=" << i << endl;

			for (int j = 0; j < 3; j++)
			{
					//cout << "MM j=" << j << endl;

				if (board[i][j] == 0) //Found a space which hasn't had a move made
				{
					//cout << "Found an empty space at [" << i << "][" << j << "]" << endl;

					int** newBoard = new int*[3];
					newBoard[0] = new int[3];
					newBoard[1] = new int[3];
					newBoard[2] = new int[3];

					//Copy the saved board pointer into the new board
					for (int a = 0; a < 3; a++)
					{
						for (int b = 0; b < 3; b++)
						{
							*(*(newBoard + a) + b) = board[a][b];

						}

					}

					//Make the available move
					*(*(newBoard + i) + j) = state;

					int thisMoveScore = FindNextBestMove (newBoard, -state, false, stepsTaken);

					if (thisMoveScore > bestScore)
					{
						bestScore = thisMoveScore;
						bestMoveX = i;
						bestMoveY = j;

					}

					delete newBoard;
					newBoard = NULL;

				}
			}
		}

		//At this point all possible moves have been checked, now make the actual best move
		cout << "Making a move at [" << bestMoveX << "][" << bestMoveY << "]" << endl;		
		cout << "Took " << stepsTaken << " steps to find the best move." << endl;

		board[bestMoveX][bestMoveY] = state;

		return stepsTaken;

	} else
	{
		cout << "Game is over, no move made." << endl;
		return 0; //No move made

	}


/*
	// default behavior: find any unoccupied square and make the move
	for( int i = 0; i < 3; i++ )
		for( int j = 0; j < 3; j++ )
			// find an empty square
			if( board[i][j] == 0 )
			{
				// that's the move
				printf( "player [%d] made move: [%d,%d]\n", state, i, j );
				board[i][j] = state;
				return 1;
			}
*/	
	// no move was made (board was full)
	return 0;
}

int FindNextBestMove (int** board, int turn, bool myTurn, int& turnCount)
{
	//cout << "NB";

	//*(*(board+ 0) + 2) = 8; //An example of how to set board[0][2] to 8

	if (GetMovesRemaining(board) == 0)
	{
		//Tie game
		return 0;

	}

	if (IsGameOver(board))
	{
		/*
			If a game over is detected on my turn, that means the previous turn resulted in a loss,
			return a negative score

			If a game over is detected on my enemy's turn, that means the previous turn resulted in a win,
			return a positive score
		*/
		int signValue = myTurn ? -1 : 1;

		return signValue * 10;

	}

	if (myTurn)
	{
		int bestMoveX = -9999;
		int bestMoveY = -9999; 
		int bestScore = -9999;

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (*(*(board + i) + j) == 0) //Found a space which hasn't had a move made
				{
					int** newBoard = new int*[3];
					newBoard[0] = new int[3];
					newBoard[1] = new int[3];
					newBoard[2] = new int[3];

					//Copy the sent board pointer into the new board pointer
					CopyBoardPtr (board, newBoard);

					//Make the available move
					*(*(newBoard + i) + j) = turn;

					turnCount++;

					int thisMoveScore = FindNextBestMove (newBoard, -turn, false, turnCount);

					if (thisMoveScore > bestScore)
					{
						bestScore = thisMoveScore;
						bestMoveX = i;
						bestMoveY = j;

					}

					delete newBoard;
					newBoard = NULL;

				}
			}
		}

		return bestScore;

	} else
	{
		int bestMoveX = -9999;
		int bestMoveY = -9999; 
		int bestScore = 9999;

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (*(*(board + i) + j) == 0) //Found a space which hasn't had a move made
				{
					int** newBoard = new int*[3];
					newBoard[0] = new int[3];
					newBoard[1] = new int[3];
					newBoard[2] = new int[3];

					//Copy the sent board pointer into the new board pointer
					CopyBoardPtr (board, newBoard);

					//Make the available move
					*(*(newBoard + i) + j) = turn;

					turnCount++;

					int thisMoveScore = FindNextBestMove (newBoard, -turn, true, turnCount);

					if (thisMoveScore < bestScore)
					{
						bestScore = thisMoveScore;
						bestMoveX = i;
						bestMoveY = j;

					}

					delete newBoard;
					newBoard = NULL;

				}
			}
		}

		return bestScore;

	}

	return 2;

}

//Returns true if the game has been won or if no spaces remain
//This overload converts a dynamic array pointer to an array
bool IsGameOver (int** sentBoard)
{
	//cout << "GOp";

	bool spacesRemaining = false;
	bool gameWon = false;

	int board[3][3];

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			board[i][j] = *(*(sentBoard + i) + j);

		}

	}

	return IsGameOver(board);

} 

//Returns true if the game has been won or if no spaces remain
bool IsGameOver (int board[][3])
{
	//cout << "GO";

	bool spacesRemaining = false;
	bool gameWon = false;

	spacesRemaining = GetMovesRemaining(board) > 0;

	//cout << "GO2";


	//While there are spaces remaining and the game hasn't been won yet, check for wins
	for (int i = 0; i < 3 && spacesRemaining && !gameWon; i++)
	{
		//Check if there are any horizontal wins for 1
		if (board[i][0] == 1 && board[i][1] == 1 && board[i][2] == 1)
		{
			gameWon = true;

		} else if (board[i][0] == -1 && board[i][1] == -1 && board[i][2] == -1) //Check for -1
		{
			gameWon = true;

		}

		//Check if there are any vertical wins for 1
		if (board[0][i] == 1 && board[1][i] == 1 && board[2][i] == 1)
		{
			gameWon = true;

		} else if (board[0][i] == -1 && board[1][i] == -1 && board[2][i] == -1) //Check for -1
		{
			gameWon = true;

		}
	}

	//Check for diagonal wins if we haven't already found a win
	if (!gameWon && board[0][0] == 1 && board[1][1] == 1 && board[2][2] == 1) //A '\' win for 1
	{
		gameWon = true;

	} else if (!gameWon && board[0][2] == 1 && board[1][1] == 1 && board[2][0] == 1) //A '/' win for 1
	{
		gameWon = true;

	} else if (!gameWon && board[0][0] == -1 && board[1][1] == -1 && board[2][2] == -1) //A '\' win for -1
	{
		gameWon = true;

	} else if (!gameWon && board[0][2] == -1 && board[1][1] == -1 && board[2][0] == -1) //A '/' win for -1
	{
		gameWon = true;

	}

	//cout << "GO Check finished" << endl;

	/*
	g | s | =
	F   F   T 	//The game has not been won but no spaces remain, game over
	F   T   F 	//The game has not been won and spaces remain
	T   F   T 	//The game has been won, game over
	T   T   T 	//The game has been won, game over
	*/
	return gameWon || !spacesRemaining;

} 

//This overload converts a dynamic array pointer to an array
int GetMovesRemaining (int** sentBoard)
{
	//cout << "MRp";

	int board[3][3];

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			board[i][j] = *(*(sentBoard + i) + j);

		}

	}

	return GetMovesRemaining(board);

} 

int GetMovesRemaining (int board[][3])
{
	//cout << "MR";

	int openSpaces = 0;

	//Check if the board is full or not
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			//cout << board[i][j] << " ";

			if (board[i][j] == 0)
			{
				openSpaces++;

			}
		}

		//cout << endl;
	}

	//cout << "There are " << openSpaces << " spaces remaining" << endl << endl;

	return openSpaces;

}

void CopyBoardPtr (int** from, int** to)
{
	//cout << "CP";

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			*(*(to + i) + j) = *(*(from + i) + j);

		}

	}

	return;

}
