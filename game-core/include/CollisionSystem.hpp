#ifndef COLLISIONSYSTEM_HPP
#define COLLISIONSYSTEM_HPP

struct Rect {
    float x, y;
    float width, height;
};

struct CollisionResult {
    bool collided;
    const char* obstacleType;
};

class CollisionSystem {
public:
    CollisionResult checkCollision(const class Dino& dino, const class ObstacleManager& obstacleManager);
    
private:
    bool rectIntersect(const Rect& rect1, const Rect& rect2);
};

#endif // COLLISIONSYSTEM_HPP