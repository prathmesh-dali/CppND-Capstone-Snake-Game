#include "game.h"

#include <algorithm>
#include <iostream>

#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(std::make_shared<Snake>(grid_width, grid_height)),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  // Add food of each type and display food of type food at game start
  for (FoodType type : food_type_list) {
    auto food = std::make_shared<Food>(type);
    PlaceFood(food);
    // Set food of type other than food to inactive
    if (type != FoodType::kFood) {
      food->SetFoodInactive();
    }
    food_list.emplace_back(food);
  }
}

Game::~Game() {
  for (auto food : food_list) {
    food->UpdateGameStatus(GameStatus::kClosed);
  }
}

void Game::Run(Controller const &controller, std::shared_ptr<Renderer> renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;

  while (status != GameStatus::kClosed) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(snake, *this);
    Update(renderer);
    renderer->Render(snake, food_list, &wall_enabled_);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      if (status == GameStatus::kRunning) {
        renderer->UpdateWindowTitle(score, frame_count);
      } else if (status == GameStatus::kPaused) {
        renderer->UpdateWindowTitlePaused(score);
      }

      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

bool Game::InFoodList(int x, int y, FoodType type) {
  // Check food location is not preoccupied with existing food
  auto result =
      std::find_if(food_list.begin(), food_list.end(), [x, y, type](auto food) {
        return food->GetFoodType() != type && x == food->x && y == food->y;
      });
  return result != food_list.end();
}

void Game::PlaceFood(std::shared_ptr<Food> food) {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake->SnakeCell(x, y) && !InFoodList(x, y, food->GetFoodType())) {
      food->x = x;
      food->y = y;
      food->SetLocationUpdated();
      return;
    }
  }
}

void Game::Update(std::shared_ptr<Renderer> renderer) {
  if (status != GameStatus::kRunning) {
    return;
  }

  // If snake is not alive update game status to paused
  if (!snake->alive) {
    UpdateStatus(GameStatus::kFinished);
    return;
  }

  snake->Update(&wall_enabled_, &score);

  int new_x = static_cast<int>(snake->head_x);
  int new_y = static_cast<int>(snake->head_y);

  // Check if there's food over here
  for (auto food : food_list) {
    if (food->x == new_x && food->y == new_y &&
        food->GetFoodStatus() == FoodStatus::kActive) {
      switch (food->GetFoodType()) {
        case FoodType::kFood:
          score++;
          PlaceFood(food);
          // Grow snake and increase speed.
          snake->GrowBody();
          snake->speed += 0.02;
          break;

        case FoodType::kBooster:
          PlaceFood(food);
          food->SetFoodInactive();
          // Increase speed of snake for 5 seconds
          snake->BoostSnake();
          break;

        case FoodType::kPoison:
          PlaceFood(food);
          food->SetFoodInactive();
          if (snake->size > 1) {
            // Shrink snake and reduce the speed.
            snake->speed -= 0.02;
            snake->ShrinkBody();
          }
          break;

        case FoodType::kRotten:
          PlaceFood(food);
          food->SetFoodInactive();
          // Make snake go in opposite direction of control
          snake->DizziSnake();
          break;
      }
    } else if (food->GetFoodStatus() == FoodStatus::kInactive &&
               !food->IsLocationUpdated()) {
      // Update food location if food locatio is not updated after inactivity
      PlaceFood(food);
    }
  }
}

void Game::UpdateStatus(GameStatus gameStatus) {
  status = gameStatus;
  // Update food and snake about game status
  for (auto food : food_list) {
    food->UpdateGameStatus(status);
  }
  snake->UpdateGameStatus(status);
}

void Game::ToggleStatus() {
  // Toggle game status between Paused and Running
  UpdateStatus(status == GameStatus::kRunning ? GameStatus::kPaused
                                              : GameStatus::kRunning);
}

void Game::ToggleWall() { wall_enabled_ = !wall_enabled_; }

GameStatus Game::GetStatus() const { return status; }
int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake->size; }