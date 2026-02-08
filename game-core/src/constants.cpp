// constants.cpp
#include "constants.hpp"

// 在C++11中，constexpr静态成员需要在类外定义
constexpr DinoConstants::Sprite DinoConstants::RUN_1;
constexpr DinoConstants::Sprite DinoConstants::RUN_2;
constexpr DinoConstants::Sprite DinoConstants::JUMP;
constexpr DinoConstants::Sprite DinoConstants::DEAD;

constexpr ObstacleConstants::Config ObstacleConstants::SMALL;
constexpr ObstacleConstants::Config ObstacleConstants::BIG;