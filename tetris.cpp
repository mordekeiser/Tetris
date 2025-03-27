#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>

using namespace std;

struct TerminalInput {
    struct termios old_settings, new_settings;

    TerminalInput() {
        tcgetattr(STDIN_FILENO, &old_settings);
        new_settings = old_settings;
        new_settings.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_settings);
    }

    ~TerminalInput() {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_settings);
    }

    bool hasInput() {
        struct timeval timeout = {0, 1000};
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout) > 0;
    }

    char getInput() {
        char ch;
        if (read(STDIN_FILENO, &ch, 1) > 0) {
            return ch;
        }
        return 0;
    }
};

const int WIDTH = 10;
const int HEIGHT = 20;

vector<vector<vector<bool>>> tetrominoes = {
    { {1, 1, 1, 1} },
    { {1, 1}, {1, 1} },
    { {0, 1, 0}, {1, 1, 1} },
    { {1, 1, 0}, {0, 1, 1} },
    { {0, 1, 1}, {1, 1, 0} },
    { {1, 0, 0}, {1, 1, 1} },
    { {0, 0, 1}, {1, 1, 1} }
};

struct Tetromino {
    vector<vector<bool>> shape;
    int x, y;

    Tetromino(vector<vector<bool>> s) {
        shape = s;
        x = WIDTH / 2 - s[0].size() / 2;
        y = 0;
    }

    void rotate() {
        int newHeight = shape[0].size();
        int newWidth = shape.size();
        vector<vector<bool>> rotated(newHeight, vector<bool>(newWidth));

        for (int i = 0; i < newWidth; i++) {
            for (int j = 0; j < newHeight; j++) {
                rotated[j][newWidth - 1 - i] = shape[i][j];
            }
        }

        shape = rotated;
    }
};

vector<vector<bool>> grid(HEIGHT, vector<bool>(WIDTH, false));
Tetromino* current_piece;
int score = 0, level = 1, speed = 500;
bool fastDrop = false;
TerminalInput input;
random_device rd;
mt19937 gen(rd());

Tetromino* getRandomPiece() {
    int index = gen() % tetrominoes.size();
    return new Tetromino(tetrominoes[index]);
}

bool isValidMove(Tetromino &piece) {
    for (int i = 0; i < piece.shape.size(); i++) {
        for (int j = 0; j < piece.shape[i].size(); j++) {
            if (piece.shape[i][j]) {
                int newX = piece.x + j;
                int newY = piece.y + i;

                if (newX < 0 || newX >= WIDTH || newY < 0 || newY >= HEIGHT) {
                    return false;
                }
                if (grid[newY][newX]) {
                    return false;
                }
            }
        }
    }
    return true;
}

void placePiece() {
    for (int i = 0; i < current_piece->shape.size(); i++) {
        for (int j = 0; j < current_piece->shape[i].size(); j++) {
            if (current_piece->shape[i][j]) {
                int newX = current_piece->x + j;
                int newY = current_piece->y + i;
                if (newY >= 0 && newY < HEIGHT && newX >= 0 && newX < WIDTH) {
                    grid[newY][newX] = true;
                }
            }
        }
    }
}

void clearLines() {
    int linesCleared = 0;
    for (int y = HEIGHT - 1; y >= 0; y--) {
        bool fullLine = true;
        for (int x = 0; x < WIDTH; x++) {
            if (!grid[y][x]) {
                fullLine = false;
                break;
            }
        }

        if (fullLine) {
            grid.erase(grid.begin() + y);
            grid.insert(grid.begin(), vector<bool>(WIDTH, false));
            linesCleared++;
        }
    }

    if (linesCleared > 0) {
        score += linesCleared * 10;
        level = score / 30 + 1;
        speed = max(100, 500 - (level * 40));
    }
}

void drawGrid() {
    system("clear");

    vector<vector<bool>> tempGrid = grid;

    for (int i = 0; i < current_piece->shape.size(); i++) {
        for (int j = 0; j < current_piece->shape[i].size(); j++) {
            if (current_piece->shape[i][j]) {
                int newX = current_piece->x + j;
                int newY = current_piece->y + i;
                if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT) {
                    tempGrid[newY][newX] = true;
                }
            }
        }
    }

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (tempGrid[i][j]) {
                cout << "[]";
            } else {
                cout << " .";
            }
        }
        cout << "\n";
    }
    cout << "Score: " << score << " | Level: " << level << " | Speed: " << speed << "ms\n";
}

void runGame() {
    current_piece = getRandomPiece();
    auto lastDropTime = chrono::steady_clock::now();

    while (true) {
        drawGrid();

        if (input.hasInput()) {
            char ch = input.getInput();
            Tetromino temp_piece = *current_piece;

            if (ch == 'a') {
                temp_piece.x--;
            } else if (ch == 'd') {
                temp_piece.x++;
            } else if (ch == 'w') {
                temp_piece.rotate();
            } else if (ch == 's') {
                fastDrop = true;
            } else if (ch == 'q') {
                return;
            }

            if (isValidMove(temp_piece)) {
                *current_piece = temp_piece;
            }
        } else {
            fastDrop = false;
        }

        auto now = chrono::steady_clock::now();
        int dropSpeed = fastDrop ? 50 : speed;

        if (chrono::duration_cast<chrono::milliseconds>(now - lastDropTime).count() > dropSpeed) {
            lastDropTime = now;
            Tetromino fallingPiece = *current_piece;
            fallingPiece.y++;
            if (isValidMove(fallingPiece)) {
                *current_piece = fallingPiece;
            } else {
                placePiece();
                clearLines();
                delete current_piece;
                current_piece = getRandomPiece();
                if (!isValidMove(*current_piece)) {
                    cout << "GAME OVER!\n";
                    return;
                }
            }
        }

        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

int main() {
    runGame();
    return 0;
}
