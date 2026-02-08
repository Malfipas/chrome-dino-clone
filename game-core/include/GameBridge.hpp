#ifndef GAMEBRIDGE_HPP
#define GAMEBRIDGE_HPP

#ifdef __cplusplus
extern "C" {
#endif

// C接口，便于JavaScript调用
void game_init();
void game_update(float currentTime);
int game_jump();
void game_restart();
void game_start();
float* game_get_state_array();
int game_is_playing();
int game_is_game_over();
int game_get_score();
int game_get_high_score();

#ifdef __cplusplus
}
#endif

#endif // GAMEBRIDGE_HPP