
#include "game.h"
#include "sprite_renderer.h"
#include "resource_manager.h"
#include "game_maze.h"
#include "player.h"
#include "text_renderer.h"


constexpr int MAZE_WIDTH = 10;
constexpr int MAZE_HEIGHT = 10;

SpriteRenderer *Renderer;
GameMaze *maze;
Player *player;
TextRenderer *Text;
GameObject *flyingPelican;
int enemyCount;

int tasksComplete = 0;
constexpr int TOTAL_TASKS = 2;

bool enemyCleared = false;
bool powerupsReleased = false;

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
    maze = new GameMaze(ROOM_TEX_COUNT, MAZE_WIDTH, MAZE_HEIGHT, enemyCount);

    {
        auto player_tex = ResourceManager::GetTexture("player");
        std::vector<Texture2D> movingSprites;
        for (int i = 1; i <= 6; i++)
            movingSprites.push_back(ResourceManager::GetTexture("stand-" + std::to_string(i)));

        auto player_room = maze->base_room_idx();
        auto player_pos = maze->getPlayerPos(player_room);
        player = new Player(player_room, player_pos, player_tex, movingSprites);
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
    ResourceManager::LoadTexture(pathToTexture("rest.png"), true, "player");
    ResourceManager::LoadTexture(pathToTexture("elite.png"), true, "elite");

    ResourceManager::LoadTexture(pathToTexture("black-square.png"), false, "wall");

    ResourceManager::LoadTexture(pathToTexture("flag.png"), false, "points");
    ResourceManager::LoadTexture(pathToTexture("sword.png"), true, "obstacle");

    ResourceManager::LoadTexture(pathToTexture("obstacle.png"), true, "enemy_killer");
    ResourceManager::LoadTexture(pathToTexture("shield.png"), true, "powerup_release");

    ResourceManager::LoadTexture(pathToTexture("pelican.png"), true, "exit");
    ResourceManager::LoadTexture(pathToTexture("pelican-flying.png"), true, "flying-pelican");

    for (int i = 0; i < ROOM_TEX_COUNT; i++) {
        std::string name = "room" + std::to_string(i);
        auto path = ("assets/textures/" + name + ".png");

        char *path_c = (char *) malloc(sizeof(char) * (path.length() + 1));
        for (int j = 0; j < path.length(); j++) path_c[j] = path[j];
        path_c[path.length()] = 0;

        ResourceManager::LoadTexture(path_c, false, name);
    }

    for (int i = 1; i <= 6; i++) {
        std::string name = "stand-" + std::to_string(i);
        auto path = ("assets/textures/" + name + ".png");

        char *path_c = (char *) malloc(sizeof(char) * (path.length() + 1));
        for (int j = 0; j < path.length(); j++) path_c[j] = path[j];
        path_c[path.length()] = 0;

        ResourceManager::LoadTexture(path_c, true, name);
    }

//    GLfloat light1PosType[] = {100.0, 100.0, 0.0, 1.0};
//    glLightfv(GL_LIGHT1, GL_POSITION, light1PosType);
//    glEnable(GL_LIGHT1);
}

float getVelocty(double dt) {
    return 400.0f * float(dt);
}

void Game::Update(double dt) {
    if (State == GAME_ACTIVE) {
        {
            auto velocity = getVelocty(dt);

            bool hit = maze->moveEnemy(player->currRoom, *player, velocity);

            if (hit) player->enemyHit();
        }

        {
            int task = maze->getOverlappingTask(*player, player->currRoom);

            if (task > 0) {
                maze->removeTask(player->currRoom, task);

                if (task == 1 or task == 2) {
                    tasksComplete++;

                    if (tasksComplete == TOTAL_TASKS) {
                        maze->setAllTasksComplete(player->currRoom);
                    }

                    if (task == 1) {
                        enemyCleared = true;
                        maze->clearEnemies();
                    } else {
                        powerupsReleased = true;
                        maze->releasePowerups();
                    }
                } else {
                    if (task == 3) player->hitObstacle();
                    else player->pointsBoost();
                }
            }
        }

        auto lost = getTimeRemaining() == 0 or player->isDead();

        if (lost) {
            State = GAME_LOSE;
        }

        auto win = maze->isCollideWithExitNode(*player, player->currRoom);
        if (win) {
            auto flyPeliTex = ResourceManager::GetTexture("flying-pelican");
            flyingPelican = new GameObject(maze->pelicanPosition, GameRoom::getPelicanSize(), flyPeliTex);
            State = GAME_FLY;
        }
    } else if (State == GAME_FLY) {
        glm::vec2 displace(3.00f, -1.00f);
        flyingPelican->move(displace);
        auto pos = flyingPelican->Position;

        if (pos[0] >= SCREEN_WIDTH or pos[1] >= SCREEN_HEIGHT) {
            State = GAME_WIN;
        }
    }

}

