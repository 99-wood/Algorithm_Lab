#include "maze_navigator.h"
#include <climits>
#include <iostream>
#include <algorithm>

const std::map<maze::NodeType, int> PRIORITY_WEIGHTS = {
        {maze::NodeType::B, 1000},  // Boss最高优先级
        {maze::NodeType::L, 1000},  // 机关同Boss
        {maze::NodeType::G, 50},    // 金币
        {maze::NodeType::E, 10},    // 出口
        {maze::NodeType::R, 1},     // 通路
        {maze::NodeType::T, -10},   // 陷阱
        {maze::NodeType::S, -100},  // 起点
        {maze::NodeType::W, -1000}  // 墙
};

MazeNavigator::MazeNavigator(const maze::Maze& maze) : maze_(maze) {
    for (const auto& row : maze_) {
        for (const auto& node : row) {
            if (PRIORITY_WEIGHTS.find(node.nodeType)  == PRIORITY_WEIGHTS.end())  {
                throw std::runtime_error("Invalid node type in maze");
            }
        }
    }
}

void MazeNavigator::updateMemory(const Position& pos) {
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            int x = pos.first  + dx;
            int y = pos.second  + dy;
            if (x >= 0 && x < static_cast<int>(maze_.size()) &&
                y >= 0 && y < static_cast<int>(maze_[0].size())) {
                memory_map_[{x, y}] = maze_[x][y];
                if (maze_[x][y].nodeType == maze::NodeType::E) {
                    exit_found_ = true;
                }
            }
        }
    }
}

void MazeNavigator::collectResource(const Position& pos) {
    auto& node = maze_[pos.first][pos.second];
    total_value_ += node.value;

    if (node.nodeType == maze::NodeType::B || node.nodeType == maze::NodeType::L) {
        collected_bl_.insert(pos);
    }

    // 收集后标记为通路
    node.nodeType = maze::NodeType::R;
    node.value = 0;
    memory_map_[pos] = node; // 更新记忆
}

std::vector<MazeNavigator::Position> MazeNavigator::getNeighbors(
        const Position& pos, bool include_traps) const {

    std::vector<Position> neighbors;
    const int dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};

    for (const auto& dir : dirs) {
        int x = pos.first  + dir[0];
        int y = pos.second  + dir[1];
        if (x >= 0 && x < static_cast<int>(maze_.size()) &&
            y >= 0 && y < static_cast<int>(maze_[0].size())) {
            auto nodeType = maze_[x][y].nodeType;
            if (nodeType != maze::NodeType::W &&
                (include_traps || nodeType != maze::NodeType::T)) {
                neighbors.emplace_back(x,  y);
            }
        }
    }
    return neighbors;
}

std::vector<MazeNavigator::Position> MazeNavigator::findPathToTarget(
        const Position& start, const Position& target, bool allow_traps) const {

    std::queue<Position> q;
    std::unordered_map<int, std::unordered_map<int, bool>> visited;
    std::unordered_map<int, std::unordered_map<int, Position>> parent;

    q.push(start);
    visited[start.first][start.second] = true;

    while (!q.empty())  {
        auto p = q.front();
        q.pop();

        if (p == target) break;

        for (const auto& neighbor : getNeighbors(p, allow_traps)) {
            if (!visited[neighbor.first][neighbor.second]) {
                visited[neighbor.first][neighbor.second] = true;
                parent[neighbor.first][neighbor.second] = p;
                q.push(neighbor);
            }
        }
    }

    std::vector<Position> path;
    if (visited[target.first][target.second]) {
        Position p = target;
        while (p != start) {
            path.push_back(p);
            p = parent[p.first][p.second];
        }
        std::reverse(path.begin(),  path.end());
    }
    return path;
}

bool MazeNavigator::allBLCollected() const {
    for (const auto& [pos, node] : memory_map_) {
        if ((node.nodeType  == maze::NodeType::B ||
             node.nodeType  == maze::NodeType::L) &&
            !collected_bl_.count(pos)) {
            return false;
        }
    }
    return true;
}

bool MazeNavigator::isCriticalPosition(const Position& pos) const {
    if (!memory_map_.count(pos)) return false;
    auto nodeType = memory_map_.at(pos).nodeType;
    return (nodeType == maze::NodeType::B ||
            nodeType == maze::NodeType::L ||
            nodeType == maze::NodeType::E);
}

