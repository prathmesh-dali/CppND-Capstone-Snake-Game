#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "enums.h"
#include "snake.h"

class Game;

class Controller {
 public:
  void HandleInput(std::shared_ptr<Snake> snake, Game &game) const;

 private:
  void ChangeDirection(std::shared_ptr<Snake> snake, SnakeDirection input,
                       SnakeDirection opposite, GameStatus gameStatus) const;
};

#endif