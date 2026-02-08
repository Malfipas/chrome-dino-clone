#include "GameState.hpp"

GameState::GameState() : state(State::IDLE), lastState(State::IDLE) {}

GameState::State GameState::setState(State newState) {
    if (state == newState) {
        return state;
    }
    
    if (!canTransitionTo(newState)) {
        return state;
    }
    
    lastState = state;
    state = newState;
    
    if (onStateChange) {
        // 模拟JavaScript的setTimeout(..., 0)
        onStateChange(newState, lastState);
    }
    
    return state;
}

bool GameState::is(State state) const {
    return this->state == state;
}

bool GameState::isPlaying() const {
    return state == State::PLAYING;
}

bool GameState::isGameOver() const {
    return state == State::GAME_OVER;
}

bool GameState::canTransitionTo(State newState) const {
    switch (state) {
        case State::IDLE:
            return newState == State::PLAYING;
        case State::PLAYING:
            return newState == State::GAME_OVER;
        case State::GAME_OVER:
            return newState == State::IDLE;
        default:
            return false;
    }
}