std::vector<MazeNavigator::Target> MazeNavigator::findAllPotentialTargets(
        const Position& current) const {
    std::vector<Target> targets;

    // 第一阶段：优先搜索记忆中的 B/L（优先非陷阱路径）
    for (const auto& [pos, node] : memory_map_) {
        if ((node.nodeType == maze::NodeType::B || node.nodeType == maze::NodeType::L) &&
            !collected_bl_.count(pos)) {
            auto path = findPathToTarget(current, pos, false);
            if (!path.empty()) {
                targets.push_back({pos, PRIORITY_WEIGHTS.at(node.nodeType),
                                   static_cast<int>(path.size()), false});
            } else {
                path = findPathToTarget(current, pos, true);
                if (!path.empty()) {
                    targets.push_back({pos, PRIORITY_WEIGHTS.at(node.nodeType),
                                       static_cast<int>(path.size()), true});
                }
            }
        }
    }

    // 第二阶段：如果没有 B/L 目标，优先收集金币（严格禁用陷阱）
    if (targets.empty()) {
        for (const auto& [pos, node] : memory_map_) {
            if (node.nodeType == maze::NodeType::G) {
                auto path = findPathToTarget(current, pos, false);
                if (!path.empty()) {
                    targets.push_back({pos, PRIORITY_WEIGHTS.at(node.nodeType),
                                       static_cast<int>(path.size()), false});
                }
            }
        }

        // 如果没有金币可收集，再处理出口（允许陷阱）
        if (targets.empty() && allBLCollected()) {
            for (const auto& [pos, node] : memory_map_) {
                if (node.nodeType == maze::NodeType::E) {
                    auto path = findPathToTarget(current, pos, true);
                    if (!path.empty()) {
                        targets.push_back({pos, PRIORITY_WEIGHTS.at(node.nodeType),
                                           static_cast<int>(path.size()), true});
                    }
                }
            }
        }
    }

    // 第三阶段：探索未走过的空格（R）或可扩展视野的通路
    if (targets.empty()) {
        // 1. 探索已走过的通路中，周围仍有未记忆区域的节点
        for (const auto& [pos, node] : memory_map_) {
            if (node.nodeType == maze::NodeType::R && !isCriticalPosition(pos)) {
                auto neighbors = getNeighbors(pos, true);
                bool has_unexplored = false;
                for (const auto& neighbor : neighbors) {
                    if (!memory_map_.count(neighbor)) {
                        has_unexplored = true;
                        break;
                    }
                }

                if (has_unexplored) {
                    auto path = findPathToTarget(current, pos, true);
                    if (!path.empty()) {
                        targets.push_back({pos, 5, static_cast<int>(path.size()), true});
                    }
                }
            }
        }

        // 2. 如果仍无目标，尝试全局搜索未记忆的通路
        if (targets.empty()) {
            for (size_t i = 0; i < maze_.size(); ++i) {
                for (size_t j = 0; j < maze_[i].size(); ++j) {
                    Position pos(i, j);
                    if (maze_[i][j].nodeType == maze::NodeType::R &&
                        !memory_map_.count(pos)) {
                        auto path = findPathToTarget(current, pos, true);
                        if (!path.empty()) {
                            targets.push_back({pos, 5, static_cast<int>(path.size()), true});
                        }
                    }
                }
            }
        }
    }

    // 第四阶段：全局搜索未记忆的金币和 B/L（优先金币，再 B/L，最后出口）
    if (targets.empty()) {
        // 1. 全局搜索未记忆的金币（禁用陷阱）
        for (size_t i = 0; i < maze_.size(); ++i) {
            for (size_t j = 0; j < maze_[i].size(); ++j) {
                Position pos(i, j);
                if (maze_[i][j].nodeType == maze::NodeType::G &&
                    !memory_map_.count(pos)) {
                    auto path = findPathToTarget(current, pos, false);
                    if (!path.empty()) {
                        targets.push_back({pos, PRIORITY_WEIGHTS.at(maze_[i][j].nodeType),
                                           static_cast<int>(path.size()), false});
                    }
                }
            }
        }

        // 2. 再次尝试收集 memory_map_ 中未被收集的金币（允许陷阱）
        for (const auto& [pos, node] : memory_map_) {
            if (node.nodeType == maze::NodeType::G) {
                auto path = findPathToTarget(current, pos, true);
                if (!path.empty()) {
                    targets.push_back({pos, PRIORITY_WEIGHTS.at(node.nodeType),
                                       static_cast<int>(path.size()), true});
                }
            }
        }

        // 3. 全局搜索未记忆的 B/L（强制允许陷阱）
        if (targets.empty()) {
            for (size_t i = 0; i < maze_.size(); ++i) {
                for (size_t j = 0; j < maze_[i].size(); ++j) {
                    Position pos(i, j);
                    auto nodeType = maze_[i][j].nodeType;
                    if ((nodeType == maze::NodeType::B || nodeType == maze::NodeType::L) &&
                        !collected_bl_.count(pos)) {
                        auto path = findPathToTarget(current, pos, true);
                        if (!path.empty()) {
                            targets.push_back({pos, PRIORITY_WEIGHTS.at(nodeType),
                                               static_cast<int>(path.size()), true});
                        }
                    }
                }
            }
        }

        // 4. 如果所有 B/L 已收集但出口未找到，再处理出口（允许陷阱）
        if (targets.empty() && allBLCollected() && !exit_found_) {
            for (size_t i = 0; i < maze_.size(); ++i) {
                for (size_t j = 0; j < maze_[i].size(); ++j) {
                    Position pos(i, j);
                    auto nodeType = maze_[i][j].nodeType;
                    if (nodeType == maze::NodeType::E) {
                        auto path = findPathToTarget(current, pos, true);
                        if (!path.empty()) {
                            targets.push_back({pos, PRIORITY_WEIGHTS.at(nodeType),
                                               static_cast<int>(path.size()), true});
                        }
                    }
                }
            }
        }
    }

    return targets;
}

