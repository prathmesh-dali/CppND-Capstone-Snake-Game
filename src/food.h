#ifndef FOOD_H
#define FOOD_H

#include "SDL.h"
#include <random>
#include <thread>
#include <mutex>

enum class FoodType {kFood, kPoison, kBooster, kRotten};
enum class FoodStatus {kActive, kInactive};

class Food: public SDL_Point {
    public:
        Food(FoodType type);
        ~Food();
        FoodType GetFoodType() const;
        FoodStatus GetFoodStatus();
        bool gameRunning{true};
    
    protected:
        std::thread t;

    private:
        FoodStatus status_;
        FoodType type_;
        long statusChangeTimeout_;
        void ToggleStatus();
        std::random_device dev_;
        std::mt19937 engine_;
        std::uniform_int_distribution<int> randomTimeout_;
        std::mutex mutex_;
};

#endif