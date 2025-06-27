//
// Created by admin on 2025/6/24.
//

#include "dp.h"

namespace dp{
    bool DP::isTree() const {
        std::vector vis(n, std::vector(m, false));
        const auto enable = [&](const int x, const int y) {
            return x >= 0 && x < n && y >= 0 && y < m && maze[x][y].nodeType != maze::NodeType::W;
        };
        // 判断是否有环
        const auto dfs = [&](auto &&self, const int x, const int y, const int lastx, const int lasty) {
            if(vis[x][y]) return true;
            vis[x][y] = true;
            for(const auto [dx, dy] : direction){
                const int u = x + dx, v = y + dy;
                if(enable(u, v) && !(u == lastx && v == lasty)){
                    if(self(self, u, v, x, y)) return true;
                }
            }
            return false;
        };
        return !dfs(dfs, start.first, start.second, -1, -1);
    }

    bool DP::treeDP() {
        std::vector must(n, std::vector(m, false));
        std::vector f(n, std::vector(m, 0));
        const auto enable = [&](const int x, const int y) {
            return x >= 0 && x < n && y >= 0 && y < m && maze[x][y].nodeType != maze::NodeType::W;
        };
        const auto isEnd = [&](const int x, const int y) {
            assert(enable(x, y));
            return maze[x][y].nodeType == maze::NodeType::E;
        };
        const auto dfs = [&](auto &&self, const int x, const int y, const int lastx, const int lasty) {
            assert(enable(x, y));
            if(isEnd(x, y)){
                must[x][y] = true;
                return;
            }
            f[x][y] += maze[x][y].value;
            for(const auto [dx, dy] : direction){
                const int u = x + dx, v = y + dy;
                if(enable(u, v) && !(u == lastx && v == lasty)){
                    self(self, u, v, x, y);
                    if(must[u][v]){
                        must[x][y] = true;
                        f[x][y] += f[u][v];
                    }
                }
            }
            for(const auto [dx, dy] : direction){
                const int u = x + dx, v = y + dy;
                if(enable(u, v) && !(u == lastx && v == lasty)){
                    if(must[u][v]) continue;
                    f[x][y] += std::max(0, f[u][v]);
                }
            }
            return;
        };
        const auto [x, y] = start;
        dfs(dfs, x, y, -1, -1);
        assert(must[x][y]);
        const std::function<void(int, int, int, int)> genPath = [&](const int x, const int y, const int lastx, const int lasty) {
            assert(enable(x, y));
            path.emplace_back(x, y);
            if(must[x][y]){
                for(const auto [dx, dy] : direction){
                    const int u = x + dx, v = y + dy;
                    if(enable(u, v) && !(u == lastx && v == lasty) && !must[u][v]){
                        if(f[u][v] > 0){
                            genPath(u, v, x, y);
                            path.emplace_back(x, y);
                        }
                    }
                }
                for(const auto [dx, dy] : direction){
                    const int u = x + dx, v = y + dy;
                    if(enable(u, v) && !(u == lastx && v == lasty) && must[u][v]){
                        genPath(u, v, x, y);
                    }
                }
            }
            else{
                for(const auto [dx, dy] : direction){
                    const int u = x + dx, v = y + dy;
                    if(enable(u, v) && !(u == lastx && v == lasty) && !must[u][v]){
                        if(f[u][v] > 0){
                            genPath(u, v, x, y);
                            path.emplace_back(x, y);
                        }
                    }
                }
            }
        };
        W = f[x][y];
        genPath(x, y, -1, -1);
        return true;
    }

    bool DP::stateDP() {
    }

    bool DP::run() {
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < m; ++j){
                const auto &[type, val] = maze[i][j];
                if(type == maze::NodeType::S){
                    assert(start.first == -1 && start.second == -1);
                    start = {i, j};
                }
                if(type == maze::NodeType::E){
                    assert(end.first == -1 && end.second == -1);
                    end = {i, j};
                }
            }
        }
        assert(start != std::pair(-1, -1));
        assert(end != std::pair(-1, -1));
        const auto enable = [&](const int x, const int y) {
            return x >= 0 && x < n && y >= 0 && y < m && maze[x][y].nodeType != maze::NodeType::W && maze[x][y].nodeType
                   != maze::NodeType::T;
        };
        const auto bfs0 = [&](const int sx, const int sy, const int cnt) {
            assert(belong[sx][sy] == -1 && enable(sx, sy));
            belong[sx][sy] = cnt;
            blockVal[cnt] = maze[sx][sy].value;
            std::queue<std::pair<int, int> > q;
            q.emplace(sx, sy);
            while(!q.empty()){
                const auto [x, y] = q.front();
                q.pop();
                for(const auto [dx, dy] : direction){
                    const int u = x + dx, v = y + dy;
                    if(enable(u, v) && belong[u][v] == -1 && maze[u][v].nodeType != maze::NodeType::E){
                        belong[u][v] = cnt;
                        blockVal[cnt] += maze[u][v].value;
                        q.emplace(u, v);
                    }
                }
            }
        };
        bfs0(start.first, start.second, 0);
        belong[end.first][end.second] = 1;
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < m; ++j){
                if(enable(i, j) && belong[i][j] == -1){
                    blockVal.emplace_back(0);
                    bfs0(i, j, static_cast<int>(blockVal.size()) - 1);
                }
                else if(maze[i][j].nodeType == maze::NodeType::T){
                    belong[i][j] = -2;
                }
            }
        } {
            // for(int i = 0; i < n; ++i){
            //     for(int j = 0; j < m; ++j){
            //         std::cout << std::setw(4) << belong[i][j] << " ";
            //     }
            //     std::cout << std::endl;
            // }
        } // print belong
        nxt.resize(blockVal.size());
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < m; ++j){
            }
        }

        if(isTree()){
            std::cout << "Is tree\n";
            return finish = treeDP();
        }
        else{
            assert(0);
            return finish = stateDP();
        }
    }

    std::vector<std::pair<int, int>> DP::getPath() const {
        assert(finish);
        return path;
    }

    int DP::getValue() const {
        assert(finish);
        return W;
    }
} // dp
