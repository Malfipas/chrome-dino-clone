#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

// 游戏物理参数
constexpr float GRAVITY = 1.5f; // 重力（加快下落速度，设为1.5）
// 跳跃力度、初始速度
constexpr float JUMP_FORCE = -38.0f; // 跳跃力度（负值向上，设为-38）
constexpr float INITIAL_GAME_SPEED = 13.0f; // 初始速度（设为13）
constexpr float MAX_GAME_SPEED = 30.0f; // 最大速度，至少为30以满足要求

// 游戏对象尺寸（基于精灵图）
struct DinoConstants {
    constexpr static int WIDTH = 89;
    constexpr static int HEIGHT = 94;
    
    struct Sprite {
        int x, y, w, h;
    };
    
    constexpr static Sprite RUN_1 = {1514, 0, 88, 94};
    constexpr static Sprite RUN_2 = {1602, 0, 88, 94};
    constexpr static Sprite JUMP = {1338, 0, 88, 94};
    constexpr static Sprite DEAD = {1788, 0, 88, 94};
};

struct ObstacleConstants {
    struct Config {
        int WIDTH;
        int HEIGHT;
        int SPRITE_X;
        int SPRITE_Y;
    };
    
    constexpr static Config SMALL = {34, 70, 446, 2};
    constexpr static Config BIG = {49, 100, 652, 2};
};

// 游戏区域
constexpr int CANVAS_WIDTH = 1300;
constexpr int CANVAS_HEIGHT = 800;
constexpr int GROUND_Y = 600;
constexpr int DINO_X = 100;

// 游戏逻辑
constexpr int SCORE_INCREMENT_INTERVAL = 5; // 每5帧增加1分
constexpr int OBSTACLE_SPAWN_RANGE_MIN = 1800; // 最小间隔增大，减少密集刷怪
constexpr int OBSTACLE_SPAWN_RANGE_MAX = 2500;
constexpr float GAME_SPEED_INCREASE_RATE = 0.005f; // 每400分增加2速度

#endif // CONSTANTS_HPP