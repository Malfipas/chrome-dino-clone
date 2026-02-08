#ifndef OBSTACLEMANAGER_HPP
#define OBSTACLEMANAGER_HPP

#include <vector>
#include <string>

struct Obstacle {
    std::string type; // "small" or "big"
    float x, y;
    int width, height;
    int spriteX, spriteY;
    
    struct BoundingBox {
        float x, y;
        float width, height;
    };
    
    BoundingBox boundingBox() const;
};

class ObstacleManager {
public:
    ObstacleManager();
    void reset();
    void update(float deltaTime, float gameSpeed);
    
    const std::vector<Obstacle>& getObstacles() const;

private:
    void spawnObstacle();
    float getRandomSpawnTime();
    float computeNextSpawnTime(float gameSpeed);
    
    std::vector<Obstacle> obstacles;
    float spawnTimer;
    float nextSpawnTime;
};

#endif // OBSTACLEMANAGER_HPP