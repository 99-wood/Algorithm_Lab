#include "maze_navigator.h"
#include <queue>
#include <unordered_map>
#include <climits>
#include <iostream>
#include <algorithm>

MazeNavigator::MazeNavigator(const maze::Maze& maze) : maze_(maze), total_value_(0) {}

void MazeNavigator::collectResource(const std::pair<int, int>& pos) {
    auto& node = maze_[pos.first][pos.second];
    switch(node.nodeType)  {
        case maze::NodeType::G: // 金币
            total_value_ += maze_[pos.first][pos.second].value;
            node.nodeType  = maze::NodeType::R; // 收集后变为通路
            break;
        case maze::NodeType::L: // 金币
            total_value_ += maze_[pos.first][pos.second].value;
            node.nodeType  = maze::NodeType::R; // 收集后变为通路
            break;
        case maze::NodeType::T: // 陷阱
            total_value_ += maze_[pos.first][pos.second].value;
            node.nodeType  = maze::NodeType::R; // 触发后变为通路
            break;
        case maze::NodeType::B: // Boss
            // Boss价值
            total_value_ += maze_[pos.first][pos.second].value;
            node.nodeType  = maze::NodeType::R; // 击败后变为通路
            break;
        default:
            break;
    }
}

std::pair<int, int> MazeNavigator::findStart() const {
    for (size_t i = 0; i < maze_.size(); ++i)
        for (size_t j = 0; j < maze_[i].size(); ++j)
            if (maze_[i][j].nodeType == maze::NodeType::S)
                return {i, j};
    return {-1, -1};
}

std::pair<int, int> MazeNavigator::findExit() const {
    for (size_t i = 0; i < maze_.size(); ++i)
        for (size_t j = 0; j < maze_[i].size(); ++j)
            if (maze_[i][j].nodeType == maze::NodeType::E)
                return {i, j};
    return {-1, -1};
}

std::vector<std::pair<int, int>> MazeNavigator::getNeighbors(const std::pair<int, int>& pos) const {
    std::vector<std::pair<int, int>> neighbors;
    const int dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};

    for (auto& dir : dirs) {
        int ni = pos.first  + dir[0];
        int nj = pos.second  + dir[1];

        if (ni >= 0 && ni < static_cast<int>(maze_.size()) &&
            nj >= 0 && nj < static_cast<int>(maze_[0].size()) &&
            maze_[ni][nj].nodeType != maze::NodeType::W) {
            neighbors.emplace_back(ni,  nj);
        }
    }
    return neighbors;
}

std::vector<std::pair<int, int>> MazeNavigator::bfsToTarget(
        const std::pair<int, int>& start,
        const std::pair<int, int>& target) const {

    std::queue<std::pair<int, int>> q;
    std::unordered_map<int, std::unordered_map<int, bool>> visited;
    std::unordered_map<int, std::unordered_map<int, std::pair<int, int>>> parent;

    q.push(start);
    visited[start.first][start.second] = true;

    while (!q.empty())  {
        auto p = q.front();
        q.pop();

        if (p == target) break;

        for (auto& neighbor : getNeighbors(p)) {
            if (!visited[neighbor.first][neighbor.second]) {
                visited[neighbor.first][neighbor.second] = true;
                parent[neighbor.first][neighbor.second] = p;
                q.push(neighbor);
            }
        }
    }

    std::vector<std::pair<int, int>> path;
    if (visited[target.first][target.second]) {
        auto p = target;
        while (p != start) {
            path.push_back(p);
            p = parent[p.first][p.second];
        }
        std::reverse(path.begin(),  path.end());
    }
    return path;
}

std::pair<int, int> MazeNavigator::findBestTarget(
        const std::pair<int, int>& current,
        const std::set<std::pair<int, int>>& collected) const {

    std::pair<int, int> best_target{-1, -1};
    int min_distance = INT_MAX;
    int max_value = INT_MIN;

    // 查找最有价值且可达的目标(G/T/B)
    for (size_t i = 0; i < maze_.size(); ++i) {
        for (size_t j = 0; j < maze_[i].size(); ++j) {
            if ((maze_[i][j].nodeType == maze::NodeType::G ||
                 maze_[i][j].nodeType == maze::NodeType::T ||
                 maze_[i][j].nodeType == maze::NodeType::B ||
                 maze_[i][j].nodeType == maze::NodeType::L) &&
                collected.find({i,  j}) == collected.end())  {

                auto path = bfsToTarget(current, {i, j});
                if (!path.empty())  {
                    int current_value = maze_[i][j].value;
//                    switch(maze_[i][j].nodeType) {
//                        case maze::NodeType::G: current_value = maze::GVAL; break;
//                        case maze::NodeType::T: current_value = maze::TVAL; break;
//                        case maze::NodeType::B: current_value = maze::BVAL; break; // Boss价值
//                        default: break;
//                    }

                    // 优先选择价值高且距离近的目标
                    if (current_value > max_value ||
                        (current_value == max_value && path.size()  < min_distance)) {
                        max_value = current_value;
                        min_distance = path.size();
                        best_target = {i, j};
                    }
                }
            }
        }
    }
    return best_target;
}

std::pair<std::vector<std::pair<int, int>>, int> MazeNavigator::findPath() {
    auto start = findStart();
    auto exit = findExit();
    std::vector<std::pair<int, int>> path;
    std::set<std::pair<int, int>> collected;

    // 第一阶段：收集所有有价值目标
    auto current = start;
    path.push_back(current);

    while (true) {
        auto target_pos = findBestTarget(current, collected);
        if (target_pos.first  == -1) break; // 没有更多目标

        auto path_to_target = bfsToTarget(current, target_pos);
        if (path_to_target.empty())  break; // 无法到达

        // 添加路径并收集资源
        path.insert(path.end(),  path_to_target.begin(),  path_to_target.end());
        collectResource(target_pos);
        collected.insert(target_pos);
        current = target_pos;
    }

    // 第二阶段：前往出口
    auto path_to_exit = bfsToTarget(current, exit);
    if (!path_to_exit.empty())  {
        path.insert(path.end(),  path_to_exit.begin(),  path_to_exit.end());
    }

    // 验证路径合法性
    for (size_t i = 1; i < path.size();  ++i) {
        int dx = abs(path[i].first - path[i-1].first);
        int dy = abs(path[i].second - path[i-1].second);
        if (dx + dy != 1) {
            std::cerr << "ERROR: Invalid move between ("
                      << path[i-1].first << "," << path[i-1].second
                      << ") and (" << path[i].first << "," << path[i].second << ")\n";
            return {{}, 0};
        }
        if (maze_[path[i].first][path[i].second].nodeType == maze::NodeType::W) {
            std::cerr << "ERROR: Path goes through wall at ("
                      << path[i].first << "," << path[i].second << ")\n";
            return {{}, 0};
        }
    }

    return {path, total_value_};
}