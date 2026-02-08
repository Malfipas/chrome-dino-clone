#include "ObstacleManager.hpp"
#include "constants.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>

ObstacleManager::ObstacleManager() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    reset();
}

void ObstacleManager::reset() {
    obstacles.clear();
    spawnTimer = 0;
    // 增加初始生成延迟：基于初始速度计算间距并再额外延迟，避免一开始太快
    nextSpawnTime = computeNextSpawnTime(INITIAL_GAME_SPEED) + 800.0f;
}

void ObstacleManager::update(float deltaTime, float gameSpeed) {
    // 移动现有障碍物（按帧数缩放，deltaTime 为毫秒）
    for (auto it = obstacles.begin(); it != obstacles.end();) {
        float frames = deltaTime / 16.67f;
        it->x -= gameSpeed * frames; // gameSpeed 以每帧像素为基准
        
        // 移除屏幕外的障碍物
        if (it->x + it->width < -50) {
            it = obstacles.erase(it);
        } else {
            ++it;
        }
    }
    
    // 生成新障碍物
    spawnTimer += deltaTime; // deltaTime 为毫秒，nextSpawnTime 也是毫秒

    if (spawnTimer >= nextSpawnTime) {
        spawnObstacle();
        spawnTimer = 0.0f;
        // 根据当前速度计算下一个生成时间（基于像素间距转换为毫秒）
        nextSpawnTime = computeNextSpawnTime(gameSpeed);
    }
    // if (spawnTimer >= nextSpawnTime/ 1000.0f) {
    //     spawnObstacle();
    //     spawnTimer = 0;
    //     nextSpawnTime = getRandomSpawnTime();
    // }
}

void ObstacleManager::spawnObstacle() {
    // 避免同时存在太多障碍物
    if (obstacles.size() >= 3) {  // 降低最大障碍物数量
        return;
    }

    // 检查屏幕上的障碍物距离，避免太密集
    if (!obstacles.empty()) {
        // 找到最近的一个障碍物
        float nearestX = CANVAS_WIDTH * 2.0f; // 初始设为屏幕外很远
        for (const auto& obs : obstacles) {
            if (obs.x < nearestX && obs.x > -obs.width) {
                nearestX = obs.x;
            }
        }
        
        // 如果最近的障碍物还在屏幕右半部分，不生成新障碍物
        if (nearestX > CANVAS_WIDTH * 0.5f) {
            return;
        }
    }

    const char* types[] = {"small", "big"};
    const char* type = types[std::rand() % 2];
    
    ObstacleConstants::Config config;
    if (std::string(type) == "small") {
        config = ObstacleConstants::SMALL;
    } else {
        config = ObstacleConstants::BIG;
    }
    
    int count = std::rand() % 2 + 1;
    float obstacleY = GROUND_Y - config.HEIGHT;

    // 检查新障碍物是否与现有障碍物太近
    if (!obstacles.empty()) {
        // 找到屏幕上最右侧的障碍物
        float rightmostX = -9999;
        for (const auto& obs : obstacles) {
            if (obs.x > rightmostX) {
                rightmostX = obs.x;
            }
        }
        
        // 确保新障碍物与最右侧障碍物有足够距离
        // 至少间隔屏幕宽度的1/3
        if (rightmostX > CANVAS_WIDTH * 0.66f) {
            return;
        }
    }
    
    for (int i = 0; i < count; i++) {
        Obstacle obstacle;
        obstacle.type = type;
        obstacle.x = CANVAS_WIDTH + i * config.WIDTH;
        obstacle.y = obstacleY;
        obstacle.width = config.WIDTH;
        obstacle.height = config.HEIGHT;
        obstacle.spriteX = config.SPRITE_X + (std::rand() % 2) * (std::string(type) == "small" ? 102 : 150);
        obstacle.spriteY = config.SPRITE_Y;
        
        obstacles.push_back(obstacle);
    }
}

float ObstacleManager::getRandomSpawnTime() {
    // 返回毫秒单位的随机生成时间
    // 增加最小生成间隔，减少生成频率
    // 使用常量范围（毫秒）作为生成间隔，恢复较短的最小间隔以匹配初始速度
    // 适当加长最小生成间隔，避免障碍生成过密
    int minSpawn = OBSTACLE_SPAWN_RANGE_MIN + 800; // 增加 800ms 最小间隔
    int maxSpawn = OBSTACLE_SPAWN_RANGE_MAX;
    
    //return static_cast<float>(std::rand() % (maxSpawn - minSpawn) + minSpawn);

    if (minSpawn >= maxSpawn) {
        minSpawn = maxSpawn - 500;
    }
    
    int range = maxSpawn - minSpawn;
    if (range <= 0) range = 1000;
    
    return static_cast<float>(minSpawn + (std::rand() % range));
}

float ObstacleManager::computeNextSpawnTime(float gameSpeed) {
    // gameSpeed currently is in pixels-per-frame.
    // Convert to pixels-per-ms: px_per_ms = gameSpeed / 16.67
    float pxPerMs = (gameSpeed > 0.0f) ? (gameSpeed / 16.67f) : (INITIAL_GAME_SPEED / 16.67f);

    // Desired gap in pixels increases with speed to avoid visual crowding at high speed.
    const float baseGap = 900.0f; // 基准像素距离
    const float gapPerSpeed = 30.0f; // 每单位速度增加的像素距离
    const float minGap = 600.0f;
    const float maxGap = 2000.0f;

    float desiredGap = baseGap + gapPerSpeed * (gameSpeed - INITIAL_GAME_SPEED);
    if (desiredGap < minGap) desiredGap = minGap;
    if (desiredGap > maxGap) desiredGap = maxGap;

    // Add a random jitter (±30%) to avoid perfect regularity
    // jitter range: [0.7, 1.3]
    float jitter = 0.7f + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 0.6f;
    desiredGap *= jitter;

    // Convert desired pixel gap into milliseconds: ms = desiredGap / pxPerMs
    float nextMs = desiredGap / pxPerMs;

    // Clamp to reasonable ms bounds to avoid too rare or too frequent spawns
    const float minMs = 1800.0f; // 最小 1800ms（用户要求）
    const float maxMs = 4000.0f; // 最大 4s
    if (nextMs < minMs) nextMs = minMs;
    if (nextMs > maxMs) nextMs = maxMs;

    return nextMs;
}

const std::vector<Obstacle>& ObstacleManager::getObstacles() const {
    return obstacles;
}

Obstacle::BoundingBox Obstacle::boundingBox() const {
    BoundingBox box;
    box.x = x + 5;
    box.y = y + 5;
    box.width = width - 10;
    box.height = height - 10;
    return box;
}