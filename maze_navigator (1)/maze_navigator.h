#ifndef MAZE_NAVIGATOR_H
#define MAZE_NAVIGATOR_H

#include "maze.h"
#include <vector>
#include <utility>
#include <map>
#include <set>
#include <queue>
#include <unordered_map>
#include <functional>

class MazeNavigator {
public:
    using Position = std::pair<int, int>;

    explicit MazeNavigator(const maze::Maze& maze);
    std::pair<std::vector<Position>, int> findPath();

private:
    // 迷宫状态
    maze::Maze maze_;
    std::map<Position, maze::Node> memory_map_;
    std::set<Position> collected_bl_;
    int total_value_ = 0;
    bool exit_found_ = false;

    // 优先级定义
    struct Target {
        Position pos;
        int priority;
        int distance;
        bool requires_trap;

        bool operator<(const Target& other) const {
            if (priority != other.priority)
                return priority < other.priority;
            if (requires_trap != other.requires_trap)
                return requires_trap;
            return distance > other.distance;
        }
    };

    // 核心方法
    void updateMemory(const Position& pos);
    void collectResource(const Position& pos);
    std::vector<Position> getNeighbors(const Position& pos, bool include_traps = false) const;
    std::vector<Position> findPathToTarget(const Position& start, const Position& target, bool allow_traps) const;

    // 决策逻辑
    bool allBLCollected() const;
    Position findHighestPriorityTarget(const Position& current) const;
    std::vector<Target> findAllPotentialTargets(const Position& current) const;
    bool isCriticalPosition(const Position& pos) const;
};

#endif
