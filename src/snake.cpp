#include "snake.h"

#include <cmath>
#include <iostream>

void Snake::Update(bool* wall_eanbled, int* score) {
  SDL_Point prev_cell{
      static_cast<int>(head_x),
      static_cast<int>(
          head_y)};  // We first capture the head's cell before updating.
  UpdateHead(wall_eanbled, score);
  SDL_Point current_cell{
      static_cast<int>(head_x),
      static_cast<int>(head_y)};  // Capture the head's cell after updating.

  // Update all of the body vector items if the snake head has moved to a new
  // cell.
  if (current_cell.x != prev_cell.x || current_cell.y != prev_cell.y) {
    UpdateBody(current_cell, prev_cell, score);
  }
}

void Snake::UpdateHead(bool* wall_enabled, int* score) {
  switch (direction) {
    case SnakeDirection::kUp:
      head_y -= GetSpeed();
      break;

    case SnakeDirection::kDown:
      head_y += GetSpeed();
      break;

    case SnakeDirection::kLeft:
      head_x -= GetSpeed();
      break;

    case SnakeDirection::kRight:
      head_x += GetSpeed();
      break;
  }
  // If wall is enabled and snake hit the wall kill the snake.
  if (*wall_enabled) {
    if (head_x > grid_width || head_x < 0 || head_y > grid_height ||
        head_y < 0) {
      MarkSnakeDead(score);
    }
  }

  // Wrap the Snake around to the beginning if going off of the screen.
  head_x = fmod(head_x + grid_width, grid_width);
  head_y = fmod(head_y + grid_height, grid_height);
}

float Snake::GetSpeed() { return speed + GetBoosting() * 0.1; }

void Snake::MarkSnakeDead(int* score) {
  // Mark snake dead and notify all timer thread about snake is dead
  alive = false;
  booster_cond.notify_all();
  dizzi_cond.notify_all();
  // Dispalay popup message with score and size with game over message.
  std::string msg{"Score: " + std::to_string(*score) +
                  "\n Size: " + std::to_string(size)};
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Over!",
                           msg.c_str(), NULL);
  // On popup close trigger window close event to close the game
  SDL_Event ev;
  ev.type = SDL_QUIT;
  SDL_PushEvent(&ev);
}

void Snake::UpdateBody(SDL_Point& current_head_cell, SDL_Point& prev_head_cell,
                       int* score) {
  // Add previous head location to vector
  body.push_back(prev_head_cell);

  if (!growing) {
    // Remove the tail from the vector.
    body.erase(body.begin());
  } else {
    growing = false;
    size++;
  }
  if (shrinking && size > 1) {
    shrinking = false;
    size--;
    body.erase(body.begin());
  }

  // Check if the snake has died.
  for (auto const& item : body) {
    if (current_head_cell.x == item.x && current_head_cell.y == item.y) {
      MarkSnakeDead(score);
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
  for (auto const& item : body) {
    if (x == item.x && y == item.y) {
      return true;
    }
  }
  return false;
}

// Remove the snake boosting after 5 seconds
void Snake::BoostSnake() {
  std::thread([this]() {
    if (alive) {
      std::unique_lock<std::mutex> uLock(mutex);
      if (boosting) {
        booster_cond.wait(uLock);
      }
      if (alive) {
        boosting = true;
        uLock.unlock();
        ManageThreadSleep();
        uLock.lock();
        boosting = false;
        uLock.unlock();
        booster_cond.notify_one();
      }
    }
  }).detach();
}

// Remove snake dizzing status after 5 seconds
void Snake::DizziSnake() {
  std::thread([this]() {
    if (alive) {
      std::unique_lock<std::mutex> uLock(mutex);
      if (dizzing) {
        dizzi_cond.wait(uLock);
      }
      if (alive) {
        dizzing = true;
        uLock.unlock();
        ManageThreadSleep();
        uLock.lock();
        dizzing = false;
        uLock.unlock();
        dizzi_cond.notify_one();
      }
    }
  }).detach();
}

// check for timeout and pause timer if game is paused
void Snake::ManageThreadSleep() {
  auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();
  auto timeDiff = currentTime - startTime;
  while (
      std::chrono::duration_cast<std::chrono::milliseconds>(timeDiff).count() <=
      5000) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    currentTime = std::chrono::high_resolution_clock::now();
    if (game_status == GameStatus::kPaused) {
      auto gamePausedTime = std::chrono::high_resolution_clock::now();
      startTime = currentTime - (startTime - gamePausedTime);
    }
    timeDiff = currentTime - startTime;
  }
}

bool Snake::GetBoosting() {
  std::lock_guard<std::mutex> lock(mutex);
  return boosting;
}

bool Snake::GetDizzing() {
  std::lock_guard<std::mutex> lock(mutex);
  return dizzing;
}

void Snake::UpdateGameStatus(GameStatus gameStatus) {
  std::lock_guard<std::mutex> lock(mutex);
  this->game_status = gameStatus;
}