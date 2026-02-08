#include "GameEngine.hpp"
#include "Dino.hpp"
#include "ObstacleManager.hpp"
#include "CollisionSystem.hpp"
#include "ScoreManager.hpp"
#include "GameState.hpp"
#include "constants.hpp"

#include <cstring>
#include <cmath>
#include <vector>

// 简单内存管理
static float* flattenedState = nullptr;
static int flattenedStateSize = 0;

GameEngine::GameEngine() {
    dino = new Dino();
    obstacleManager = new ObstacleManager();
    collisionSystem = new CollisionSystem();
    scoreManager = new ScoreManager();
    gameState = new GameState();
    
    gameSpeed = INITIAL_GAME_SPEED;
    lastTime = 0;
    groundOffset = 0;
    
    // 加载最高分
    loadHighScore();
    
    gameState->onStateChange = [this](GameState::State newState, GameState::State oldState) {
        // 避免递归的检查
        if (newState == oldState) return;

        switch (newState) {
        case GameState::State::GAME_OVER:
            saveHighScore();
            break;
         case GameState::State::IDLE:
         case GameState::State::PLAYING:
         case GameState::State::PAUSED:
        // 这些状态不需要特殊处理
        break;
}
        // switch (newState) {
        //     case GameState::State::GAME_OVER:
        //         saveHighScore();
        //         break;
        // }
    };
}

GameEngine::~GameEngine() {
    delete dino;
    delete obstacleManager;
    delete collisionSystem;
    delete scoreManager;
    delete gameState;
    
    if (flattenedState) {
        delete[] flattenedState;
        flattenedState = nullptr;
    }
}

bool GameEngine::start() {
    // 检查当前状态，如果已经是PLAYING，直接返回
    if (gameState->is(GameState::State::PLAYING)) {
        return true;
    }

    if (gameState->canTransitionTo(GameState::State::PLAYING)) {
        // 直接设置状态
        gameState->setState(GameState::State::PLAYING);
        //reset();
        lastTime = 0; // 会在update中设置

        // 确保恐龙处于正常状态
        if (dino->getState().isDead) {
            dino->reset();
        }

        return true;
    }
    return false;
}

bool GameEngine::reset() {
    dino->reset();
    obstacleManager->reset();
    scoreManager->reset();
    gameSpeed = INITIAL_GAME_SPEED;
    groundOffset = 0;

    if (gameState->canTransitionTo(GameState::State::IDLE)) {
        gameState->setState(GameState::State::IDLE);
        return true;
    }
    return false;
}

void GameEngine::update(float currentTime) {
    if (!gameState->isPlaying()) return;

    // 计算时间增量
    //float deltaTime = (lastTime == 0) ? 0 : (currentTime - lastTime);
    // 计算时间增量（使用毫秒），并限制范围以避免跳帧过大
    float deltaMs;

    if (lastTime == 0) {
        // 第一帧，使用约一帧的默认值（16.67ms -> 60FPS）
        deltaMs = 16.67f;
    } else {
        deltaMs = currentTime - lastTime; // currentTime 是毫秒

        // 限制 deltaMs 范围，避免异常情况
        if (deltaMs > 50.0f) deltaMs = 50.0f; // 最大限制为 50ms (~20FPS)
        if (deltaMs < 1.0f) deltaMs = 1.0f; // 最小 1ms
    }

    lastTime = currentTime;

    // 将 gameSpeed（以每帧单位为基准）按帧数缩放
    float frames = deltaMs / 16.67f;

    // 更新地面滚动（以帧为单位移动）
    groundOffset = fmod(groundOffset + gameSpeed * frames, 2404.0f);

    // 更新各个模块（以毫秒或帧为单位，模块内部负责如何使用）
    dino->update(deltaMs);
    obstacleManager->update(deltaMs, gameSpeed);
    scoreManager->update(deltaMs, gameState->isPlaying());

    // 更新游戏速度（基于分数）
    gameSpeed = scoreManager->getGameSpeed(INITIAL_GAME_SPEED);

    // 检测碰撞
    const auto& obstacles = obstacleManager->getObstacles();
    auto collisionResult = collisionSystem->checkCollision(*dino, *obstacleManager);

    if (collisionResult.collided && !dino->getState().isDead) {
        gameOver();
    }
}

bool GameEngine::jump() {
    if (gameState->isPlaying()) {
        return dino->jump();
    }
    return false;
}

