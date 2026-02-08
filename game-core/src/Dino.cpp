#include "Dino.hpp"

Dino::Dino() {
    reset();
}

void Dino::reset() {
    x = DINO_X;
    y = GROUND_Y - DinoConstants::HEIGHT;
    yVelocity = 0;
    width = DinoConstants::WIDTH;
    height = DinoConstants::HEIGHT;
    isJumping = false;
    isDead = false;
    isOnGround = true;
    currentSprite = DinoConstants::RUN_1;
}

void Dino::update(float deltaTime) {
    // deltaTime 以毫秒为单位，使用帧数比例计算位置与速度变化
    float frames = deltaTime / 16.67f;
    if (!isOnGround) {
        yVelocity += GRAVITY * frames;
    }
    y += yVelocity * frames;
    
    // 地面碰撞检测
    if (y >= GROUND_Y - height) {
        y = GROUND_Y - height;
        yVelocity = 0;
        isJumping = false;
        isOnGround = true;
    } else {
        isOnGround = false;
    }
    
    updateSprite();
}

bool Dino::jump() {
    if (!isJumping && !isDead) {
        yVelocity = JUMP_FORCE;
        isJumping = true;
        return true;
    }
    return false;
}

void Dino::die() {
    isDead = true;
    currentSprite = DinoConstants::DEAD;
}

Dino::State Dino::getState() const {
    State state;
    state.x = x;
    state.y = y;
    state.width = width;
    state.height = height;
    state.isJumping = isJumping;
    state.isDead = isDead;
    state.sprite = {currentSprite.x, currentSprite.y, currentSprite.w, currentSprite.h};
    return state;
}

Dino::BoundingBox Dino::getBoundingBox() const {
    BoundingBox box;
    box.x = x + 10;
    box.y = y + 10;
    box.width = width - 20;
    box.height = height - 20;
    return box;
}

void Dino::updateSprite() {
    if (isDead) return;
    
    if (isJumping) {
        currentSprite = DinoConstants::JUMP;
    } else {
        // 简单的基于帧切换动画
        static int counter = 0;
        counter = (counter + 1) % 10; // 每10次update切换一次
        int frame = (counter < 5) ? 0 : 1;
        currentSprite = (frame == 0) ? DinoConstants::RUN_1 : DinoConstants::RUN_2;
    }
}