void Game::ProcessInput(double dt) {
#define pressed(x) (this->Keys[x])

    switch (this->State) {
        case GAME_ACTIVE: {
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

                    break;
                }
            }

            bool wors_key = pressed(GLFW_KEY_W) or pressed(GLFW_KEY_S);
            bool a_key = pressed(GLFW_KEY_A);
            bool d_key = pressed(GLFW_KEY_D);
            player->update(wors_key, a_key, d_key);
        }
            break;
        case GAME_FLY:
            break;
        case GAME_MENU: {
            std::vector<int> counter = {0, 1, 3, 5, 10};

            bool press = false;
            int key = 0;
            for (int i = 1; i <= 4; i++) {
                if (pressed(GLFW_KEY_0 + i)) {
                    press = true;
                    key = i;
                    break;
                }
            }

            if (press) {
                enemyCount = counter[key];
                Reset();
                this->State = GAME_ACTIVE;
            }
        }

            break;
        default:
            if (pressed(GLFW_KEY_SPACE)) {
                this->State = GAME_MENU;
            }
    }
}


void Game::Render() {
    auto CHAR_HEIGHT = 20;
    auto textColor = glm::vec3(1.0f, 0.4f, 1.0f);

    auto renderActiveGame = [&]() {
        maze->Draw(*Renderer);
        player->Draw(*Renderer);

        std::vector<std::string> textsToRender = {
                "Health: " + std::to_string(player->getHealth()),
                "Tasks: " + std::to_string(tasksComplete) + " / " + std::to_string(TOTAL_TASKS),
                "Light: On",
                "Time remaining: " + std::to_string(getTimeRemaining()),
        };

        if (enemyCleared) textsToRender.emplace_back("Enemies cleared");
        if (powerupsReleased) textsToRender.emplace_back("Powerups released");

        float yOffset = 5.0f;

        for (auto &str : textsToRender) {
            Text->RenderText(str, 5.0f, yOffset, 1.0f, textColor);
            yOffset += CHAR_HEIGHT;
        }

        yOffset = 5.0f;
        auto renderLocation = [&CHAR_HEIGHT, &textColor, &yOffset](const std::string desc, int room) {
            auto[px, py] = maze->getRoomCoordinate(room);
            auto pstr = desc + " location: (" + std::to_string(px) + ", " + std::to_string(py) + ")";

            Text->RenderText(pstr, SCREEN_WIDTH - 400.0f, yOffset, 1.0f, textColor);
            yOffset += CHAR_HEIGHT;
        };

        renderLocation("Player", player->currRoom);
        auto rooms = maze->getEnemiesRooms();

        if (rooms.size() < 5) {
            for (auto i = 0; i < rooms.size(); i++) {
                renderLocation("Impostor " + std::to_string(i), rooms[i]);
            }
        }

//        if (maze->isAllTasksComplete()) {
//            renderLocation("Exit room", maze->getExitRoomIndex());
//        }
    };

    auto renderFlying = [&]() {
        maze->Draw(*Renderer);
        flyingPelican->Draw(*Renderer);
    };

    auto renderTextCenter = [&](const std::vector<std::string> &texts, float xLeft = 350.0f) {
        float yOffset = 200.0f;

        for (auto &str : texts) {
            Text->RenderText(str, xLeft, yOffset, 2.0f, glm::vec3(1.0f, 0.4f, 1.0f));
            yOffset += 2 * CHAR_HEIGHT;
        }
    };

    auto renderMenu = [&]() {
        std::vector<std::string> textsToRender = {
                "Welcome to AmongHalo!",
                "Choose difficulty below:",
                "",
                "Easy: one enemy, press 1",
                "Normal: three enemies, press 2",
                "Heroic: five enemies, press 3",
                "Legendary: ten enemies, press 4",
        };

        renderTextCenter(textsToRender, 250.0f);
    };

    auto renderWinner = [&]() {
        std::vector<std::string> textsToRender = {
                "You won the game!",
                "Press Space to play again!"
        };

        renderTextCenter(textsToRender);
    };

    auto renderLose = [&]() {
        std::vector<std::string> textsToRender = {
                "Sadly, you lost! :(",
                getTimeRemaining() == 0 ? "You ran out of time!" : "You died!",
                "Press Space to restart"
        };

        renderTextCenter(textsToRender);
    };

    switch (this->State) {
        case GAME_ACTIVE:
            renderActiveGame();
            break;
        case GAME_MENU:
            renderMenu();
            break;
        case GAME_FLY:
            renderFlying();
            break;
        case GAME_WIN:
            renderWinner();
            break;
        case GAME_LOSE:
            renderLose();
            break;
    }
}

int Game::getTimeRemaining() {
    return int(std::max(0.0, endTime - glfwGetTime()));
}

