// Battleship.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "Utils.h"

using namespace std;

const char* INPUT_ERROR_STRING = "Input Error! Please try again. ";

/* Enums */

enum                                                                    // Anonymous enum to define constant values 
{
    AIRCRAFT_CARRIER_SIZE = 5,
    BATTLESHIP_SIZE = 4,
    CRUISER_SIZE = 3,
    DESTROYER_SIZE = 3,
    SUBMARINE_SIZE = 2,

    BOARD_SIZE = 10,
    NUM_SHIPS = 5,
    PLAYER_NAME_SIZE = 8,                                               // Player1, Player2
    MAX_SHIP_SIZE = AIRCRAFT_CARRIER_SIZE
};
 
enum ShipType                                                           // Type of ship enum to simplify shiptypes to a number
{
    ST_NONE = 0,
    ST_AIRCRAFT_CARRIER,
    ST_BATTLESHIP,
    ST_CRUISER,
    ST_DESTROYER,
    ST_SUBMARINE
};
            
enum ShipOrientationType                                                 // Orientation enum to define horizontal and vertical direction of ships 
{
    SO_HORIZONTAL = 0,
    SO_VERTICAL
};

enum GuessType                                                           // Type of guess the player makes, only three types
{
    GT_NONE = 0,
    GT_MISSED,
    GT_HIT
};

enum PlayerType
{
    PT_HUMAN = 0,
    PT_AI
};

/* Structs */

struct ShipPositionType                                                 // The position coordinates of the ship on the board
{
    int row;
    int col;
};

struct ShipPartType                                                     // The board spot state
{
    ShipType shipType;
    bool isHit;
};

struct Ship                                                             // Ship struct defining ship data 
{
    ShipType shipType;
    int shipSize;
    ShipOrientationType shipOrientation;
    ShipPositionType shipPosition;
};

struct Player                                                           // Player struct defining player data
{
    PlayerType playerType;
    char playerName[PLAYER_NAME_SIZE];
    Ship ships[NUM_SHIPS];
    GuessType guessBoard[BOARD_SIZE][BOARD_SIZE];
    ShipPartType shipBoard[BOARD_SIZE][BOARD_SIZE];
};

/* Initializations for player and ships */

void InitializePlayer(Player& player, const char* playerName);          // Initialize the player function
void InitializeShip(Ship& ship, int shipSize, ShipType shipType);       // Initialize the ship function

/* Game functions */

void PlayGame(Player& player1, Player& player2);                        // Play game function
bool WantToPlayAgain();                                                 // Play again function
ShipType UpdateBoards(ShipPositionType guess, Player& currentPlayer, Player& otherPlayer);
bool IsGameOver(const Player& player1, const Player& player2);
bool AreAllShipsSunk(const Player& player);
bool IsSunk(const Player& player, const Ship& ship);
void SwitchPlayers(Player** currentPlayer, Player** otherPlayer);
void DisplayWinner(const Player& player1, const Player& player2);
PlayerType GetPlayer2Type();
ShipPositionType GetAIGuess(const Player& aiPlayer);
ShipPositionType GetRandomPosition();

/* Board functions */

void SetupBoards(Player& player);                                       // Seting up the game boards function (for ship and guess boards)
void ClearBoards(Player& player);                                       // Clear boards for starting new games
void DrawBoards(const Player& player);                                  // Draw the game board in the terminal
void SetupAIBoards(Player& player);

/* Drawing of the board functions */

void DrawSeparatorLine();                                               // Creates separation lines for the game board
void DrawColumnsRow();                                                  // Creates columns for the game board, including the number of column
void DrawShipBoardRow(const Player& player, int row);                   // Creates the row for the ship board, starting with 'A'
void DrawGuessBoardRow(const Player& player, int row);                  // Creates the row for the guessing board, starting with 'A'

/* Drawing of square functions for the boards */

char GetShipRepresentationAt(const Player& player, int row, int col);   // Creates the ship representation tag for ship board
char GetGuessRepresentationAt(const Player& player, int row, int col);  // Creates the ship representation tag for the guess board 

/* Placement of ship functions */

const char* GetShipNameForShipType(ShipType shipType);
ShipPositionType GetBoardPosition();
ShipPositionType MapBoardPosition(char rowInput, int colInput);
ShipOrientationType GetShipOrientation();
bool IsValidPlacement(const Player& player, const Ship& currentShip, const ShipPositionType shipPosition, ShipOrientationType orientation);
void PlaceShipOnBoard(Player& player, Ship& currentShip, const ShipPositionType shipPosition, const ShipOrientationType orientation);


