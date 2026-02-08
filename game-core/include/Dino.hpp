#ifndef DINO_HPP
#define DINO_HPP

#include "constants.hpp"

class Dino {
public:
    Dino();
    void reset();
    void update(float deltaTime);
    bool jump();
    void die();
    
    struct State {
        float x, y;
        int width, height;
        bool isJumping;
        bool isDead;
        struct Sprite {
            int x, y, w, h;
        } sprite;
    };
    
    State getState() const;
    
    struct BoundingBox {
        float x, y;
        float width, height;
    };
    
    BoundingBox getBoundingBox() const;

private:
    void updateSprite();
    
    float x;
    float y;
    float yVelocity;
    int width;
    int height;
    bool isJumping;
    bool isDead;
    bool isOnGround;
    DinoConstants::Sprite currentSprite;
};

#endif // DINO_HPP
