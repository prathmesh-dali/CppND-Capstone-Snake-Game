#include "food.h"
#include <chrono>

Food::Food(FoodType type): type_(type), engine_(dev_()), random_timeout_(2000, 15000), status_(FoodStatus::kActive), game_status_(GameStatus::kRunning){
    if(type_ != FoodType::kFood){
        t = std::thread(&Food::ToggleStatus, this);
    }
};

Food::~Food(){
    if(type_ != FoodType::kFood){
        t.join();
    }
}

void Food::UpdateGameStatus(GameStatus gameStatus){
    game_status_ = gameStatus;
}

void Food::ToggleStatus(){
    auto startTime = std::chrono::high_resolution_clock::now();
    int statusChangeTimeout = random_timeout_(engine_);
    while(game_status_ !=GameStatus::kClosed){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        if(game_status_ == GameStatus::kPaused){
            auto gamePausedTime = std::chrono::high_resolution_clock::now();
            auto gameExecTime = startTime - gamePausedTime;
            startTime = currentTime - gameExecTime;
        }
        auto timeDiff = currentTime - startTime;
        if(statusChangeTimeout < std::chrono::duration_cast<std::chrono::milliseconds>(timeDiff).count()){
            std::unique_lock<std::mutex> uLock(mutex_);

            status_ = status_ == FoodStatus::kActive? FoodStatus::kInactive : FoodStatus::kActive;

            uLock.unlock();
            statusChangeTimeout = random_timeout_(engine_);
            startTime = std::chrono::high_resolution_clock::now();
        }
    }
}

FoodStatus Food::GetFoodStatus(){
    std::lock_guard<std::mutex> lock(mutex_);
    return status_;
}

FoodType Food::GetFoodType() const{
    return type_;
}