int main()
{
    srand(time(NULL));
    
    Player player1;
    Player player2;

    InitializePlayer(player1, "Player1");
    InitializePlayer(player2, "Player2");

    do
    {
        PlayGame(player1, player2);
    } while (WantToPlayAgain());

    return 0;
}

/* Game Functions */

void PlayGame(Player& player1, Player& player2)
{
    ClearScreen();

    player1.playerType = PT_HUMAN;
    player2.playerType = GetPlayer2Type();

    SetupBoards(player1);
    SetupBoards(player2);

    Player* currentPlayer = &player1;
    Player* otherPlayer = &player2;

    ShipPositionType guess;

    do
    {
        if (currentPlayer->playerType == PT_HUMAN)
        {
            DrawBoards(*currentPlayer);
        }

        bool isValidGuess;

        do
        {
            if (currentPlayer->playerType == PT_HUMAN)
            {
                cout << currentPlayer->playerName << " what is your guess? " << endl;

                guess = GetBoardPosition();
            }
            else
            {
                guess = GetAIGuess(*currentPlayer);
            }
            isValidGuess = currentPlayer->guessBoard[guess.row][guess.col] == GT_NONE;

            if (!isValidGuess && currentPlayer->playerType == PT_HUMAN)
            {
                cout << "That was not a valid guess! Please try again." << endl;
            }

        } while (!isValidGuess);

        ShipType type = UpdateBoards(guess, *currentPlayer, *otherPlayer);

        if (currentPlayer->playerType == PT_AI)
        {
            DrawBoards(*otherPlayer);
            cout << currentPlayer->playerName << " chose row " << char(guess.row + 'A') << " and column " << guess.col + 1 << endl;
        }
        else
        {
            DrawBoards(*currentPlayer);
        }
        if (type != ST_NONE && IsSunk(*otherPlayer, otherPlayer->ships[type - 1]))
        {
            if (currentPlayer->playerType == PT_AI)
            {
                cout << currentPlayer->playerName << " sunk your " << GetShipNameForShipType(type) << "!" << endl;
            }
            else
            {
                cout << "You sunk " << otherPlayer->playerName << "'s " << GetShipNameForShipType(type) << "!" << endl;
            }
        }

        WaitForKeyPress();
        SwitchPlayers(&currentPlayer, &otherPlayer);

    } while (!IsGameOver(player1, player2));

    DisplayWinner(player1, player2);
}

bool WantToPlayAgain()
{
    char input;

    const char validInput[2] = { 'y', 'n' };

    input = GetCharacter("Would you like to play again? (y/n): ", INPUT_ERROR_STRING, validInput, 2, CC_LOWER_CASE);

    return input == 'y';
}

bool IsGameOver(const Player& player1, const Player& player2)
{
    return AreAllShipsSunk(player1) || AreAllShipsSunk(player2);
}

bool AreAllShipsSunk(const Player& player)
{
    for (int i = 0; i < NUM_SHIPS; i++)
    {
        if (!IsSunk(player, player.ships[i]))
        {
            return false;
        }
    }
    return true;
}

bool IsSunk(const Player& player, const Ship& ship)
{
    if (ship.shipOrientation == SO_HORIZONTAL)
    {
        for (int col = ship.shipPosition.col; col < (ship.shipPosition.col + ship.shipSize); col++)
        {
            if (!player.shipBoard[ship.shipPosition.row][col].isHit)
            {
                return false;
            }
        }
    }
    else
    {
        for (int row = ship.shipPosition.row; row < (ship.shipPosition.row + ship.shipSize); row++)
        {
            if (!player.shipBoard[row][ship.shipPosition.col].isHit)
            {
                return false;
            }
        }
    }
    return true;
}

void SwitchPlayers(Player** currentPlayer, Player** otherPlayer)
{
    Player* temp = *currentPlayer;
    *currentPlayer = *otherPlayer;
    *otherPlayer = temp;
}

PlayerType GetPlayer2Type()
{
    const int validInputs[2] = { 1, 2 };

    int input = GetInteger("Who would you like to play against? \n1. Human \n2 AI\n\n What is your choice? ", INPUT_ERROR_STRING, validInputs, 2);

    if (input == 1)
    {
        return PT_HUMAN;
    }
    else
    {
        return PT_AI;
    }
}

