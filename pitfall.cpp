#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <ctime>

// Macroidentifiers for rows and cols to make them publicly accesible
// For the 2.0, maybe add a class that wraps both the 2-D array (board) and its numRows and numCols
// This would allow you have multiple maps in a much more condensed format
#define ROWS 20
#define COLS 40

// Every given player has a coordinate and symbol
struct Player {
    int x;
    int y;
    char symbol;
};

// Gets user input (key pressed) without the user having to press enter/return
char getChar() {
    struct termios oldt, newt;
    char ch;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    
    return ch;
}

// Prints the instructions at the beginning of the game
void printInstructions() {
    std::cout << "***********************************\n";
    std::cout << "*    Welcome to the Pitfall!!!    *\n";
    std::cout << "*  You need to escape as quickly  *\n";
    std::cout << "*    as possible to ensure the    *\n";
    std::cout << "*  monster '\033[1;31m@\033[0m' does not catch up  *\n";
    std::cout << "*  to you! In order to escape the *\n";
    std::cout << "* maze, you must find the path to *\n";
    std::cout << "* this token: \'\033[32m$\033[0m\' \t\t  *\n";
    std::cout << "*********** !GOOD LUCK! ***********\n";
    std::cout << "Choose your character: ";
}

// Prints the current maze/level
void printHeader() {
    for (int i = 0; i < COLS + 2; i++) std::cout << "+";
    std::cout << '\n';
    for (int i = 0; i < COLS / 2 - 3; i++) std::cout << "+";
    std::cout << " Maze 1 ";
    for (int i = 0; i < COLS / 2 - 3; i++) std::cout << "+";
    std::cout << '\n';
    for (int i = 0; i < COLS + 2; i++) std::cout << "+";
    std::cout << '\n';
}

// Prints the board
// User is displayed as blue, computer as red, and the goal is green
void printBoard(int board[ROWS][COLS], Player user, Player computer) {
    printHeader();
    // Prints walls at the top
    std::cout << "┌";
    for (int i = 0; i < COLS; i++) std::cout << "─";
    std::cout << "┐\n";

    for (int i = 0; i < ROWS; i++) {
        std::cout << '|';
        for (int j = 0; j < COLS; j++) {
            if (i == user.y && j == user.x && board[i][j] == 2) { // If the user made it to the '$', then display their symbol as green since they won
                std::cout << "\033[1;32m" << user.symbol << "\033[0m";
            }
            // Checks for printing the computer's symbol
            // Computer has precedence over the user since touching the computer results in a loss
            else if (i == computer.y && j == computer.x) {
                std::cout << "\033[1;31m" << computer.symbol << "\033[0m";
            }
            else if (i == user.y && j == user.x) { // Checks for printing the user
                std::cout << "\033[34m" << user.symbol << "\033[0m";
            }
            else if (board[i][j] == 1) std::cout << '#'; // Prints walls
            else if (board[i][j] == 2) std::cout << "\033[32m$\033[0m"; // Prints open space
            else std::cout << ' ';
        }
        std::cout << '|' << '\n';
    }
    // Prints walls at the bottom
    std::cout << "└";
    for (int i = 0; i < COLS; i++) std::cout << "─";
    std::cout << "┘\n";
}

// Changes the user's location based on their input
void moveUser(int board[ROWS][COLS], Player& user, char response) {
    // Switch ensures a valid key was pressed
    switch(response) {
            case 'a': if (board[user.y][user.x - 1] != 1) user.x--;
                      break;
            case 's': if (board[user.y + 1][user.x] != 1) user.y++;
                      break;
            case 'd': if (board[user.y][user.x + 1] != 1) user.x++;
                      break;
            case 'w': if (board[user.y - 1][user.x] != 1) user.y--;
                      break;
            default:
                      break;
    }
}

// Changes the computer's location based on random movement
// Maybe add a search algorithm, such as A* in the 2.0
void moveComputer(int board[ROWS][COLS], Player& computer) {
    srand(time(0));
    int response;
    do {
        response = rand() % 4;
        switch(response) {
            case 0: if (board[computer.y][computer.x - 1] != 1) {
                        computer.x--;
                        response = -1;
                    }
                    break;
            case 1: if (board[computer.y + 1][computer.x] != 1) {
                        computer.y++;
                        response = -1;
                    }
                    break;
            case 2: if (board[computer.y][computer.x + 1] != 1) {
                        computer.x++;
                        response = -1;
                    }
                    break;
            case 3: if (board[computer.y - 1][computer.x] != 1) {
                        computer.y--;
                        response = -1;
                    }
                    break;
        }
    } while (response != -1);
}

// Print whether a victory or defeat occured
void congratulate(std::string word) {
    for (int i = 0; i < COLS + 2; i++) std::cout << "+";
    std::cout << '\n';
    for (int i = 0; i < COLS / 2 - 3; i++) std::cout << "+";
    std::cout << word;
    for (int i = 0; i < COLS / 2 - 3; i++) std::cout << "+";
    std::cout << '\n';
    for (int i = 0; i < COLS + 2; i++) std::cout << "+";
    std::cout << '\n';
}

int main() {
    char response;
    // Level one of the board
    int board[ROWS][COLS] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1},
        {1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1},
        {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1},
        {1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1},
        {1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 2, 1},
        {1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1},
        {1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1},
        {1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1},
        {1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1}
    };
    Player user = {1, 1};
    Player computer = {37, 13, '@'};

    system("clear");
    printInstructions();
    std::cin >> response;
    user.symbol = response; // Gets user's symbol
    std::cin.clear();
    fflush(stdin);

    system("clear");
    printBoard(board, user, computer);

    // Loops until user wins or loses
    do {
        response = getChar();
        moveUser(board, user, response);
        moveComputer(board, computer);
        system("clear");
        printBoard(board, user, computer);
        if (user.x == computer.x && user.y == computer.y) break;
    } while(board[user.y][user.x] != 2);

    // Displays whether a loss or victory occurred
    if (board[user.y][user.x] == 2) congratulate(" Winner ");
    else if (user.x == computer.x && user.y == computer.y) congratulate(" Defeat ");

    return 0;
}