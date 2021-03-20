#include "game.h"
#include "sprite_renderer.h"
#include "resource_manager.h"

SpriteRenderer *Renderer;

Game::Game(unsigned int width, unsigned int height)
        : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {

}

Game::~Game() {

}

#define pathToShader(filename) "assets/shaders/" filename
#define pathToTextures(filename) "assets/textures/" filename

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
    ResourceManager::LoadTexture(pathToTextures("awesomeface.png"), true, "face");
}

void Game::Update(double dt) {

}

void Game::ProcessInput(double dt) {
#define pressed(x) (glfwGetKey(window, x) == GLFW_PRESS)
}

void Game::Render() {
    auto x = ResourceManager::GetTexture("face");
    Renderer->DrawSprite(x, glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