void DisplayWinner(const Player& player1, const Player& player2)
{
    if (AreAllShipsSunk(player1))
    {
        cout << "Congrats " << player2.playerName << "! You won!" << endl;
    }
    else
    {
        cout << "Congrats " << player1.playerName << "! You won!" << endl;
    }
}

ShipPositionType GetRandomPosition()
{
    ShipPositionType guess;

    guess.row = rand() % BOARD_SIZE;
    guess.col = rand() % BOARD_SIZE;

    return guess;
}

ShipPositionType GetAIGuess(const Player& aiPlayer)
{
    return GetRandomPosition();
}

/* End Game Functions */

/* Board Functions */

void SetupBoards(Player& player)
{

    ClearBoards(player);

    if (player.playerType == PT_AI)
    {
        SetupAIBoards(player);
        return;
    }

    for (int i = 0; i < NUM_SHIPS; i++)
    {
        DrawBoards(player);

        Ship& currentShip = player.ships[i];

        ShipPositionType shipPosition;
        ShipOrientationType orientation;

        bool isValidPlacement = false;

        do
        {
            cout << player.playerName << " please set the potision and orientation for your " << GetShipNameForShipType(currentShip.shipType) << endl;

            shipPosition = GetBoardPosition();
            orientation = GetShipOrientation();

            isValidPlacement = IsValidPlacement(player, currentShip, shipPosition, orientation);

            if (!isValidPlacement)
            {
                cout << "That was not a valid placement. Please try again." << endl;
                WaitForKeyPress();
            }

        } while (!isValidPlacement);

        PlaceShipOnBoard(player, currentShip, shipPosition, orientation);

    }

    DrawBoards(player);
    WaitForKeyPress();

}

void ClearBoards(Player& player)
{
    for (int r = 0; r < BOARD_SIZE; r++)
    {
        for (int c = 0; c < BOARD_SIZE; c++)
        {
            player.guessBoard[r][c] = GT_NONE;
            player.shipBoard[r][c].shipType = ST_NONE;
            player.shipBoard[r][c].isHit = false;
        }
    }
}

void DrawBoards(const Player& player)
{
    ClearScreen();

    DrawColumnsRow();

    DrawColumnsRow();

    cout << endl;

    for (int r = 0; r < BOARD_SIZE; r++)
    {
        DrawSeparatorLine();

        cout << " ";

        DrawSeparatorLine();

        cout << endl;

        DrawShipBoardRow(player, r);

        cout << " ";

        DrawGuessBoardRow(player, r);

        cout << endl;
    }

    DrawSeparatorLine();

    cout << " ";

    DrawSeparatorLine();

    cout << endl;
}


ShipType UpdateBoards(ShipPositionType guess, Player& currentPlayer, Player& otherPlayer)
{
    if (otherPlayer.shipBoard[guess.row][guess.col].shipType != ST_NONE)
    {
        currentPlayer.guessBoard[guess.row][guess.col] = GT_HIT;
        otherPlayer.shipBoard[guess.row][guess.col].isHit = true;
    }
    else
    {
        currentPlayer.guessBoard[guess.row][guess.col] = GT_MISSED;
    }

    return otherPlayer.shipBoard[guess.row][guess.col].shipType;
}

void SetupAIBoards(Player& player)
{
    ShipPositionType pos;
    ShipOrientationType orientation;

    for (int i = 0; i < NUM_SHIPS; i++)
    {
        Ship& currentShip = player.ships[i];

        do
        {
            pos = GetRandomPosition();
            orientation = ShipOrientationType(rand() % 2);

        } while (!IsValidPlacement(player, currentShip, pos, orientation));

        PlaceShipOnBoard(player, currentShip, pos, orientation);
    }
}

/* End of Board Functions */

/* Drawing Board Functions */

void DrawSeparatorLine()
{
    cout << " ";

    for (int c = 0; c < BOARD_SIZE; c++)
    {
        cout << "+---";
    }

    cout << "+";
}

void DrawColumnsRow()
{
    cout << "  ";
    for (int c = 0; c < BOARD_SIZE; c++)
    {
        int columnName = c + 1;

        cout << " " << columnName << "  ";
    }
}

void DrawShipBoardRow(const Player& player, int row)                    
{
    char rowName = row + 'A';                                           // Setting row name variable to the letter, first row is 'A', then adding 1 to it each time it's called.

    cout << rowName << "|";

    for (int c = 0; c < BOARD_SIZE; c++)
    {
        cout << " " << GetShipRepresentationAt(player, row, c) << " |";
    }
}

