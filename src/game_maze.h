#ifndef ASSIGNMENT_GAME_MAZE_H
#define ASSIGNMENT_GAME_MAZE_H

#include <stack>
#include <vector>
#include <utility>
#include <iostream>
#include <bitset>
#include "game_const.h"
#include "game_room.h"
#include "resource_manager.h"
#include "player.h"

// Do we want to store powerups etc. as part of rooms or inside maze only?
class GameMaze {
    static constexpr int dx[4] = {-1, 1, 0, 0};
    static constexpr int dy[4] = {0, 0, -1, 1};
    static constexpr int rev_idx[4] = {1, 0, 3, 2};

private:
    std::vector<Player> enemies;
    std::vector<GameRoom> rooms;
    int width, height;
    // fW[i][j] = { next_node, shortest_dist }
    std::vector<std::vector<std::pair<int, int>>> floydWarshall;

    [[nodiscard]] int room_index(int x, int y) const { return x * width + y; }

    std::vector<std::bitset<4>> generateTree(int seed = 0) {
        srand(seed);
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

        auto valid = [&](int x, int y) {
            return x >= 0 and x < width and y >= 0 and y < height and not visited[x][y];
        };

        std::vector<std::bitset<4>> door_data(width * height, 0);

        while (not st.empty()) {
            auto &[nx, ny] = st.top();
            st.pop();

            int node_idx = room_index(nx, ny);
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

            auto chosen_node_idx = room_index(cx, cy);

            door_data[node_idx][idx] = true;
            door_data[chosen_node_idx][rev_idx[idx]] = true;

            floydWarshall[node_idx][chosen_node_idx] = {chosen_node_idx, 1};
            floydWarshall[chosen_node_idx][node_idx] = {node_idx, 1};
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
        auto treeData = generateTree();
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
            rooms.emplace_back(position, roomSprite, treeData[room_idx]);
        }
    }

    int doorIndex(int currRoom, int nextRoom) {
        for (int i = 0; i < 4; i++) {
            int candRoom = currRoom + dx[i] * width + dy[i];
            if (candRoom == nextRoom) return i;
        }
        assert(false);
        return -1;
    }

public:
    GameMaze(int tex_count, int w = 3, int h = 3) : width(w), height(h) {
        generateRooms(tex_count);

        auto enemy_tex = ResourceManager::GetTexture("bowser");
        enemies.emplace_back(0, PLAYER_SIZE, enemy_tex);
    }

    void Draw(SpriteRenderer &renderer) {
        for (auto &room : rooms)
            room.Draw(renderer);
        for (auto &room : rooms)
            room.DrawAddons(renderer);

        for (auto &enemy : enemies)
            enemy.Draw(renderer);
    }

    void moveEnemy(int targetRoom, const GameObject &player, float velocity) {
        auto move_towards_target = [&velocity](GameObject &object, const glm::vec2 &targetPos) {
            auto &currPos = object.Position;
            std::vector<int> indices = {0, 1};
            if (rand() % 2) indices = {1, 0};

            bool moved = false;
            for (const auto &i : indices) {
                auto diff = std::abs(currPos[i] - targetPos[i]);
                if (diff < 3) continue;

                moved = true;
                float sign = currPos[i] > targetPos[i] ? -1 : 1;
                currPos[i] += velocity * sign;
                break;
            }
            if (not moved) {
                std::cout << "Nope" << std::endl;
            }
        };

        const auto &playerPos = player.Position;

        for (auto &enemy : enemies) {
            const auto &currRoom = enemy.currRoom;

            if (targetRoom == currRoom) {
                move_towards_target(enemy, playerPos);
                continue;
            }

            auto nextRoom = floydWarshall[currRoom][targetRoom].first;
            auto &currRoomObj = rooms[currRoom];
            int currDoorIndex = doorIndex(currRoom, nextRoom);
            auto currDoorPosition = currRoomObj.getDoorPosition(currDoorIndex);

            auto nextRoomObj = rooms[nextRoom];
            auto nextDoorIndex = doorIndex(nextRoom, currRoom);

            auto currDoorAllows = currRoomObj.doorAllowsObject(enemy, currDoorIndex);
            auto nextDoorAllows = nextRoomObj.doorAllowsObject(enemy, nextDoorIndex);

            std::cout << currDoorIndex << std::endl;

            if (not currDoorAllows) {
                if (nextDoorAllows) {
                    enemy.currRoom = nextRoom;
                } else {
                    auto oldPosition = enemy.Position;
                    move_towards_target(enemy, currDoorPosition);
                    auto newPosition = enemy.Position;
//                    assert(oldPosition != newPosition);
                    continue;
                }
            }

            auto nextDoorPosition = nextRoomObj.getDoorPosition(nextDoorIndex);
            move_towards_target(enemy, nextDoorPosition);
        }
    }

    void moveAll(const glm::vec2 &displace) {
        for (auto &room : rooms) {
            room.moveAll(displace);
        }
    }

    int base_room_idx() {
        auto[cx, cy] = base_room_coordinate();
        return room_index(cx, cy);
    }

    glm::vec2 base_room_center_position() {
        int idx = base_room_idx();
        return rooms[idx].Position + rooms[idx].Size / 2.0f;
    }

    int find_player_room(const GameObject &player) {
        int idx = -1;
        for (auto &room : rooms) {
            idx++;

            if (checkInside(room, player)) return idx;
            if (checkOutside(room, player)) continue;

            if (room.doorAllowsObject(player)) return idx;
        }

        return -1;
    }
};


#endif //ASSIGNMENT_GAME_MAZE_H
