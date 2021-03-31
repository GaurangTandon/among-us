#ifndef ASSIGNMENT_GAME_MAZE_H
#define ASSIGNMENT_GAME_MAZE_H

#include <algorithm>
#include <bitset>
#include <iostream>
#include <random>
#include <stack>
#include <utility>
#include <vector>

#include "game_const.h"
#include "game_room.h"
#include "player.h"
#include "resource_manager.h"
#include "task.h"

// Do we want to store powerups etc. as part of rooms or inside maze only?
class GameMaze {
    static constexpr int dx[4] = {-1, 1, 0, 0};
    static constexpr int dy[4] = {0, 0, -1, 1};
    static constexpr int rev_idx[4] = {1, 0, 3, 2};

private:
    bool exitNodeEnabled;
    std::vector<Player> enemies;
    std::vector<GameRoom> rooms;
    std::pair<int, int> exitRoom;
    int width, height;
    int enemyCount;
    // fW[i][j] = { next_node, shortest_dist }
    std::vector<std::vector<std::pair<int, int>>> floydWarshall;

    std::vector<std::bitset<4>> generateGraph(int seed = 0) {
        if (seed) srand(seed);
        else srand(time(nullptr));

        using PII = std::pair<int, int>;

        std::vector<std::vector<bool>> visited(width, std::vector<bool>(height, false));
        std::stack<PII> st;

        const int INF = 1e9;
        const int nodes = height * width;
        floydWarshall.resize(nodes);
        for (auto &vec : floydWarshall) vec.resize(nodes);
        for (int i = 0; i < nodes; i++)
            for (int j = 0; j < nodes; j++)
                floydWarshall[i][j] = {0, INF};

        auto[start_x, start_y] = base_room_coordinate();
        visited[start_x][start_y] = true;
        st.push({start_x, start_y});

        auto coord_valid = [&](int x, int y) {
            return x >= 0 and x < width and y >= 0 and y < height;
        };
        auto valid = [&](int x, int y) {
            return coord_valid(x, y) and not visited[x][y];
        };

        std::vector<std::bitset<4>> door_data(width * height, 0);

        auto add_door = [&](int roomA, int roomB, int door_idx) {
            door_data[roomA][door_idx] = true;
            door_data[roomB][rev_idx[door_idx]] = true;

            floydWarshall[roomA][roomB] = {roomB, 1};
            floydWarshall[roomB][roomA] = {roomA, 1};
        };

        while (not st.empty()) {
            auto &[nx, ny] = st.top();
            st.pop();

            int node_idx = getRoomIndex(nx, ny);
            std::vector<std::pair<int, PII>> neighbours;

            for (int i = 0; i < 4; i++) {
                PII newnode = {nx + dx[i], ny + dy[i]};

                if (not valid(newnode.first, newnode.second)) continue;

                neighbours.emplace_back(i, newnode);
            }

            if (neighbours.empty()) continue;

            st.push({nx, ny});

            auto &[idx, c] = neighbours[rand() % neighbours.size()];
            auto &[cx, cy] = c;

            visited[cx][cy] = true;
            st.push({cx, cy});

            auto chosen_node_idx = getRoomIndex(cx, cy);
            add_door(node_idx, chosen_node_idx, idx);
        }

        int LOOP_ROOMS = int(0.05f * door_data.size());

        if (width < 5) LOOP_ROOMS = 0;

        std::random_device rd;
        for (int _ = 0; _ < LOOP_ROOMS; _++) {
            int room = rd() % door_data.size();

            auto[rx, ry] = getRoomCoordinate(room);

            std::vector<int> order = {0, 1, 2, 3};
            for (int i = 0; i < 4; i++)
                std::swap(order[i], order[rd() % (i + 1)]);

            bool found = false;
            for (int door_idx = 0; door_idx < 4; door_idx++) {
                auto[cx, cy] = std::make_pair(rx + dx[door_idx], ry + dy[door_idx]);

                if (not coord_valid(cx, cy)) continue;

                if (not door_data[room][door_idx]) {
                    found = true;

                    auto chosen_node_idx = getRoomIndex(cx, cy);
                    add_door(room, chosen_node_idx, door_idx);

                    break;
                }
            }

            if (not found) LOOP_ROOMS++;
        }

        for (int i = 0; i < nodes; i++) floydWarshall[i][i] = {0, 0};

        for (int middle = 0; middle < nodes; middle++) {
            for (int a = 0; a < nodes; a++) {
                for (int b = 0; b < nodes; b++) {
                    auto[_, dist] = floydWarshall[a][b];
                    auto new_dist = floydWarshall[a][middle].second + floydWarshall[middle][b].second;
                    auto transit_node = floydWarshall[a][middle].first;

                    if (dist > new_dist)
                        floydWarshall[a][b] = {transit_node, new_dist};
                }
            }
        }

        return door_data;
    }

