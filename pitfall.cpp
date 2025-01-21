#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <ctime>
#include <fstream>
#include <vector>

// Every given player has a coordinate and symbol
struct Player {
    int x;
    int y;
    char symbol;
};

// Class that represents the entire board
class Map {
    public:
        Map() {}
        Map(int numRows, int numCols, int level) {
            this->level = level;
            this->numRows = numRows;
            this->numCols = numCols;
            board = new int*[numRows];
            for (int i = 0; i < numRows; i++) {
                board[i] = new int[numCols];
                for (int j = 0; j < numCols; j++) {
                    board[i][j] = 0;
                }
            }
        }
        int getRows() { return numRows; }
        int getCols() { return numCols; }
        void setRows(int numRows) { this->numRows = numRows; }
        void setCols(int numCols) { this->numCols = numCols; }
        void setLevel(int level) { this->level = level; }
        int getLevel() { return level; }
        int get(int row, int col) {
            if (row >= 0 && row < numRows && col >= 0 && col < numCols) return board[row][col];
            else return -1;
        }
        void set(int row, int col, int data) {
            if (row >= 0 && row < numRows && col >= 0 && col < numCols) board[row][col] = data;
        }
        void setComputer(int x, int y) {
            computer.x = x;
            computer.y = y;
            computer.symbol = '@';
        }
        void printHeader();
        void printBoard(Player user);
        void moveUser(Player& user, char response);
        void moveComputer();
        void congratulate(std::string word);
        bool playGame(Map& m1, Player& user, char response);

    private:
        int** board;
        int numRows;
        int numCols;
        int level;
        Player computer;
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

// Reads a given files and turns it into a map
Map readMap(std::string map, int level) {
    std::fstream file(map, std::ios::in);
    int rows;
    int cols;
    int computerX;
    int computerY;
    int currNum;

    // Sets the size of the map
    file >> rows;
    file >> cols;
    Map m1(rows, cols, level);
    // Sets the location of the computer
    file >> computerX;
    file >> computerY;
    m1.setComputer(computerX, computerY);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            file >> currNum;
            m1.set(i, j, currNum);
        }
    }
    return m1;
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

// Prints the current level
void Map::printHeader() {
    for (int i = 0; i < getCols() + 2; i++) std::cout << "+";
    std::cout << '\n';
    for (int i = 0; i < getCols() / 2 - 3; i++) std::cout << "+";
    std::cout << " Maze " << getLevel() << ' ';
    for (int i = 0; i < getCols() / 2 - 3; i++) std::cout << "+";
    std::cout << '\n';
    for (int i = 0; i < getCols() + 2; i++) std::cout << "+";
    std::cout << '\n';
}

