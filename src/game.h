#ifndef A1_GAM_H
#define A1_GAM_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game {
public:
    static constexpr int ROOM_TEX_COUNT = 4;
    static constexpr int DURATION = 50;
    // game state
    GameState State;
    bool Keys[1024];
    unsigned int Width, Height;
    double endTime;

    // constructor/destructor
    Game(unsigned int width, unsigned int height);

    ~Game();

    // initialize game state (load all shaders/textures/levels)
    void Init();

    // game loop
    void ProcessInput(double dt);

    void Update(double dt);

    void Render();

    void Reset();

    int getTimeRemaining();
};


#endif //A1_GAM_H