MazeNavigator::Position MazeNavigator::findHighestPriorityTarget(
        const Position& current) const {

    auto targets = findAllPotentialTargets(current);
    if (!targets.empty())  {
        std::priority_queue<Target> pq;
        for (const auto& t : targets) {
            pq.push(t);
        }
        return pq.top().pos;
    }
    return {-1, -1};
}

std::pair<std::vector<MazeNavigator::Position>, int> MazeNavigator::findPath() {
    std::vector<Position> path;
    Position current{-1, -1};

    // 初始化起点
    for (size_t i = 0; i < maze_.size(); ++i) {
        for (size_t j = 0; j < maze_[i].size(); ++j) {
            if (maze_[i][j].nodeType == maze::NodeType::S) {
                current = {static_cast<int>(i), static_cast<int>(j)};
                break;
            }
        }
    }
    if (current.first == -1) throw std::runtime_error("Maze has no start point");

    path.push_back(current);
    updateMemory(current);

    const int max_steps = maze_.size() * maze_[0].size() * 3;
    int step_count = 0;

    while (step_count++ < max_steps) {
        auto next = findHighestPriorityTarget(current);
        if (next.first == -1) break;

        bool allow_traps = false;
        auto targets = findAllPotentialTargets(current);
        for (const auto& t : targets) {
            if (t.pos == next && t.requires_trap) {
                allow_traps = true;
                break;
            }
        }

        auto path_segment = findPathToTarget(current, next, allow_traps);
        if (path_segment.empty()) {
            std::cerr << "Warning: No path to target (" << next.first << "," << next.second << ")\n";
            break;
        }

        // ✅ 修改点：路径段中每个节点都调用 collectResource
        for (const auto& pos : path_segment) {
            path.push_back(pos);
            updateMemory(pos);
            collectResource(pos); // ✅ 每个节点都触发 collectResource
        }

        current = next;

        // 终止条件
        if (maze_[current.first][current.second].nodeType == maze::NodeType::E && allBLCollected()) {
            break;
        }
    }

    // 验证结果
    if (!allBLCollected()) {
        std::cerr << "Warning: Not all B/L collected!\n";
        for (const auto& [pos, node] : memory_map_) {
            if ((node.nodeType == maze::NodeType::B || node.nodeType == maze::NodeType::L) &&
                !collected_bl_.count(pos)) {
                std::cerr << "  Missing " << node.nodeType
                          << " at (" << pos.first << "," << pos.second << ")\n";
            }
        }
    }

    return {path, total_value_};
}