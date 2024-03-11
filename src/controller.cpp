#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"
#include "game.h"

void Controller::ChangeDirection(std::shared_ptr<Snake> snake, Snake::Direction input,
                                 Snake::Direction opposite, GameStatus gameStatus) const {
  if(gameStatus == GameStatus::kRunning){
    if(snake->GetDizzing()){
      if (snake->direction != input || snake->size == 1) snake->direction = opposite;
    } else {
      if (snake->direction != opposite || snake->size == 1) snake->direction = input;
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
          ChangeDirection(snake, Snake::Direction::kUp,
                          Snake::Direction::kDown, game.GetStatus());
          break;

        case SDLK_DOWN:
          ChangeDirection(snake, Snake::Direction::kDown,
                          Snake::Direction::kUp, game.GetStatus());
          break;

        case SDLK_LEFT:
          ChangeDirection(snake, Snake::Direction::kLeft,
                          Snake::Direction::kRight, game.GetStatus());
          break;

        case SDLK_RIGHT:
          ChangeDirection(snake, Snake::Direction::kRight,
                          Snake::Direction::kLeft, game.GetStatus());
          break;

        case SDLK_ESCAPE:
          game.ToggleStatus();
          break;

        case SDLK_w:
          game.ToggleWall();
          break;
      }
    }
  }
}