    std::pair<int, int> base_room_coordinate() {
        return {width / 2, height / 2};
    }

    void generateRooms(int texture_count) {
        int nodes = width * height;
        auto graphData = generateGraph();
        rooms.reserve(nodes);

        glm::vec2 first_room_pos;
        auto[cx, cy] = base_room_coordinate();
        first_room_pos[0] = SCREEN_WIDTH / 2 - cx * GameRoom::SIZE[0];
        first_room_pos[1] = SCREEN_HEIGHT / 2 - cy * GameRoom::SIZE[1];

        for (int room_idx = 0; room_idx < nodes; room_idx++) {
            int row = room_idx / width, col = room_idx % width;

            glm::vec2 offset(float(col) * GameRoom::SIZE[0], float(row) * GameRoom::SIZE[1]);

            auto position = first_room_pos + offset;

            auto roomSprite = ResourceManager::GetTexture("room" + std::to_string(room_idx % texture_count));
            rooms.emplace_back(position, roomSprite, graphData[room_idx]);
        }
    }

    void addNewEnemies(int playerRoom) {
        auto far_rooms = getFarthestRooms(playerRoom);
        auto enemy_tex = ResourceManager::GetTexture("elite");

        int step = far_rooms.size() / (enemyCount + 1);
        int curr_room = 0;

        for (int i = 0; i < enemyCount; i++) {
            auto room = far_rooms[curr_room];
            enemies.emplace_back(Player(room, getPlayerPos(room), enemy_tex, {enemy_tex}));
            curr_room += step;
        }
    }

    std::vector<int> getFarthestRooms(int sourceRoom) {
        std::vector<int> roomIndices(rooms.size());
        std::iota(roomIndices.begin(), roomIndices.end(), 0);
        std::sort(roomIndices.begin(), roomIndices.end(), [&](auto a, auto b) {
            return floydWarshall[a][sourceRoom].second > floydWarshall[b][sourceRoom].second;
        });

        return roomIndices;
    }

    void addTasks(int playerRoom) {
        auto roomIndices = getFarthestRooms(playerRoom);

        for (int type = 1; type <= 2; type++) {
            auto roomIndex = roomIndices[type - 1];
            auto &randRoomObj = rooms[roomIndex];
            randRoomObj.addTask(getPlayerPos(roomIndex), type);
        }
    }

public:
    glm::vec2 pelicanPosition;

    GameMaze(int tex_count, int w = 3, int h = 3, int ene_count = 1) : width(w), height(h), enemyCount(ene_count) {
        generateRooms(tex_count);

        addNewEnemies(getRoomIndex(base_room_coordinate()));

        addTasks(getRoomIndex(base_room_coordinate()));

        exitNodeEnabled = false;
    }

    void Draw(SpriteRenderer &renderer) {
        for (auto &room : rooms) room.Draw(renderer);
        for (auto &room : rooms) room.DrawAddons(renderer);
        for (auto &enemy : enemies) enemy.Draw(renderer);
    }

