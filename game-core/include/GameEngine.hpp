#ifndef GAMEENGINE_HPP
#define GAMEENGINE_HPP

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <string>
#include <functional>

class Dino;
class ObstacleManager;
class CollisionSystem;
class ScoreManager;
class GameState;

// 前向声明 JavaScript 函数，但不在这里定义
#ifdef __EMSCRIPTEN__
extern "C" {
    void js_save_high_score(int score);
    int js_load_high_score();
}
#endif

class GameEngine {
public:
    GameEngine();
    ~GameEngine();
    
    bool start();
    bool reset();
    void update(float currentTime);
    bool jump();
    void* gameOver(); // 返回游戏结束信息
    
    // 保存和加载最高分
    void saveHighScore();
    void loadHighScore();
    
    float* getFlattenedState();
    
    struct RenderState {
        struct DinoState {
            float x, y;
            int width, height;
            bool isJumping;
            bool isDead;
            struct Sprite {
                int x, y, w, h;
            } sprite;
        } dino;
        
        const std::vector<struct Obstacle>* obstacles;
        float groundOffset;
        float gameSpeed;
        
        struct ScoreState {
            int score;
            int highScore;
            int totalTime;
        } score;
        
        int gameState; // 0:IDLE, 1:PLAYING, 2:GAME_OVER
    };
    
    RenderState getStateForRender();
    
    // 获取当前分数和最高分
    int getScore() const;
    int getHighScore() const;
    void setHighScore(int highScore);

private:
    Dino* dino;
    ObstacleManager* obstacleManager;
    CollisionSystem* collisionSystem;
    ScoreManager* scoreManager;
    GameState* gameState;
    
    float gameSpeed;
    float lastTime;
    float groundOffset;
};

#endif // GAMEENGINE_HPP