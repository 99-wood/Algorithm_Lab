//
// Created by admin on 2025/7/2.
//

#include "SmartRunner.h"

#include <queue>
#include <set>

namespace smart{
    constexpr int INF = 1e8;
    constexpr double K = 1.5;
    constexpr double r = 0.85;
    constexpr std::pair<int, int> dir[] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};


    void SmartRunner::buildTree(int x, int y) {
        const int n = static_cast<int>(originMaze.size());
        for(auto [dx, dy] : dir){
            const int tx = x + dx, ty = y + dy;
            if(tx < 0 || tx >= n || ty < 0 || ty >= n || originMaze[tx][ty].nodeType == maze::NodeType::W || std::pair(tx, ty) == fa[x][y]) continue;
            fa[tx][ty] = std::pair(x, y);
            dep[tx][ty] = dep[x][y] + 1;
            buildTree(tx, ty);
        }
        return;
    }

    void SmartRunner::genPath(int x, int y, int u, int v, std::deque<std::pair<int, int>> &ans) {
        if(x == u && y == v){
            ans.emplace_back(x, y);
            return;
        }
        if(dep[x][y] > dep[u][v]){
            genPath(fa[x][y].first, fa[x][y].second, u, v, ans);
            ans.emplace_front(x, y);
        }
        else{
            genPath(x, y, fa[u][v].first, fa[u][v].second, ans);
            ans.emplace_back(u, v);
        }
        return;
    }

    bool SmartRunner::run() {
        const int n = static_cast<int>(originMaze.size());
        std::vector currentMaze(n, std::vector<maze::Node>(n));
        std::vector isPushed(n, std::vector<bool>(n));
        // std::vector vis(n, std::vector<bool>(n));
        bool findB = false, findL = false;
#define ratio (1.0 * (w - (findB ? maze::BVAL : 0) - (findL ? maze::LVAL : 0)) * K / (point.size()))
        auto getExpect = [&](int x, int y) -> double {
            std::vector vi(n, std::vector<bool>(n));
            std::priority_queue<std::pair<double, std::pair<int, int> > > q;
            assert(currentMaze[x][y].nodeType == maze::NodeType::T);
            double E = currentMaze[x][y].value;
            for(auto [dx, dy] : dir){
                const int tx = x + dx, ty = y + dy;
                if(tx < 0 || tx >= n || ty < 0 || ty >= n || vi[tx][ty] || isPushed[tx][ty] || currentMaze[tx][ty].
                   nodeType == maze::NodeType::W)
                    continue;
                if(currentMaze[tx][ty].nodeType == maze::NodeType::U){
                    vi[tx][ty] = true;
                    q.emplace(r, std::pair(tx, ty));
                }
                else{
                    vi[tx][ty] = true;
                    q.emplace(1, std::pair(tx, ty));
                }

                if(currentMaze[tx][ty].nodeType == maze::NodeType::S)
                    assert(0);
                if(currentMaze[tx][ty].nodeType == maze::NodeType::E || currentMaze[tx][ty].nodeType == maze::NodeType::B
                   || currentMaze[tx][ty].nodeType == maze::NodeType::L)
                    return INF;
            }
            if(q.empty()) return -INF;
            while(!q.empty()){
                const auto [d, p] = q.top();
                q.pop();
                x = p.first;
                y = p.second;
                assert(!isPushed[x][y]);
                if(currentMaze[x][y].nodeType == maze::NodeType::S)
                    assert(0);
                if(currentMaze[x][y].nodeType == maze::NodeType::W)
                    assert(0);
                if(currentMaze[x][y].nodeType == maze::NodeType::E || currentMaze[x][y].nodeType == maze::NodeType::B ||
                   currentMaze[x][y].nodeType == maze::NodeType::L)
                    assert(0);

                if(currentMaze[x][y].nodeType == maze::NodeType::T || currentMaze[x][y].nodeType == maze::NodeType::G)
                    E += currentMaze[x][y].value * d;
                if(currentMaze[x][y].nodeType == maze::NodeType::U) E += ratio * d;
                if(currentMaze[x][y].nodeType == maze::NodeType::R) E += 0 * d;
                for(auto [dx, dy] : dir){
                    const int tx = x + dx, ty = y + dy;
                    if(tx < 0 || tx >= n || ty < 0 || ty >= n || vi[tx][ty] || isPushed[tx][ty] || currentMaze[tx][ty].
                       nodeType == maze::NodeType::W)
                        continue;
                    if(currentMaze[tx][ty].nodeType == maze::NodeType::U){
                        vi[tx][ty] = true;
                        q.emplace(d * r, std::pair(tx, ty));
                    }
                    else if(d == 1){
                        vi[tx][ty] = true;
                        q.emplace(d * 1.0, std::pair(tx, ty));
                    }

                    if(currentMaze[tx][ty].nodeType == maze::NodeType::S)
                        assert(0);
                    if(currentMaze[tx][ty].nodeType == maze::NodeType::E || currentMaze[tx][ty].nodeType ==
                       maze::NodeType::B || currentMaze[tx][ty].nodeType == maze::NodeType::L){
                        if(d == 1) return INF;
                    }
                }
            }
            return E;
        };
        int x = -1, y = -1;
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < n; ++j){
                if(originMaze[i][j].nodeType == maze::NodeType::S){
                    x = i;
                    y = j;
                }
                currentMaze[i][j].nodeType = maze::NodeType::U;
            }
        }
        assert(x != -1 && y != -1);
        buildTree(x, y);
        path.emplace_back(x, y);
        for(int i = x - 1; i <= x + 1; ++i){
            for(int j = y - 1; j <= y + 1; ++j){
                if(0 <= i && i < n && 0 <= j && j < n) currentMaze[i][j] = originMaze[i][j];
            }
        }
        std::set<std::pair<int, int> > q;
        q.emplace(x, y);
        isPushed[x][y] = true;
        auto getPriority = [&](const int u, const int v) {
            if(u == -1 && v == -1) return -1;
            if(currentMaze[u][v].nodeType == maze::NodeType::W) return -2;
            if(currentMaze[u][v].nodeType == maze::NodeType::S) return 9;
            if(currentMaze[u][v].nodeType == maze::NodeType::G || currentMaze[u][v].nodeType == maze::NodeType::R ||
               currentMaze[u][v].nodeType == maze::NodeType::B || currentMaze[u][v].nodeType == maze::NodeType::L)
                return 8;
            if(currentMaze[u][v].nodeType == maze::NodeType::T) return 7;
            if(currentMaze[u][v].nodeType == maze::NodeType::E){
                if(findB && findL) return 7;
                else return 6;
            }
            assert(0);
        };

        auto less = [&](const int p, const int q, const int u, const int v) {
            if(currentMaze[u][v].nodeType == maze::NodeType::W) return false;
            if(p == -1 && q == -1){
                return true;
            }
            if(getPriority(p, q) < getPriority(u, v)) return true;
            if(getPriority(p, q) > getPriority(u, v)) return false;
            if(currentMaze[u][v].nodeType == maze::NodeType::T || currentMaze[u][v].nodeType == maze::NodeType::E){
                double wx = 0, wu = 0;
                if(currentMaze[p][q].nodeType == maze::NodeType::T) wx = getExpect(p, q);
                if(currentMaze[u][v].nodeType == maze::NodeType::T) wu = getExpect(u, v);
                return wx < wu;
            }
            else return std::abs(p - x) + std::abs(q - y) > std::abs(u - x) + std::abs(v - y);
        };
        while(!q.empty()){
            int tx = -1, ty = -1;
            for(auto [u, v] : q){
                if(less(tx, ty, u, v)){
                    tx = u;
                    ty = v;
                }
            }
            assert(tx != -1 && ty != -1);
            x = tx;
            y = ty;
            q.erase(std::pair(x, y));
            point.emplace_back(x, y);
            if(currentMaze[x][y].nodeType == maze::NodeType::E){
                assert(findB);
                assert(findL);
                break;
            }
            if(currentMaze[x][y].nodeType == maze::NodeType::B) findB = true;
            if(currentMaze[x][y].nodeType == maze::NodeType::L) findL = true;
            w += currentMaze[x][y].value;
            for(auto [dx, dy] : dir){
                const int tx = x + dx, ty = y + dy;
                if(tx < 0 || tx >= n || ty < 0 || ty >= n || isPushed[tx][ty]) continue;
                q.emplace(tx, ty);
                isPushed[tx][ty] = true;
            }
            for(int i = x - 1; i <= x + 1; ++i){
                for(int j = y - 1; j <= y + 1; ++j){
                    if(0 <= i && i < n && 0 <= j && j < n) currentMaze[i][j] = originMaze[i][j];
                }
            }
        }
        for(int i = 1; i < point.size(); ++i){
            std::deque<std::pair<int, int>> tmp;
            genPath(point[i - 1].first, point[i - 1].second, point[i].first, point[i].second, tmp);
            tmp.pop_front();
            while(!tmp.empty()){
                path.push_back(tmp.front());
                tmp.pop_front();
            }
        }
        finish = true;
        return true;
#undef ratio
    }

    std::vector<std::pair<int, int> > SmartRunner::getPath() const {
        assert(finish);
        return path;
    }

    int SmartRunner::getW() const {
        assert(finish);
        return w;
    }
} // smart