void DrawGuessBoardRow(const Player& player, int row)
{
    char rowName = row + 'A';                                         // Setting row name variable to the letter, first row is 'A', then adding 1 to it each time it's called.  

    cout << rowName << "|";

    for (int c = 0; c < BOARD_SIZE; c++)
    {
        cout << " " << GetGuessRepresentationAt(player, row, c) << " |"; // Grab ship representation for guess board
    }
}

/* End Board Draw Functions */

/* Drawing of the Squares Functions */



char GetShipRepresentationAt(const Player& player, int row, int col)      // NOTE: Could have used a switch statement here, consider changing in future
{
    if (player.shipBoard[row][col].isHit)
    {
        return '*';                                                     // represents a hit
    }
    if (player.shipBoard[row][col].shipType == ST_AIRCRAFT_CARRIER)
    {
        return 'A';
    }
    else if (player.shipBoard[row][col].shipType == ST_BATTLESHIP)
    {
        return 'B';
    }
    else if (player.shipBoard[row][col].shipType == ST_CRUISER)
    {
        return 'C';
    }
    else if (player.shipBoard[row][col].shipType == ST_DESTROYER)
    {
        return 'D';
    }
    else if (player.shipBoard[row][col].shipType == ST_SUBMARINE)
    {
        return 'S';
    }
    else
    {
        return ' ';
    }
}

char GetGuessRepresentationAt(const Player& player, int row, int col)
{
    if (player.guessBoard[row][col] == GT_HIT)
    {
        return '*';
    }
    else if(player.guessBoard[row][col] == GT_MISSED)
    {
        return 'o';
    }
    else
    {
        return ' ';
    }
}

/* End of Drawing of Squares Functions */

const char* GetShipNameForShipType(ShipType shipType)
{
    if (shipType == ST_AIRCRAFT_CARRIER)
    {
        return "Aircraft Carrier";
    }
    else if(shipType == ST_BATTLESHIP)
    {
        return "Battleship";
    }
    else if(shipType == ST_CRUISER)
    {
        return "Cruiser";
    }
    else if (shipType == ST_DESTROYER)
    {
        return "Destroyer";
    }
    else if (shipType == ST_SUBMARINE)
    {
        return "Submarine";
    }

    return "None";
}

ShipPositionType MapBoardPosition(char rowInput, int colInput)
{
    int realRow = rowInput - 'A';
    int realCol = colInput - 1;

    ShipPositionType boardPosition;

    boardPosition.row = realRow;
    boardPosition.col = realCol;

    return boardPosition;
}

