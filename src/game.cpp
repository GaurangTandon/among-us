#include "game.h"
#include "sprite_renderer.h"
#include "resource_manager.h"
#include "game_maze.h"

SpriteRenderer *Renderer;
GameMaze *maze;

Game::Game(unsigned int width, unsigned int height)
        : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {

}

Game::~Game() {
    delete Renderer;
}

#define pathToShader(filename) "assets/shaders/" filename
#define pathToTexture(filename) "assets/textures/" filename

void Game::Init() {
    ResourceManager::LoadShader(pathToShader("sprite.vs"), pathToShader("sprite.fs"), nullptr, "sprite");
    ResourceManager::LoadShader(pathToShader("particle.vs"), pathToShader("particle.fs"), nullptr, "particle");
    ResourceManager::LoadShader(pathToShader("post_processing.vs"), pathToShader("post_processing.fs"), nullptr,
                                "postprocessing");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f,
                                      -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

    // set render-specific controls
    auto x = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(x);
    // load textures
    ResourceManager::LoadTexture(pathToTexture("awesomeface.png"), true, "face");
    ResourceManager::LoadTexture(pathToTexture("awesomeface.png"), true, "room");
    ResourceManager::LoadTexture(pathToTexture("awesomeface.png"), true, "door");

    maze = new GameMaze();
}

void Game::Update(double dt) {

}

void Game::ProcessInput(double dt) {
#define pressed(x) (this->Keys[x])

    if (this->State == GAME_ACTIVE) {
        float velocity = 100.0f * float(dt);

        std::map<unsigned int, glm::vec2> movers = {
                {GLFW_KEY_W, glm::vec2(0.0f, -1.0f)},
                {GLFW_KEY_A, glm::vec2(-1.0f, 0.0f)},
                {GLFW_KEY_S, glm::vec2(0.0f, 1.0f)},
                {GLFW_KEY_D, glm::vec2(1.0f, 0.0f)}
        };

        for (auto &[key, displace]: movers) {
            if (pressed(key)) {
                maze->moveAll(displace * velocity);
                return;
            }
        }
    }
}

void Game::Render() {
    if (this->State == GAME_ACTIVE) {
        maze->Draw(*Renderer);
    }
}

