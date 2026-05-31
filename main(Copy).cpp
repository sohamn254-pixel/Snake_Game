/**
 * @file main.cpp
 * @brief Console-Based Snake Game Implementation
 * @details Showcases C++ fundamental concepts: Arrays, Loops, Conditional Logic,
 * and Asynchronous Hardware Input Polling.
 */

#include <iostream>
#include <conio.h>   // For _kbhit() and _getch()
#include <windows.h> // For Sleep()
#include <ctime>     // For time() seed

using namespace std;

// Global Game Configuration
const int WIDTH = 40;
const int HEIGHT = 20;
const int MAX_SNAKE_LENGTH = 100;

// Game State Enum
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction dir;

// Structural Game Variables
bool gameOver;
int score;
int headX, headY;
int fruitX, fruitY;

// Track body coordinates using linear arrays
int tailX[MAX_SNAKE_LENGTH];
int tailY[MAX_SNAKE_LENGTH];
int nTail; // Current length of the tail

/**
 * @brief Re-initializes and seeds the structural variables for a fresh session.
 */
void Setup() {
    gameOver = false;
    dir = STOP; // Game starts stationary until user sends an input vector
    headX = WIDTH / 2;
    headY = HEIGHT / 2;
    nTail = 0;
    score = 0;

    // Seed the random engine
    srand(static_cast<unsigned int>(time(NULL)));
    
    // Spawn initial target item
    fruitX = rand() % (WIDTH - 2) + 1;
    fruitY = rand() % (HEIGHT - 2) + 1;
}

/**
 * @brief Dynamically renders the scene using coordinate matrix translation.
 */
void Draw() {
    // Clear console display
    system("cls");

    // Top Border Panel
    for (int i = 0; i < WIDTH + 2; i++) cout << "#";
    cout << "\n";

    // Dynamic Render Matrix
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x <= WIDTH + 1; x++) {
            // Left Wall
            if (x == 0) {
                cout << "#";
            }
            // Right Wall
            else if (x == WIDTH + 1) {
                cout << "#";
            }
            // Internal coordinate processing
            else {
                int currentX = x - 1; // Map back to board matrix offset
                
                // Draw Snake Head Vector
                if (currentX == headX && y == headY) {
                    cout << "O";
                }
                // Draw Target/Fruit Vector
                else if (currentX == fruitX && y == fruitY) {
                    cout << "F";
                }
                // Draw Snake Tail Segments
                else {
                    bool isTailPrinted = false;
                    for (int k = 0; k < nTail; k++) {
                        if (tailX[k] == currentX && tailY[k] == y) {
                            cout << "o";
                            isTailPrinted = true;
                            break;
                        }
                    }
                    // Empty Track Vector Element
                    if (!isTailPrinted) {
                        cout << " ";
                    }
                }
            }
        }
        cout << "\n";
    }

    // Bottom Border Panel
    for (int i = 0; i < WIDTH + 2; i++) cout << "#";
    cout << "\n\n";

    // Score Dashboard Layout
    cout << "========================================" << "\n";
    cout << "  SCORE PANEL: " << score << " points" << "\n";
    cout << "  CONTROLS   : W (UP) | S (DOWN) | A (LEFT) | D (RIGHT) | X (EXIT)" << "\n";
    cout << "========================================" << "\n";
}

/**
 * @brief Non-blocking input pipeline. Tracks asynchronous keystrokes.
 */
void Input() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'a':
            case 'A':
                if (dir != RIGHT || nTail == 0) dir = LEFT; // Prevent instantly turning back into oneself
                break;
            case 'd':
            case 'D':
                if (dir != LEFT || nTail == 0) dir = RIGHT;
                break;
            case 'w':
            case 'W':
                if (dir != DOWN || nTail == 0) dir = UP;
                break;
            case 's':
            case 'S':
                if (dir != UP || nTail == 0) dir = DOWN;
                break;
            case 'x':
            case 'X':
                gameOver = true;
                break;
        }
    }
}

/**
 * @brief Processes mathematical tracking, state transitions, and collision vectors.
 */
void Logic() {
    // 1. Shift Coordinate Queue for Tail Tracking
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

    // 2. Adjust State Velocity Vector
    switch (dir) {
        case LEFT:  headX--; break;
        case RIGHT: headX++; break;
        case UP:    headY--; break;
        case DOWN:  headY++; break;
        default:             break;
    }

    // 3. Wall Boundary Collision Assertions
    if (headX < 0 || headX >= WIDTH || headY < 0 || headY >= HEIGHT) {
        gameOver = true;
    }

    // 4. Tail Collision Assertions (Self-Cannibalization Check)
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == headX && tailY[i] == headY) {
            gameOver = true;
        }
    }

    // 5. Consumption Vector Resolution
    if (headX == fruitX && headY == fruitY) {
        score += 10;
        if (nTail < MAX_SNAKE_LENGTH) {
            nTail++;
        }
        
        // Relocate Target/Fruit to a clean set of coordinates
        fruitX = rand() % (WIDTH - 2) + 1;
        fruitY = rand() % (HEIGHT - 2) + 1;
    }
}

/**
 * @brief System Entrypoint. Drives initialization, active lifecycle, and replay looping.
 */
int main() {
    char replayChoice;
    
    do {
        Setup();
        
        // Active Frame Execution Loop
        while (!gameOver) {
            Draw();
            Input();
            Logic();
            
            // Adjust frame rate delay based on direction vectors to balance out vertical/horizontal console pacing
            if (dir == UP || dir == DOWN) {
                Sleep(80); // Moving vertically can visually feel faster in terminal fonts, pad slightly
            } else {
                Sleep(50);
            }
        }

        // Display Post-Mortem Screen State
        cout << "\nGAME OVER! Your final score: " << score << "\n";
        cout << "Would you like to play again? (Y/N): ";
        cin >> replayChoice;

    } while (replayChoice == 'y' || replayChoice == 'Y');

    cout << "\nThank you for playing! Exiting application...\n";
    return 0;
}
