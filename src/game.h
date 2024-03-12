#ifndef GAME_H
#define GAME_H

#include <random>
#include <vector>

#include "SDL.h"
#include "controller.h"
#include "enums.h"
#include "food.h"
#include "renderer.h"
#include "snake.h"

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  ~Game();
  void Run(Controller const &controller, std::shared_ptr<Renderer> renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  GameStatus GetStatus() const;
  void UpdateStatus(GameStatus status);
  void ToggleStatus();
  void ToggleWall();

 private:
  std::shared_ptr<Snake> snake;
  std::vector<std::shared_ptr<Food>> food_list;
  std::vector<FoodType> const food_type_list{
      FoodType::kFood, FoodType::kBooster, FoodType::kPoison,
      FoodType::kRotten};

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};
  GameStatus status{GameStatus::kRunning};
  bool InFoodList(int x, int y, FoodType type);
  void PlaceFood(std::shared_ptr<Food>);
  void Update(std::shared_ptr<Renderer> renderer);
  bool wall_enabled_{false};
};

#endif