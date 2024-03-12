#ifndef FOOD_H
#define FOOD_H

#include <enums.h>

#include <mutex>
#include <random>
#include <thread>

#include "SDL.h"

class Food : public SDL_Point {
 public:
  Food(FoodType type);
  ~Food();
  FoodType GetFoodType() const;
  FoodStatus GetFoodStatus();
  void SetFoodInactive();
  void UpdateGameStatus(GameStatus gameStatus);
  void SetLocationUpdated();
  bool IsLocationUpdated();

 protected:
  std::thread t;

 private:
  FoodStatus status_;
  FoodType type_;
  void ToggleStatus();
  std::random_device dev_;
  std::mt19937 engine_;
  std::uniform_int_distribution<int> random_timeout_;
  std::mutex mutex_;
  GameStatus game_status_;
  bool location_updated_{false};
};

#endif