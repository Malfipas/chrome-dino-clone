#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <string>
#include <functional>

class GameState {
public:
    enum class State { IDLE, PLAYING, PAUSED, GAME_OVER };
    
    GameState();
    
    State setState(State newState);
    bool is(State state) const;
    bool isPlaying() const;
    bool isGameOver() const;
    bool canTransitionTo(State newState) const;
    
    // 回调函数，对应JavaScript的onStateChange
    std::function<void(State, State)> onStateChange;

private:
    State state;
    State lastState;
};

#endif // GAMESTATE_HPP