#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int GRID_SIZE = 20;
const SDL_Color HEAD_COLOR = { 0, 255, 0, 255 };

struct Point {
    int x;
    int y;
};

enum class Direction { UP, DOWN, LEFT, RIGHT };

void CheckBorderCrossing(Point& head) {
    if (head.x < 0) {
        head.x = (SCREEN_WIDTH / GRID_SIZE) - 1;
    }
    else if (head.x >= (SCREEN_WIDTH / GRID_SIZE)) {
        head.x = 0;
    }

    if (head.y < 0) {
        head.y = (SCREEN_HEIGHT / GRID_SIZE) - 1;
    }
    else if (head.y >= (SCREEN_HEIGHT / GRID_SIZE)) {
        head.y = 0;
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL initialization failed: " << SDL_GetError() << endl;
        return 1;
    }

    if (TTF_Init() < 0) {
        cerr << "SDL_ttf initialization failed: " << TTF_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "Window creation failed: " << SDL_GetError() << endl;
        TTF_Quit();
        SDL_Quit();
        return 2;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cerr << "Renderer creation failed: " << SDL_GetError() << endl;
        TTF_Quit();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 3;
    }

    TTF_Font* font = TTF_OpenFont("your_font.ttf", 24);
    if (!font) {
        cerr << "Font could not be loaded! TTF_Error: " << TTF_GetError() << endl;
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 4;
    }

    srand(static_cast<unsigned>(time(nullptr)));

    vector<Point> snake;
    Direction snakeDirection = Direction::RIGHT;
    Point food;
    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE);
    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE);

    for (int i = 0; i < 4; ++i) {
        Point p;
        p.x = (SCREEN_WIDTH / GRID_SIZE) / 2 - i;
        p.y = SCREEN_HEIGHT / GRID_SIZE / 2;
        snake.push_back(p);
    }

    int score = 0;
    bool quit = false;
    bool gameOver = false;
    bool gameStarted = false;

    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (!gameOver) {
                    if (!gameStarted) {
                        if (event.key.keysym.sym == SDLK_RETURN) {
                            gameStarted = true;
                        }
                    }
                    else {
                        switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            if (snakeDirection != Direction::DOWN) snakeDirection = Direction::UP;
                            break;
                        case SDLK_DOWN:
                            if (snakeDirection != Direction::UP) snakeDirection = Direction::DOWN;
                            break;
                        case SDLK_LEFT:
                            if (snakeDirection != Direction::RIGHT) snakeDirection = Direction::LEFT;
                            break;
                        case SDLK_RIGHT:
                            if (snakeDirection != Direction::LEFT) snakeDirection = Direction::RIGHT;
                            break;
                        case SDLK_ESCAPE:
                            quit = true;
                            break;
                        }
                    }
                }
                else {
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        snake.clear();
                        for (int i = 0; i < 4; ++i) {
                            Point p;
                            p.x = (SCREEN_WIDTH / GRID_SIZE) / 2 - i;
                            p.y = SCREEN_HEIGHT / GRID_SIZE / 2;
                            snake.push_back(p);
                        }
                        snakeDirection = Direction::RIGHT;
                        score = 0;
                        gameOver = false;
                        gameStarted = false;
                        food.x = rand() % (SCREEN_WIDTH / GRID_SIZE);
                        food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE);
                    }
                }
            }
        }

        if (gameStarted && !gameOver) {
            Point newHead = snake[0];
            switch (snakeDirection) {
            case Direction::UP:
                newHead.y -= 1;
                break;
            case Direction::DOWN:
                newHead.y += 1;
                break;
            case Direction::LEFT:
                newHead.x -= 1;
                break;
            case Direction::RIGHT:
                newHead.x += 1;
                break;
            }

            CheckBorderCrossing(newHead);

            for (size_t i = 1; i < snake.size(); ++i) {
                if (newHead.x == snake[i].x && newHead.y == snake[i].y) {
                    cerr << "Snake collision!" << endl;
                    gameOver = true;
                    break;
                }
            }

            if (!gameOver) {
                snake.insert(snake.begin(), newHead);

                if (newHead.x == food.x && newHead.y == food.y) {
                    food.x = rand() % (SCREEN_WIDTH / GRID_SIZE);
                    food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE);
                    score += 10;
                }
                else {
                    snake.pop_back();
                }

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_Rect foodRect = { food.x * GRID_SIZE, food.y * GRID_SIZE, GRID_SIZE, GRID_SIZE };
                SDL_RenderFillRect(renderer, &foodRect);

                for (size_t i = 0; i < snake.size(); ++i) {
                    SDL_Rect snakeRect = { snake[i].x * GRID_SIZE, snake[i].y * GRID_SIZE, GRID_SIZE, GRID_SIZE };

                    if (i == 0) {
                        SDL_SetRenderDrawColor(renderer, HEAD_COLOR.r, HEAD_COLOR.g, HEAD_COLOR.b, HEAD_COLOR.a);
                    }
                    else {
                        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    }

                    SDL_RenderFillRect(renderer, &snakeRect);
                }

                SDL_Color textColor = { 0, 255, 0, 255 };
                string scoreText = "Score: " + to_string(score);
                SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_Rect textRect = { 10, 10, textSurface->w, textSurface->h };
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);

                SDL_RenderPresent(renderer);
                SDL_Delay(100);
            }
        }
        else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            if (gameOver) {
                SDL_Color textColor = { 255, 0, 0, 255 };
                string gameOverText = "Game Over! Score: " + to_string(score);
                SDL_Surface* textSurface = TTF_RenderText_Solid(font, gameOverText.c_str(), textColor);
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_Rect textRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, textSurface->w, textSurface->h };
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_FreeSurface(textSurface);
                SDL_DestroyTexture(textTexture);

                SDL_Color exitTextColor = { 0, 255, 0, 255 };
                string exitText = "Press Enter to Play Again or ESC to Exit";
                SDL_Surface* exitTextSurface = TTF_RenderText_Solid(font, exitText.c_str(), exitTextColor);
                SDL_Texture* exitTextTexture = SDL_CreateTextureFromSurface(renderer, exitTextSurface);
                SDL_Rect exitTextRect = { SCREEN_WIDTH / 4, (SCREEN_HEIGHT / 2) + 50, exitTextSurface->w, exitTextSurface->h };
                SDL_RenderCopy(renderer, exitTextTexture, nullptr, &exitTextRect);
                SDL_FreeSurface(exitTextSurface);
                SDL_DestroyTexture(exitTextTexture);

                SDL_RenderPresent(renderer);
            }
            else {
                SDL_Color startTextColor = { 0, 255, 0, 255 };
                string startText = "Press Enter to Start";
                SDL_Surface* startTextSurface = TTF_RenderText_Solid(font, startText.c_str(), startTextColor);
                SDL_Texture* startTextTexture = SDL_CreateTextureFromSurface(renderer, startTextSurface);
                SDL_Rect startTextRect = { SCREEN_WIDTH / 3, (SCREEN_HEIGHT / 2) - 25, startTextSurface->w, startTextSurface->h };
                SDL_RenderCopy(renderer, startTextTexture, nullptr, &startTextRect);
                SDL_FreeSurface(startTextSurface);
                SDL_DestroyTexture(startTextTexture);

                SDL_RenderPresent(renderer);
            }

            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    quit = true;
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        if (gameOver) {
                            snake.clear();
                            for (int i = 0; i < 4; ++i) {
                                Point p;
                                p.x = (SCREEN_WIDTH / GRID_SIZE) / 2 - i;
                                p.y = SCREEN_HEIGHT / GRID_SIZE / 2;
                                snake.push_back(p);
                            }
                            snakeDirection = Direction::RIGHT;
                            score = 0;
                            gameOver = false;
                            gameStarted = false;
                            food.x = rand() % (SCREEN_WIDTH / GRID_SIZE);
                            food.y = rand() % (SCREEN_HEIGHT / GRID_SIZE);
                        }
                        else {
                            gameStarted = true;
                        }
                    }
                    else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        if (!gameStarted) {
                            quit = true;
                        }
                    }
                }
            }
        }
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