    bool moveEnemy(int targetRoom, const GameObject &player, float velocity) {
        auto getVelocity = [&velocity]() {
            return ((rand() % 150) / 100.0f) * velocity;
        };

        auto move_towards_target = [&getVelocity](GameObject &object, const glm::vec2 &targetPos) {
            auto &currPos = object.Position;
            std::vector<int> indices = {0, 1};
            if (rand() % 2) indices = {1, 0};

            for (const auto &i : indices) {
                auto diff = std::abs(currPos[i] - targetPos[i]);
                if (diff < 3) continue;

                float sign = currPos[i] > targetPos[i] ? -1 : 1;
                currPos[i] += getVelocity() * sign;
                break;
            }
        };

        for (auto &enemy : enemies) {
            const auto &currRoom = enemy.currRoom;

            if (targetRoom == currRoom) {
                const auto &playerPos = player.Position;
                move_towards_target(enemy, playerPos);
                continue;
            }

            auto nextRoom = floydWarshall[currRoom][targetRoom].first;
            const auto &currRoomObj = rooms[currRoom];
            const auto &nextRoomObj = rooms[nextRoom];

            if (nextRoomObj.contains(enemy, 0.99)) {
                enemy.currRoom = nextRoom;
            } else if (currRoomObj.contains(enemy, 0.01f)) {
                auto dir = glm::normalize(nextRoomObj.Position - currRoomObj.Position);
                enemy.Position += dir * getVelocity();
            } else {
                auto dir = glm::normalize(currRoomObj.Position - enemy.Position);
                enemy.Position += dir * getVelocity();
                // :(
//                assert(false);
            }
        }

        bool hit = false;

        for (auto &enemy : enemies)
            hit = hit or enemy.hasOverlap(player);

        return hit;
    }

    void moveAll(const glm::vec2 &displace) {
        for (auto &room : rooms) room.moveAll(displace);
        for (auto &enemy : enemies) enemy.Position += displace;
    }

    int base_room_idx() {
        auto[cx, cy] = base_room_coordinate();
        return getRoomIndex(cx, cy);
    }

    int find_player_room(const GameObject &player) {
        int idx = -1;

        for (auto &room : rooms) {
            idx++;

            float area_overlap = room.areaOverlap(player);
            float threshold = 0.45f * player.area();

            if (area_overlap < threshold) continue;

            if (room.wallOverlaps(player)) return -1;

            return idx;
        }

        return -1;
    }

    glm::vec2 getPlayerPos(int room) {
        auto room_center = rooms[room].getCenterCoordinate();
        auto player_pos = room_center - PLAYER_SIZE / 2.0f;
        return player_pos;
    }

    int getOverlappingTask(const GameObject &object, int room) {
        return rooms[room].overlapsTask(object);
    }

    void clearEnemies() {
        enemies.clear();
    }

    void releasePowerups() {
        std::vector<int> choice(rooms.size());
        std::iota(choice.begin(), choice.end(), 0);

        std::random_device dev;
        std::mt19937 mt(dev());
        std::shuffle(choice.begin(), choice.end(), mt);

        const int take = int(0.4f * rooms.size());

        for (int i = 0; i < take; i++) {
            auto roomIdx = choice[i];

            int powerup = dev() % 10 <= 7;
            rooms[roomIdx].addTask(getPlayerPos(roomIdx), powerup ? 3 : 4);
        }
    }

    void removeTask(int room, int task) {
        rooms[room].removeTask(task);
    }

    bool isCollideWithExitNode(const GameObject &player, int currRoom) {
        bool yes = exitNodeEnabled and getExitRoomIndex() == currRoom and rooms[currRoom].exitNodeOverlap(player);
        if (yes) {
            rooms[currRoom].removeExit();
            pelicanPosition = rooms[currRoom].getExitNodePosition();
        }
        return yes;
    }

    void setAllTasksComplete(int playerRoom) {
        exitNodeEnabled = true;
        auto roomIdx = getFarthestRooms(playerRoom)[0];
        exitRoom = getRoomCoordinate(roomIdx);
        rooms[roomIdx].setExitNode();
    }

    bool isAllTasksComplete() {
        return exitNodeEnabled;
    }

    std::pair<int, int> getRoomCoordinate(int roomIndex) {
        return {roomIndex / width, roomIndex % width};
    }

    int getRoomIndex(int x, int y) const { return x * width + y; }

    int getRoomIndex(std::pair<int, int> p) {
        return getRoomIndex(p.first, p.second);
    }

    std::vector<int> getEnemiesRooms() {
        std::vector<int> res;
        for (const auto &en : enemies) res.push_back(en.currRoom);
        return res;
    }

    int getExitRoomIndex() {
        return getRoomIndex(exitRoom);
    }
};


#endif //ASSIGNMENT_GAME_MAZE_H
