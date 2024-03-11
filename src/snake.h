#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include "SDL.h"
#include "enums.h"

class Snake {
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };

  Snake(int grid_width, int grid_height)
      : grid_width(grid_width),
        grid_height(grid_height),
        head_x(grid_width / 2),
        head_y(grid_height / 2) {}

  void Update();

  void GrowBody();
  void ShrinkBody();
  bool SnakeCell(int x, int y);

  Direction direction = Direction::kUp;

  float speed{0.1f};
  int size{1};
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;
  bool GetBoosting();
  void BoostSnake();
  void DizziSnake();
  bool GetDizzing();
  void UpdateGameStatus(GameStatus gameStatus);
  
 private:
  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell);
  bool growing{false};
  bool shrinking{false};
  bool dizzing{false};
  bool boosting{false};
  std::condition_variable booster_cond;
  std::condition_variable dizzi_cond;
  int grid_width;
  int grid_height;
  std::mutex mutex;
  std::chrono::time_point<std::chrono::high_resolution_clock> gamePausedTime;
  GameStatus gameStatus{false};
};

#endif