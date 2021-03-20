#include "game.h"

Game::Game(unsigned int width, unsigned int height)
        : State(GAME_ACTIVE), Keys(), Width(width), Height(height) {

}

Game::~Game() {

}

void Game::Init() {

}

void Game::Update(double dt) {

}

void Game::ProcessInput(double dt) {
#define pressed(x) (glfwGetKey(window, x) == GLFW_PRESS)
}

void Game::Render() {

}

