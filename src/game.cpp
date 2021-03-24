#include "game.h"
#include "sprite_renderer.h"
#include "resource_manager.h"
#include "game_maze.h"
#include "player.h"
#include "text_renderer.h"

SpriteRenderer *Renderer;
GameMaze *maze;
Player *player;
TextRenderer *Text;

Game::Game(unsigned int width, unsigned int height)
        : State(GAME_MENU), Keys(), Width(width), Height(height) {

}

Game::~Game() {
    delete Renderer;
}

#define pathToShader(filename) "assets/shaders/" filename
#define pathToTexture(filename) "assets/textures/" filename
#define pathToFont(filename) "assets/fonts/" filename

void Game::Reset() {
    maze = new GameMaze(ROOM_TEX_COUNT, 3, 3);

    {
        auto player_tex = ResourceManager::GetTexture("player");
        auto player_tex_hit = ResourceManager::GetTexture("player_hit");
        auto room_center = maze->base_room_center_position();
        auto player_pos = room_center - PLAYER_SIZE / 2.0f;
        auto player_room = maze->base_room_idx();
        player = new Player(player_room, player_pos, player_tex, player_tex_hit);
    }

    endTime = glfwGetTime() + DURATION;
}

void Game::Init() {
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load(pathToFont("ocraext.TTF"), 24);

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
    ResourceManager::LoadTexture(pathToTexture("transparent.png"), true, "door");
    ResourceManager::LoadTexture(pathToTexture("mario_transparent.png"), true, "player");
    ResourceManager::LoadTexture(pathToTexture("mario_transparent_hit.png"), true, "player_hit");
    ResourceManager::LoadTexture(pathToTexture("bowser_transparent.png"), true, "bowser");

    ResourceManager::LoadTexture(pathToTexture("black-square.png"), false, "wall");

    for (int i = 0; i < ROOM_TEX_COUNT; i++) {
        std::string name = "room" + std::to_string(i);
        auto path = ("assets/textures/" + name + ".png");

        char *path_c = (char *) malloc(sizeof(char) * (path.length() + 1));
        for (int j = 0; j < path.length(); j++) path_c[j] = path[j];
        path_c[path.length()] = 0;

        ResourceManager::LoadTexture(path_c, false, name);
    }

    // TODO: delete when finished
    {
        State = GAME_ACTIVE;
        Reset();
    }
}

float getVelocty(double dt) {
    return 400.0f * float(dt);
}

void Game::Update(double currentTime, double dt) {
    if (State == GAME_ACTIVE) {
        auto velocity = getVelocty(dt);
        auto enemyVelocity = velocity / 2;

        bool hit = maze->moveEnemy(player->currRoom, *player, enemyVelocity);
        if (hit) player->hit(int(currentTime));
        player->update(int(currentTime));
    }

    auto lost = getTimeRemaining() == 0 or player->isDead();

    if (lost) {
        State = GAME_LOSE;
        return;
    }
}

void Game::ProcessInput(double dt) {
#define pressed(x) (this->Keys[x])

    if (this->State == GAME_ACTIVE) {
        auto velocity = getVelocty(dt);

        std::map<unsigned int, glm::vec2> movers = {
                {GLFW_KEY_W, glm::vec2(0.0f, -1.0f)},
                {GLFW_KEY_A, glm::vec2(-1.0f, 0.0f)},
                {GLFW_KEY_S, glm::vec2(0.0f, 1.0f)},
                {GLFW_KEY_D, glm::vec2(1.0f, 0.0f)}
        };

        for (auto &[key, displace]: movers) {
            if (pressed(key)) {
                auto mazeDisplace = -displace * velocity;

                maze->moveAll(mazeDisplace);

                int roomBelongs = maze->find_player_room(*player);

                if (roomBelongs == -1) maze->moveAll(-mazeDisplace);
                else player->currRoom = roomBelongs;

                return;
            }
        }
    } else {
        if (pressed(GLFW_KEY_SPACE)) {
            Reset();
            this->State = GAME_ACTIVE;
        }
    }
}


void Game::Render() {
    auto renderActiveGame = [&]() {
        maze->Draw(*Renderer);
        player->Draw(*Renderer);

        std::vector<std::string> textsToRender = {
                "Health: " + std::to_string(player->getHealth()),
                "Tasks: ",
                "Light: ",
                "Time remaining: " + std::to_string(getTimeRemaining())
        };

        float yOffset = 5.0f;

        for (auto &str : textsToRender) {
            Text->RenderText(str, 5.0f, yOffset, 1.0f, glm::vec3(1.0f, 0.4f, 1.0f));
            yOffset += 20;
        }
    };

    auto renderMenu = [&]() {

    };

    switch (this->State) {
        case GAME_ACTIVE:
            renderActiveGame();
            break;
        case GAME_MENU:
            renderMenu();
            break;
        case GAME_WIN:
//            renderWinner();
            break;
        case GAME_LOSE:
//            renderLose();
            break;
    }
}

int Game::getTimeRemaining() {
    return int(std::max(0.0, endTime - glfwGetTime()));
}