// Prints the board
// User is displayed as blue, computer as red, and the goal is green
void Map::printBoard(Player user) {
    printHeader();
    // Prints walls at the top
    std::cout << "┌";
    for (int i = 0; i < getCols(); i++) std::cout << "─";
    std::cout << "┐\n";

    for (int i = 0; i < getRows(); i++) {
        std::cout << '|';
        for (int j = 0; j < getCols(); j++) {
            if (i == user.y && j == user.x && get(i, j) == 2) { // If the user made it to the '$', then display their symbol as green since they won
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
            else if (get(i, j) == 1) std::cout << '#'; // Prints walls
            else if (get(i, j) == 2) std::cout << "\033[32m$\033[0m"; // Prints open space
            else std::cout << ' ';
        }
        std::cout << '|' << '\n';
    }
    // Prints walls at the bottom
    std::cout << "└";
    for (int i = 0; i < getCols(); i++) std::cout << "─";
    std::cout << "┘\n";
}

// Changes the user's location based on their input
void Map::moveUser(Player& user, char response) {
    // Switch ensures a valid key was pressed
    switch(response) {
            case 'a': if (user.y >= 0 && user.y < getRows() && user.x - 1 >= 0 && user.x - 1 < getCols() && !(user.x - 1 == computer.x && user.y == computer.y) && get(user.y, user.x - 1) != 1) user.x--;
                      break;
            case 's': if (user.y + 1 >= 0 && user.y + 1 < getRows() && user.x >= 0 && user.x < getCols() && !(user.x == computer.x && user.y + 1 == computer.y) && get(user.y + 1, user.x) != 1) user.y++;
                      break;
            case 'd': if (user.y >= 0 && user.y < getRows() && user.x + 1 >= 0 && user.x + 1 < getCols() && !(user.x + 1 == computer.x && user.y == computer.y) && get(user.y, user.x + 1) != 1) user.x++;
                      break;
            case 'w': if (user.y - 1 >= 0 && user.y - 1 < getRows() && user.x >= 0 && user.x < getCols() && !(user.x == computer.x && user.y - 1 == computer.y) && get(user.y - 1, user.x) != 1) user.y--;
                      break;
            default:
                      break;
    }
}

// Changes the computer's location based on random movement
// Maybe add a search algorithm, such as A* in the 3.0
void Map::moveComputer() {
    srand(time(0));
    int response;
    do {
        response = rand() % 4;
        switch(response) {
            case 0: if (computer.y >= 0 && computer.y < 20 && computer.x - 1 >= 0 && computer.x - 1 < 40 && get(computer.y, computer.x - 1) != 1) {
                        computer.x--;
                        response = -1;
                    }
                    break;
            case 1: if (computer.y + 1>= 0 && computer.y + 1< 20 && computer.x >= 0 && computer.x < 40 && get(computer.y + 1, computer.x) != 1) {
                        computer.y++;
                        response = -1;
                    }
                    break;
            case 2: if (computer.y >= 0 && computer.y < 20 && computer.x + 1 >= 0 && computer.x + 1 < 40 && get(computer.y, computer.x + 1) != 1) {
                        computer.x++;
                        response = -1;
                    }
                    break;
            case 3: if (computer.y - 1 >= 0 && computer.y - 1 < 20 && computer.x >= 0 && computer.x < 40 && get(computer.y - 1, computer.x) != 1) {
                        computer.y--;
                        response = -1;
                    }
                    break;
        }
    } while (response != -1);
}

// Print whether a victory or defeat occured
void Map::congratulate(std::string word) {
    for (int i = 0; i < getCols() + 2; i++) std::cout << "+";
    std::cout << '\n';
    for (int i = 0; i < getCols() / 2 - 3; i++) std::cout << "+";
    std::cout << word;
    for (int i = 0; i < getCols() / 2 - 3; i++) std::cout << "+";
    std::cout << '\n';
    for (int i = 0; i < getCols() + 2; i++) std::cout << "+";
    std::cout << '\n';
}

// Plays the game on a certain map
bool Map::playGame(Map& m1, Player& user, char response) {
    // Loops until user wins or loses
    do {
        system("clear");
        m1.printBoard(user);
        if (user.x == computer.x && user.y == computer.y) break;
        response = getChar();
        m1.moveUser(user, response);
        m1.moveComputer();
    } while(m1.get(user.y, user.x) != 2);

    // Displays whether a loss or victory occurred
    if (m1.get(user.y, user.x) == 2) {
        system("clear");
        m1.printBoard(user);
        return true;}
    return false;
}

int main() {
    char response;
    std::vector<Map> match; // Vector that contains each map
    Map m1 = readMap("m1.txt", 1); // Level 1 of the match
    match.push_back(m1);
    
    // Initial location
    Player user = {1, 1};

    system("clear");
    printInstructions();
    std::cin >> response;
    user.symbol = response; // Gets user's symbol
    std::cin.clear();
    fflush(stdin);

    for (int i = 0; i < match.size(); i++) {
        bool won = match.at(i).playGame(match.at(i), user, response);
        if (won && i == match.size() - 1) match.at(i).congratulate(" Winner ");
        else if (won) continue;
        else {
            match.at(i).congratulate(" Defeat ");
            break;
        }
    }

    return 0;
}