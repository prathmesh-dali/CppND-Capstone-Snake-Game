#ifndef SNAKE_H
#define SNAKE_H

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "SDL.h"
#include "enums.h"

class Snake {
 public:

  Snake(int grid_width, int grid_height)
      : grid_width(grid_width),
        grid_height(grid_height),
        head_x(grid_width / 2),
        head_y(grid_height / 2) {}

  void Update(bool* wall_enabled, int* score);

  void GrowBody();
  void ShrinkBody();
  bool SnakeCell(int x, int y);

  SnakeDirection direction = SnakeDirection::kUp;

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
  void UpdateHead(bool* wall_enabled, int* score);
  void UpdateBody(SDL_Point& current_cell, SDL_Point& prev_cell, int* score);
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
  GameStatus game_status{false};
  void MarkSnakeDead(int* score);
};

#endif