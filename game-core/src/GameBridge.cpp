#include "GameBridge.hpp"
#include "GameEngine.hpp"

#ifdef __EMSCRIPTEN__
// 在这里定义 EM_JS 函数，避免重复
EM_JS(void, js_save_high_score, (int score), {
    if (typeof window !== 'undefined' && window.localStorage) {
        localStorage.setItem('dino_high_score', score.toString());
    }
});

EM_JS(int, js_load_high_score, (), {
    if (typeof window !== 'undefined' && window.localStorage) {
        const saved = localStorage.getItem('dino_high_score');
        return saved ? parseInt(saved) : 0;
    }
    return 0;
});
#endif

static GameEngine* engine = nullptr;

void game_init() {
    if (!engine) {
        engine = new GameEngine();
        engine->loadHighScore();
        //engine->start();
        // 改为调用 reset()，让游戏处于 IDLE 状态
        engine->reset();
    }
}

void game_start() {
    if (engine) {
        engine->start();
    }
}

void game_update(float currentTime) {
    if (engine) {
        engine->update(currentTime);
    }
}

int game_jump() {
    if (engine) {
        return engine->jump() ? 1 : 0;
    }
    return 0;
}

void game_restart() {
    if (engine) {
        engine->reset();
    }
}

float* game_get_state_array() {
    if (engine) {
        return engine->getFlattenedState();
    }
    return nullptr;
}

int game_is_playing() {
    if (engine) {
        const auto state = engine->getStateForRender();
        return state.gameState == 1 ? 1 : 0;
    }
    return 0;
}

int game_is_game_over() {
    if (engine) {
        const auto state = engine->getStateForRender();
        return state.gameState == 2 ? 1 : 0;
    }
    return 0;
}

int game_get_score() {
    if (engine) {
        return engine->getScore();
    }
    return 0;
}

int game_get_high_score() {
    if (engine) {
        return engine->getHighScore();
    }
    return 0;
}