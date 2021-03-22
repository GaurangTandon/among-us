#ifndef ASSIGNMENT_GAME_MAZE_H
#define ASSIGNMENT_GAME_MAZE_H

#include <stack>
#include <vector>
#include <utility>
#include <iostream>
#include <bitset>
#include "game_room.h"
#include "resource_manager.h"


// Do we want to store powerups etc. as part of rooms or inside maze only?
class GameMaze {
    static constexpr int dx[4] = {-1, 1, 0, 0};
    static constexpr int dy[4] = {0, 0, -1, 1};
    static constexpr int rev_idx[4] = {1, 0, 3, 2};

private:
    std::vector<GameRoom> rooms;
    int width, height;

    [[nodiscard]] std::vector<std::bitset<4>> generateTree(int seed = 0) const {
#define room_index(x, y) (x * width + y)

        srand(seed);
        using PII = std::pair<int, int>;

        std::vector<std::vector<bool>> visited(width, std::vector<bool>(height, false));
        std::stack<PII> st;

        visited[0][0] = true;
        st.push({0, 0});

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

            auto &[idx, c] = neighbours[rand() % neighbours.size()];
            auto &[cx, cy] = c;

            visited[cx][cy] = true;
            st.push({cx, cy});

            door_data[node_idx][idx] = true;
            door_data[room_index(cx, cy)][rev_idx[idx]] = true;
        }

        return door_data;
    }

    void generateRooms(int texture_count) {
        int nodes = width * height;
        auto treeData = generateTree();
        rooms.reserve(nodes);

        auto base = glm::vec2(100.0f, 100.0f);

        for (int room_idx = 0; room_idx < nodes; room_idx++) {
            int row = room_idx / width, col = room_idx % width;

            glm::vec2 offset(float(col) * GameRoom::SIZE[0], float(row) * GameRoom::SIZE[1]);

            auto position = base + offset;

            // TODO: load different roomsprite based on position
            auto roomSprite = ResourceManager::GetTexture("room" + std::to_string(room_idx % texture_count));
            rooms.emplace_back(position, roomSprite, treeData[room_idx]);
        }
    }

public:
    GameMaze(int tex_count, int w = 3, int h = 3) : width(w), height(h) {
        generateRooms(tex_count);
    }

    void Draw(SpriteRenderer &renderer) {
        for (auto &room : rooms)
            room.Draw(renderer);
        for (auto &room : rooms)
            room.DrawAddons(renderer);
    }

    void moveAll(const glm::vec2 &displace) {
        for (auto &room : rooms) {
            room.moveAll(displace);
        }
    }

    glm::vec2 base_room_center_position() {
        return rooms[0].Position + rooms[0].Size / 2.0f;
    }

    bool invalid_player_pos(const GameObject &player) {
        for (auto &room : rooms) {
            if (checkInside(room, player)) return false;
            if (checkOutside(room, player)) continue;

            if (room.doorAllowsObject(player)) return false;
        }

        return true;
    }
};


#endif //ASSIGNMENT_GAME_MAZE_H
