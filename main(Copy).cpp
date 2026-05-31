
#include <iostream>
#include <unistd.h>     // For usleep()
#include <termios.h>    // For Linux terminal control
#include <fcntl.h>      // For non-blocking input
#include <ctime>        // For time() seed
#include <cstdlib>

using namespace std;

const int WIDTH = 40;
const int HEIGHT = 20;
const int MAX_SNAKE_LENGTH = 100;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction dir;

bool gameOver;
int score;
int headX, headY, fruitX, fruitY;
int tailX[MAX_SNAKE_LENGTH], tailY[MAX_SNAKE_LENGTH];
int nTail;

// Linux equivalent of kbhit()
int linux_kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

// Linux equivalent of getch()
int linux_getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void Setup() {
    gameOver = false;
    dir = STOP;
    headX = WIDTH / 2;
    headY = HEIGHT / 2;
    nTail = 0;
    score = 0;
    srand(static_cast<unsigned int>(time(NULL)));
    fruitX = rand() % (WIDTH - 2) + 1;
    fruitY = rand() % (HEIGHT - 2) + 1;
}

void Draw() {
    // Clear Linux Terminal
    cout << "\033[2J\033[1;1H";

    for (int i = 0; i < WIDTH + 2; i++) cout << "#";
    cout << "\n";

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x <= WIDTH + 1; x++) {
            if (x == 0 || x == WIDTH + 1) {
                cout << "#";
            } else {
                int currentX = x - 1;
                if (currentX == headX && y == headY) cout << "O";
                else if (currentX == fruitX && y == fruitY) cout << "F";
                else {
                    bool isTailPrinted = false;
                    for (int k = 0; k < nTail; k++) {
                        if (tailX[k] == currentX && tailY[k] == y) {
                            cout << "o";
                            isTailPrinted = true;
                            break;
                        }
                    }
                    if (!isTailPrinted) cout << " ";
                }
            }
        }
        cout << "\n";
    }

    for (int i = 0; i < WIDTH + 2; i++) cout << "#";
    cout << "\n\n";

    cout << "========================================" << "\n";
    cout << "  SCORE PANEL: " << score << " points" << "\n";
    cout << "  CONTROLS   : W (UP) | S (DOWN) | A (LEFT) | D (RIGHT) | X (EXIT)" << "\n";
    cout << "========================================" << "\n";
}

void Input() {
    if (linux_kbhit()) {
        switch (linux_getch()) {
            case 'a': case 'A': if (dir != RIGHT || nTail == 0) dir = LEFT; break;
            case 'd': case 'D': if (dir != LEFT || nTail == 0) dir = RIGHT; break;
            case 'w': case 'W': if (dir != DOWN || nTail == 0) dir = UP; break;
            case 's': case 'S': if (dir != UP || nTail == 0) dir = DOWN; break;
            case 'x': case 'X': gameOver = true; break;
        }
    }
}

void Logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = headX;
    tailY[0] = headY;

    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir) {
        case LEFT:  headX--; break;
        case RIGHT: headX++; break;
        case UP:    headY--; break;
        case DOWN:  headY++; break;
        default:             break;
    }

    if (headX < 0 || headX >= WIDTH || headY < 0 || headY >= HEIGHT) gameOver = true;

    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == headX && tailY[i] == headY) gameOver = true;
    }

    if (headX == fruitX && headY == fruitY) {
        score += 10;
        if (nTail < MAX_SNAKE_LENGTH) nTail++;
        fruitX = rand() % (WIDTH - 2) + 1;
        fruitY = rand() % (HEIGHT - 2) + 1;
    }
}

int main() {
    char replayChoice;
    do {
        Setup();
        while (!gameOver) {
            Draw();
            Input();
            Logic();
            // usleep uses microseconds (1 millisecond = 1000 microseconds)
            if (dir == UP || dir == DOWN) usleep(80000); 
            else usleep(50000);
        }

        cout << "\nGAME OVER! Your final score: " << score << "\n";
        cout << "Would you like to play again? (Y/N): ";
        cin >> replayChoice;
    } while (replayChoice == 'y' || replayChoice == 'Y');

    return 0;
}
