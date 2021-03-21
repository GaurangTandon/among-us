#ifndef ASSIGNMENT_GAME_MAZE_H
#define ASSIGNMENT_GAME_MAZE_H

#include <stack>
#include <vector>
#include <utility>
#include <bitset>
#include "game_room.h"

// Do we want to store powerups etc. as part of rooms or inside maze only?
class GameMaze {
private:
    std::vector<GameRoom> rooms;

    std::vector<std::bitset<4>> generateTree(int width, int height, int seed = 0) {
        srand(seed);
        using PII = std::pair<int, int>;
        int dx[4] = {-1, 1, 0, 0};
        int dy[4] = {0, 0, -1, 1};

        std::vector<std::vector<bool>> visited(width, std::vector<bool>(height, 0));
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

    void generateRooms(int width = 3, int height = 3) {
        int nodes = width * height;
        auto treedata = generateTree(width, height);
        rooms.reserve(nodes);

        auto base = glm::vec2(0.0f, 0.0f);
        int room_idx = -1;

        Texture2D roomSprite;

        for (auto &room : rooms) {
            room_idx++;
            int row = room_idx / width, col = room_idx % width;

            glm::vec2 offset((col - 1) * 1, (row - 1) * 1);

            auto position = base + offset;

            room = GameRoom(position, roomSprite, treedata[room_idx]);
        }
    }

public:
    void Draw(SpriteRenderer &renderer) {
        for (auto &room : rooms) {
            room.Draw(renderer);
        }
    }
};


#endif //ASSIGNMENT_GAME_MAZE_H
