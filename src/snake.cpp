#include "snake.h"
#include <cmath>
#include <iostream>

void Snake::Update() {
  SDL_Point prev_cell{
      static_cast<int>(head_x),
      static_cast<int>(
          head_y)};  // We first capture the head's cell before updating.
  UpdateHead();
  SDL_Point current_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)};  // Capture the head's cell after updating.

  // Update all of the body vector items if the snake head has moved to a new
  // cell.
  if (current_cell.x != prev_cell.x || current_cell.y != prev_cell.y) {
    UpdateBody(current_cell, prev_cell);
  }
}

void Snake::UpdateHead() {
  switch (direction) {
    case Direction::kUp:
      head_y -= (speed + GetBoosting() * 0.1);
      break;

    case Direction::kDown:
      head_y += (speed + GetBoosting() * 0.1);
      break;

    case Direction::kLeft:
      head_x -= (speed + GetBoosting() * 0.1);
      break;

    case Direction::kRight:
      head_x += (speed + GetBoosting() * 0.1);
      break;
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  head_x = fmod(head_x + grid_width, grid_width);
  head_y = fmod(head_y + grid_height, grid_height);
}

void Snake::UpdateBody(SDL_Point &current_head_cell, SDL_Point &prev_head_cell) {
  // Add previous head location to vector
  body.push_back(prev_head_cell);

  if (!growing) {
    // Remove the tail from the vector.
    body.erase(body.begin());
  } else {
    growing = false;
    size++;
  }
  if(shrinking && size > 1){
    shrinking = false;
    size--;
    body.erase(body.begin());
  }

  // Check if the snake has died.
  for (auto const &item : body) {
    if (current_head_cell.x == item.x && current_head_cell.y == item.y) {
      alive = false;
      booster_cond.notify_all();
      dizzi_cond.notify_all();
    }
  }
}

void Snake::GrowBody() { growing = true; }
void Snake::ShrinkBody() { shrinking = true; }

// Inefficient method to check if cell is occupied by snake.
bool Snake::SnakeCell(int x, int y) {
  if (x == static_cast<int>(head_x) && y == static_cast<int>(head_y)) {
    return true;
  }
  for (auto const &item : body) {
    if (x == item.x && y == item.y) {
      return true;
    }
  }
  return false;
}


void Snake::BoostSnake(){
  std::thread([this](){
    if(alive){
      std::unique_lock<std::mutex> uLock(mutex);
      if(boosting){
        booster_cond.wait(uLock);
      }
      if(alive){
        boosting = true;
        uLock.unlock();
        auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timeDiff = currentTime - startTime;
        while(std::chrono::duration_cast<std::chrono::milliseconds>(timeDiff).count()<=5000){
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
          currentTime = std::chrono::high_resolution_clock::now();
          if(gameStatus == GameStatus::kPaused){
            auto gamePausedTime = std::chrono::high_resolution_clock::now();
            auto gameExecTime = startTime - gamePausedTime;
            startTime = currentTime - gameExecTime;
          } 
          timeDiff = currentTime - startTime;
        }
        uLock.lock();
        boosting =false;
        uLock.unlock();
        booster_cond.notify_one();
      }
    }
  }).detach();
}

void Snake::DizziSnake(){
  std::thread([this](){
    if(alive){
      std::unique_lock<std::mutex> uLock(mutex);
      if(dizzing){
        dizzi_cond.wait(uLock);
      }
      if(alive){
        dizzing = true;
        uLock.unlock();
        auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timeDiff = currentTime - startTime;
        while(std::chrono::duration_cast<std::chrono::milliseconds>(timeDiff).count()<=5000){
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
          currentTime = std::chrono::high_resolution_clock::now();
          if(gameStatus == GameStatus::kPaused){
            auto gamePausedTime = std::chrono::high_resolution_clock::now();
            auto gameExecTime = startTime - gamePausedTime;
            startTime = currentTime - gameExecTime;
          } 
          timeDiff = currentTime - startTime;
        }
        uLock.lock();
        dizzing =false;
        uLock.unlock();
        dizzi_cond.notify_one();
      }
    }
  }).detach();
}


bool Snake::GetBoosting(){
  std::lock_guard<std::mutex> lock(mutex);
  return boosting;
}

bool Snake::GetDizzing(){
  std::lock_guard<std::mutex> lock(mutex);
  return dizzing;
}

void Snake::UpdateGameStatus(GameStatus gameStatus){
  std::lock_guard<std::mutex> lock(mutex);
  this->gameStatus = gameStatus;
  this->gamePausedTime = std::chrono::high_resolution_clock::now();
}