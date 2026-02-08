#include "CollisionSystem.hpp"
#include "Dino.hpp"
#include "ObstacleManager.hpp"

CollisionResult CollisionSystem::checkCollision(const Dino& dino, const ObstacleManager& obstacleManager) {
    Rect dinoBox = {
        dino.getBoundingBox().x,
        dino.getBoundingBox().y,
        dino.getBoundingBox().width,
        dino.getBoundingBox().height
    };
    
    for (const auto& obstacle : obstacleManager.getObstacles()) {
        auto obsBox = obstacle.boundingBox();
        Rect obsRect = {obsBox.x, obsBox.y, obsBox.width, obsBox.height};
        
        if (rectIntersect(dinoBox, obsRect)) {
            return {true, obstacle.type.c_str()};
        }
    }
    
    return {false, ""};
}

bool CollisionSystem::rectIntersect(const Rect& rect1, const Rect& rect2) {
    return (
        rect1.x < rect2.x + rect2.width &&
        rect1.x + rect1.width > rect2.x &&
        rect1.y < rect2.y + rect2.height &&
        rect1.y + rect1.height > rect2.y
    );
}