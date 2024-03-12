#include "controller.h"

#include <iostream>

#include "SDL.h"
#include "game.h"
#include "snake.h"

void Controller::ChangeDirection(std::shared_ptr<Snake> snake,
                                 SnakeDirection input, SnakeDirection opposite,
                                 GameStatus gameStatus) const {
  if (gameStatus == GameStatus::kRunning) {
    if (snake->GetDizzing()) {
      if (snake->direction != input || snake->size == 1)
        snake->direction = opposite;
    } else {
      if (snake->direction != opposite || snake->size == 1)
        snake->direction = input;
    }
  }
  return;
}

void Controller::HandleInput(std::shared_ptr<Snake> snake, Game &game) const {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      game.UpdateStatus(GameStatus::kClosed);
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_UP:
          ChangeDirection(snake, SnakeDirection::kUp, SnakeDirection::kDown,
                          game.GetStatus());
          break;

        case SDLK_DOWN:
          ChangeDirection(snake, SnakeDirection::kDown, SnakeDirection::kUp,
                          game.GetStatus());
          break;

        case SDLK_LEFT:
          ChangeDirection(snake, SnakeDirection::kLeft, SnakeDirection::kRight,
                          game.GetStatus());
          break;

        case SDLK_RIGHT:
          ChangeDirection(snake, SnakeDirection::kRight, SnakeDirection::kLeft,
                          game.GetStatus());
          break;

        case SDLK_ESCAPE:
          game.ToggleStatus();
          break;

        case SDLK_w:
          if(game.GetStatus() == GameStatus::kRunning){
            game.ToggleWall();
          }
          break;
      }
    }
  }
}