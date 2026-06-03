#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

const int WIDTH = 20;
const int HEIGHT = 20;

struct Point {
    int x, y;
};

void drawBoard(const vector<Point>& snake, const Point& food, int score) {
    system(
    #ifdef _WIN32
        "cls"
    #else
        "clear"
    #endif
    );

    for (int i = 0; i < WIDTH + 2; i++) cout << "#";
    cout << "\n";

    for (int y = 0; y < HEIGHT; y++) {
        cout << "#";
        for (int x = 0; x < WIDTH; x++) {
            bool printed = false;

            if (x == food.x && y == food.y) {
                cout << "F";
                printed = true;
            }

            for (size_t i = 0; i < snake.size(); i++) {
                if (snake[i].x == x && snake[i].y == y) {
                    cout << (i == 0 ? "O" : "o");
                    printed = true;
                    break;
                }
            }

            if (!printed) cout << " ";
        }
        cout << "#\n";
    }

    for (int i = 0; i < WIDTH + 2; i++) cout << "#";
    cout << "\nScore: " << score << "\n";
}

bool isCollision(const vector<Point>& snake) {
    Point head = snake[0];

    if (head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT)
        return true;

    for (size_t i = 1; i < snake.size(); i++) {
        if (head.x == snake[i].x && head.y == snake[i].y)
            return true;
    }

    return false;
}

int main() {
    srand((unsigned)time(0));

    char replay;

    do {
        vector<Point> snake = {{WIDTH / 2, HEIGHT / 2}};
        Point food = {rand() % WIDTH, rand() % HEIGHT};

        char direction = 'd';
        int score = 0;
        bool gameOver = false;

        while (!gameOver) {
            drawBoard(snake, food, score);

            cout << "Move (W/A/S/D): ";
            char input;
            cin >> input;

            input = tolower(input);

            if (input == 'w' || input == 'a' || input == 's' || input == 'd')
                direction = input;

            Point newHead = snake[0];

            if (direction == 'w') newHead.y--;
            else if (direction == 's') newHead.y++;
            else if (direction == 'a') newHead.x--;
            else if (direction == 'd') newHead.x++;

            snake.insert(snake.begin(), newHead);

            if (newHead.x == food.x && newHead.y == food.y) {
                score += 10;
                food = {rand() % WIDTH, rand() % HEIGHT};
            } else {
                snake.pop_back();
            }

            if (isCollision(snake))
                gameOver = true;
        }

        cout << "\nGame Over!\nFinal Score: " << score << "\n";
        cout << "Play Again? (y/n): ";
        cin >> replay;

    } while (tolower(replay) == 'y');

    return 0;
}
