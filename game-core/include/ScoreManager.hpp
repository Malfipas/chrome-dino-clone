#ifndef SCOREMANAGER_HPP
#define SCOREMANAGER_HPP

class ScoreManager {
public:
    ScoreManager();
    void reset();
    void update(float deltaTime, bool gameRunning);
    bool updateHighScore();
    float getGameSpeed(float baseSpeed) const;
    
    struct State {
        int score;
        int highScore;
        int totalTime; // 秒
    };
    
    State getState() const;
    
    int score;
    int highScore;

private:
    int scoreInterval;
    float totalTime;
};

#endif // SCOREMANAGER_HPP