void* GameEngine::gameOver() {
    dino->die();
    bool newRecord = scoreManager->updateHighScore();

    if (gameState->canTransitionTo(GameState::State::GAME_OVER)) {
        gameState->setState(GameState::State::GAME_OVER);
        
        // 保存最高分
        saveHighScore();

        // 创建返回结果
        static struct {
            bool newRecord;
            int finalScore;
            int highScore;
        } result;
        
        result.newRecord = newRecord;
        result.finalScore = scoreManager->score;
        result.highScore = scoreManager->highScore;
        
        return &result;
    }
    return nullptr;
}

void GameEngine::saveHighScore() {
#ifdef __EMSCRIPTEN__
    
    js_save_high_score(scoreManager->highScore);
#else
    // 对于非WebAssembly环境，可以使用文件或其他存储方式
    printf("High score saved: %d\n", scoreManager->highScore);
#endif
}

void GameEngine::loadHighScore() {
#ifdef __EMSCRIPTEN__
    
    int loadedScore = js_load_high_score();
    scoreManager->highScore = loadedScore;
    printf("High score loaded: %d\n", loadedScore);
#else
    // 对于非WebAssembly环境，可以从文件加载
    printf("Loading high score from file (simulated)...\n");
    // 这里可以添加文件读取逻辑
#endif
}

float* GameEngine::getFlattenedState() {
    auto dinoState = dino->getState();
    auto obstacles = obstacleManager->getObstacles();
    auto scoreState = scoreManager->getState();

    // 计算所需数组大小
    int obstacleCount = static_cast<int>(obstacles.size());
    int size = 11 + obstacles.size() * 5; // 基本数据 + 障碍物数据

    // 重新分配内存
    if (flattenedStateSize < size) {
        delete[] flattenedState;
        flattenedState = new float[size];
        flattenedStateSize = size;
    }

    // 填充数据
    int index = 0;
    flattenedState[index++] = dinoState.x;
    flattenedState[index++] = dinoState.y;
    flattenedState[index++] = static_cast<float>(dinoState.width);
    flattenedState[index++] = static_cast<float>(dinoState.height);
    flattenedState[index++] = dinoState.isJumping ? 1.0f : 0.0f;
    flattenedState[index++] = dinoState.isDead ? 1.0f : 0.0f;
    flattenedState[index++] = groundOffset;
    flattenedState[index++] = gameSpeed;
    flattenedState[index++] = static_cast<float>(scoreState.score);
    flattenedState[index++] = static_cast<float>(scoreState.highScore);
    //flattenedState[index++] = static_cast<float>(obstacles.size());
    flattenedState[index++] = static_cast<float>(obstacleCount); // 确保这里是整型转换

    // 添加障碍物数据
    for (const auto& obs : obstacles) {
        flattenedState[index++] = obs.x;
        flattenedState[index++] = obs.y;
        flattenedState[index++] = static_cast<float>(obs.width);
        flattenedState[index++] = static_cast<float>(obs.height);
        flattenedState[index++] = (obs.type == "small") ? 1.0f : 0.0f;
    }

    return flattenedState;
}

GameEngine::RenderState GameEngine::getStateForRender() {
    RenderState state;
    
    // 手动复制恐龙状态
    auto dinoState = dino->getState();
    state.dino.x = dinoState.x;
    state.dino.y = dinoState.y;
    state.dino.width = dinoState.width;
    state.dino.height = dinoState.height;
    state.dino.isJumping = dinoState.isJumping;
    state.dino.isDead = dinoState.isDead;
    state.dino.sprite.x = dinoState.sprite.x;
    state.dino.sprite.y = dinoState.sprite.y;
    state.dino.sprite.w = dinoState.sprite.w;
    state.dino.sprite.h = dinoState.sprite.h;
    
    state.obstacles = &obstacleManager->getObstacles();
    state.groundOffset = groundOffset;
    state.gameSpeed = gameSpeed;
    
    // 手动复制分数状态
    auto scoreState = scoreManager->getState();
    state.score.score = scoreState.score;
    state.score.highScore = scoreState.highScore;
    state.score.totalTime = scoreState.totalTime;
    
    if (gameState->isPlaying()) {
        state.gameState = 1;
    } else if (gameState->isGameOver()) {
        state.gameState = 2;
    } else {
        state.gameState = 0; // IDLE
    }
    
    return state;
}

int GameEngine::getScore() const {
    return scoreManager->score;
}

int GameEngine::getHighScore() const {
    return scoreManager->highScore;
}

void GameEngine::setHighScore(int highScore) {
    scoreManager->highScore = highScore;
}