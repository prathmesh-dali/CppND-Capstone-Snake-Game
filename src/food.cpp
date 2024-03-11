#include "food.h"
#include <chrono>

Food::Food(FoodType type): type_(type), engine_(dev_()), randomTimeout_(2000, 15000), status_(FoodStatus::kActive){
    if(type_ != FoodType::kFood){
        t = std::thread(&Food::ToggleStatus, this);
    }
};

Food::~Food(){
    if(type_ != FoodType::kFood){
        t.join();
    }
}

void Food::ToggleStatus(){
    auto startTime = std::chrono::high_resolution_clock::now();
    statusChangeTimeout_ = randomTimeout_(engine_);
    while(gameRunning){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timeDiff = currentTime - startTime;
        if(statusChangeTimeout_ < std::chrono::duration_cast<std::chrono::milliseconds>(timeDiff).count()){
            std::unique_lock<std::mutex> uLock(mutex_);

            status_ = status_ == FoodStatus::kActive? FoodStatus::kInactive : FoodStatus::kActive;

            uLock.unlock();
            statusChangeTimeout_ = randomTimeout_(engine_);
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