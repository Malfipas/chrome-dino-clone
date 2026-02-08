#include "ScoreManager.hpp"
#include "constants.hpp"
#include <algorithm> // 添加这个包含，用于std::min和std::max

ScoreManager::ScoreManager() : score(0), highScore(0), scoreInterval(0), totalTime(0) {}

void ScoreManager::reset() {
    score = 0;
    scoreInterval = 0;
    totalTime = 0;
}

void ScoreManager::update(float deltaTime, bool gameRunning) {
    totalTime += deltaTime;
    
    if (gameRunning) {
        scoreInterval++;
        if (scoreInterval >= SCORE_INCREMENT_INTERVAL) {
            score++;
            scoreInterval = 0;
        }
    }
}

bool ScoreManager::updateHighScore() {
    if (score > highScore) {
        highScore = score;
        return true;
    }
    return false;
}

float ScoreManager::getGameSpeed(float baseSpeed) const {
    int speedIncrease = static_cast<int>(score * GAME_SPEED_INCREASE_RATE);
    float newSpeed = baseSpeed + speedIncrease;
    
    // 使用std::min和std::max来限制速度范围
    if (newSpeed > MAX_GAME_SPEED) {
        newSpeed = MAX_GAME_SPEED;
    }
    if (newSpeed < INITIAL_GAME_SPEED) {
        newSpeed = INITIAL_GAME_SPEED;
    }
    return newSpeed;
}

ScoreManager::State ScoreManager::getState() const {
    State state;
    state.score = score;
    state.highScore = highScore;
    state.totalTime = static_cast<int>(totalTime / 1000);
    return state;
}