ShipPositionType GetBoardPosition()
{
    char rowInput;
    int colInput;

    const char validRowInputs[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };
    const int validColumnInputs[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    rowInput = GetCharacter("Please input a row(A - J): ", INPUT_ERROR_STRING, validRowInputs, BOARD_SIZE, CC_UPPER_CASE);
    colInput = GetInteger("Please input a col(1-10)", INPUT_ERROR_STRING, validColumnInputs, BOARD_SIZE);
    
    return MapBoardPosition(rowInput, colInput);
}

ShipOrientationType GetShipOrientation()
{

    const char validInput[2] = { 'H', 'V' };

    char input = GetCharacter("Please choose an orientation (H) for Horizontal or (V) for Vertical: ", INPUT_ERROR_STRING, validInput, 2, CC_UPPER_CASE);

    if (input == validInput[0])
    {
        return SO_HORIZONTAL;
    }
    else
    {
        return SO_VERTICAL;
    }
}

bool IsValidPlacement(const Player& player, const Ship& currentShip, const ShipPositionType shipPosition, ShipOrientationType orientation)
{

    if (orientation == SO_HORIZONTAL)
    {
        for (int c = shipPosition.col; c < (shipPosition.col + currentShip.shipSize); c++)
        {
            if (player.shipBoard[shipPosition.row][c].shipType != ST_NONE || c >= BOARD_SIZE)
            {
                return false;
            }
        }
    }
    else {
        for (int r = shipPosition.row; r < (shipPosition.row + currentShip.shipSize); r++)
        {
            if (player.shipBoard[r][shipPosition.col].shipType != ST_NONE || r >= BOARD_SIZE)
            {
                return false;
            }
        }
    }

    return true;
}

void PlaceShipOnBoard(Player& player, Ship& currentShip, const ShipPositionType shipPosition, const ShipOrientationType orientation)
{
    currentShip.shipPosition = shipPosition;
    currentShip.shipOrientation = orientation;

    if (orientation == SO_HORIZONTAL)
    {
        for (int c = shipPosition.col; c < (shipPosition.col + currentShip.shipSize); c++)
        {
            player.shipBoard[shipPosition.row][c].shipType = currentShip.shipType;
            player.shipBoard[shipPosition.row][c].isHit = false;
        }
    }
    else
    {
        for (int r = shipPosition.row; r < (shipPosition.row + currentShip.shipSize); r++)
        {
            player.shipBoard[r][shipPosition.col].shipType = currentShip.shipType;
            player.shipBoard[r][shipPosition.col].isHit = false;
        }
    }
}

/* Player/Ship Initializations functions */

void InitializePlayer(Player& player, const char* playerName)
{
    if (playerName != nullptr && strlen(playerName) > 0)
    {
        strcpy_s(player.playerName, playerName);
    }

    InitializeShip(player.ships[0], AIRCRAFT_CARRIER_SIZE, ST_AIRCRAFT_CARRIER);
    InitializeShip(player.ships[1], BATTLESHIP_SIZE, ST_BATTLESHIP);
    InitializeShip(player.ships[2], CRUISER_SIZE, ST_CRUISER);
    InitializeShip(player.ships[3], DESTROYER_SIZE, ST_DESTROYER);
    InitializeShip(player.ships[4], SUBMARINE_SIZE, ST_SUBMARINE);

}

void InitializeShip(Ship& ship, int shipSize, ShipType shipType)
{
    ship.shipType = shipType;
    ship.shipSize =shipSize;
    ship.shipPosition.row = 0;
    ship.shipPosition.col = 0;
    ship.shipOrientation = SO_HORIZONTAL;
}

/*

SetupBoards(player)
-------------------

ClearBoards(player)

for(all the ships)
{
    DrawBoards(player)

    currentShip = get the current ship

    do
    {
        Get Board Position for the head of the ship
        Get the ship orientation

        isValidPlacement = ISValidPlacement(currentShip, position, orientation, player)

        if(!ISValidPlacement(currentShip, position, orientation, player))
        {
            output to the player it was not a valid placement
        }
    }while(!IsValidPlacement)

    PlcaeShipOnBoard(player, currentShip, position, orientation)
}

bool IsValidPlacement(currentShip, position, orientation, player)
--------------------------------------------------------------

if(orientation == HORIZONTAL)
{
    for(all the columns the currentShip would take up)
    {
        if(current position on the player's ship board is taken or if the ship will be off the board horizontally)
        {
            return false;
        }
    }
}
else
{
    for(all the rows the cuurentShip would take up)
    {
        if(currentShip will overlap anouther ship or the ship will be off the board vertically)
        {
            return false;
        }
    }
}

return true;


PlaceShipOnBoard(player, ship, position, orientation)
--------------------------------------------------------

ship.position = position;
ship.orientation = orientation;

if(orientation == HORIZAONTAL)
{
    for(all the columns the ship would take up)
    {
        set the ship part on the board at position.row and current column
    }
}
else
{
    for(all the rows the ship would take up)
    {
        set the ship part on the board at current row and position.col
    }
}

UpdateBoards(guess, currentPlayer, otherPlayer)
------------------------------------------------

if(otherPlayer's ship board at guess is a ship)
{
    that's a hit
    set hit on the currentPlayer's guess board
    apply damage to the otherPlayer's shipBoard
    return shipType;
}
else
{
    set miss on the currentPlayer's guess board

    return ST_NONE;
}

IsGameOver(player1, player2)
-----------------------------

return AreAllShipsSunk(player1) || AreAllShipsSunk(player2)

bool AreAllShipsSunk(player)
---------------------------

for(all the player's ships)
{
    if(!IsSunk(player, currentShip)
    {
        return false;
    }
}
return true;

ISSunk(player, ship)
----------------------

if(ship.orientation == HORIZONTAL)
{
    for(columns the ship takes up)
    {
        if(currentPosition on the shipBoard is not hit)
        {
            return false;
        }
    }
}
else
{
    for(rows the ship takes up)
    {
        if(currentPosition on the shipBoard is not hit)
        {
            return false;
        }
    }
}

return true;



*/