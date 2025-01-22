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
        bool playGame(Player& user, char response);

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

// Reads a given file and turns it into a map
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
    file.close();
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
// Maybe add a search algorithm, such as A* in the 4.0
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
bool Map::playGame(Player& user, char response) {
    // Loops until user wins or loses
    do {
        system("clear");
        printBoard(user);
        if (user.x == computer.x && user.y == computer.y) break;
        response = getChar();
        moveUser(user, response);
        moveComputer();
    } while(get(user.y, user.x) != 2);

    // Displays whether a loss or victory occurred
    if (get(user.y, user.x) == 2) {
        system("clear");
        printBoard(user);
        return true;}
    return false;
}

// Checks to see if the program begins in the Pitfall directory
// Ensures access to maps is maintained
bool checkDirectory() {
    std::string dir;
    std::fstream outFile("directory.txt", std::ios::out);
    std::fstream inFile("directory.txt", std::ios::in);
    system("pwd >> directory.txt");
    outFile.close();
    std::getline(inFile, dir);
    remove("directory.txt");
    if (dir.find("pitfall") == std::string::npos && dir.find("Pitfall") == std::string::npos) {
        std::cout << "\n\033[1;31mPlease open the application from within the Pitfall directory!\n";
        std::cout << "e.g. Current working directory = .../Pitfall\033[0m\n";
        return false;
    }
    return true;
}

int main() {
    if (checkDirectory()) {
        char response;
        std::vector<Map> game; // Vector that contains each map
        Map m1 = readMap("maps/m1.txt", 1); // Level 1 of the game
        game.push_back(m1);
        Map m2 = readMap("maps/m2.txt", 2); // Level 2 of the game
        game.push_back(m2);
        Map m3 = readMap("maps/m3.txt", 3); // Level 3 of the game
        game.push_back(m3);
        Map m4 = readMap("maps/m4.txt", 4); // Level 4 of the game
        game.push_back(m4);
        Map m5 = readMap("maps/m5.txt", 5); // Level 5 of the game
        game.push_back(m5);
        Map m6 = readMap("maps/m6.txt", 6); // Level 6 of the game
        game.push_back(m6);
        Map m7 = readMap("maps/m7.txt", 7); // Level 7 of the game
        game.push_back(m7);
        Map m8 = readMap("maps/m8.txt", 8); // Level 8 of the game
        game.push_back(m8);
        Map m9 = readMap("maps/m9.txt", 9); // Level 9 of the game
        game.push_back(m9);

        // Initial location
        Player user = {1, 1};

        system("clear");
        printInstructions();
        std::cin >> response;
        user.symbol = response; // Gets user's symbol
        std::cin.clear();
        fflush(stdin);

        // Runs each round/map
        for (int i = 0; i < game.size(); i++) {
            bool won = game.at(i).playGame(user, response); // Plays game
            if (won && i == game.size() - 1) game.at(i).congratulate(" Congratulations! "); // Checks to see if the user has won the game
            else if (won) continue; // Checks to see if the user can move on to the next maze
            else {
                game.at(i).congratulate(" Defeat ");
                break;
            }
        }
    }

    return 0;
}
