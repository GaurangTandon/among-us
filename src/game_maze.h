#ifndef ASSIGNMENT_GAME_MAZE_H
#define ASSIGNMENT_GAME_MAZE_H

#include <stack>
#include <vector>
#include <utility>
#include <bitset>
#include "game_room.h"
#include "resource_manager.h"

// Do we want to store powerups etc. as part of rooms or inside maze only?
class GameMaze {
private:
    std::vector<GameRoom> rooms;
    int width, height;

    std::vector<std::bitset<4>> generateTree(int seed = 0) {
        srand(seed);
        using PII = std::pair<int, int>;
        int dx[4] = {-1, 1, 0, 0};
        int dy[4] = {0, 0, -1, 1};

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

            int node_idx = ny * width + nx;
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
        }

        return door_data;
    }

    void generateRooms() {
        int nodes = width * height;
        auto treeData = generateTree();
        rooms.reserve(nodes);

        auto base = glm::vec2(0.0f, 0.0f);

        for (int room_idx = 0; room_idx < nodes; room_idx++) {
            float row = room_idx / width, col = room_idx % width;

            glm::vec2 offset(col * GameRoom::SIZE[0], row * GameRoom::SIZE[1]);

            auto position = base + offset;

            auto roomSprite = ResourceManager::GetTexture("room");
            rooms.emplace_back(position, roomSprite, treeData[room_idx]);
        }
    }

public:
    GameMaze(int w = 3, int h = 3) : width(w), height(h) {
        generateRooms();
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
};


#endif //ASSIGNMENT_GAME_MAZE_H