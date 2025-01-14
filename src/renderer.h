#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include "SDL.h"
#include "food.h"
#include "snake.h"

class Renderer {
 public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
           const std::size_t grid_width, const std::size_t grid_height);
  ~Renderer();

  void Render(std::shared_ptr<Snake> const snake,
              std::vector<std::shared_ptr<Food>> const food_list,
              bool *wall_enabled);
  void UpdateWindowTitle(int score, int fps);
  void UpdateWindowTitlePaused(int score);

 private:
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_width;
  const std::size_t grid_height;
};

#endif