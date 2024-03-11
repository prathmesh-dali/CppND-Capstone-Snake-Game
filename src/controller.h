#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"

class Game;

class Controller {
 public:
  void HandleInput(std::shared_ptr<Snake> snake, Game &game) const;

 private:
  void ChangeDirection(std::shared_ptr<Snake> snake, Snake::Direction input,
                       Snake::Direction opposite, GameStatus gameStatus) const;
};

#endif