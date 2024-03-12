#include "food.h"

#include <chrono>

Food::Food(FoodType type)
    : type_(type),
      engine_(dev_()),
      random_timeout_(2000, 15000),
      status_(FoodStatus::kActive),
      game_status_(GameStatus::kRunning) {
  // Spawn food status toggle thread based on food type. If normal food don't
  // don't need toggle
  if (type_ != FoodType::kFood) {
    t = std::thread(&Food::ToggleStatus, this);
  }
};

Food::~Food() {
  if (type_ != FoodType::kFood) {
    t.join();
  }
}

void Food::UpdateGameStatus(GameStatus gameStatus) {
  game_status_ = gameStatus;
}

void Food::ToggleStatus() {
  auto startTime = std::chrono::high_resolution_clock::now();
  int statusChangeTimeout = random_timeout_(engine_);
  // if game is not finished or closed check for food toggle timeout and toggle
  // food
  while (game_status_ != GameStatus::kClosed &&
         game_status_ != GameStatus::kFinished) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    auto currentTime = std::chrono::high_resolution_clock::now();
    // on game pause pause the toggle timer
    if (game_status_ == GameStatus::kPaused) {
      auto gamePausedTime = std::chrono::high_resolution_clock::now();
      startTime = currentTime - (startTime - gamePausedTime);
    }
    auto timeDiff = currentTime - startTime;
    // on timeout change the status of the food and reset the timer
    if (statusChangeTimeout <
        std::chrono::duration_cast<std::chrono::milliseconds>(timeDiff)
            .count()) {
      std::unique_lock<std::mutex> uLock(mutex_);

      status_ = status_ == FoodStatus::kActive ? FoodStatus::kInactive
                                               : FoodStatus::kActive;
      location_updated_ = false;

      uLock.unlock();
      statusChangeTimeout = random_timeout_(engine_);
      startTime = std::chrono::high_resolution_clock::now();
    }
  }
}

FoodStatus Food::GetFoodStatus() {
  std::lock_guard<std::mutex> lock(mutex_);
  return status_;
}

void Food::SetFoodInactive() {
  std::lock_guard<std::mutex> lock(mutex_);
  status_ = FoodStatus::kInactive;
}

void Food::SetLocationUpdated() {
  std::lock_guard<std::mutex> lock(mutex_);
  location_updated_ = true;
}

bool Food::IsLocationUpdated() {
  std::lock_guard<std::mutex> lock(mutex_);
  return location_updated_;
}

FoodType Food::GetFoodType() const { return type_; }