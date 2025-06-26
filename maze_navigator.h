#ifndef MAZE_NAVIGATOR_H
#define MAZE_NAVIGATOR_H

#include "maze.h"
#include <vector>
#include <utility>
#include <set>

class MazeNavigator {
public:
    explicit MazeNavigator(const maze::Maze& maze);
    std::pair<std::vector<std::pair<int, int>>, int> findPath(); // 返回路径和总价值

private:
    maze::Maze maze_;
    int total_value_ = 0;

    std::pair<int, int> findStart() const;
    std::pair<int, int> findExit() const;
    std::vector<std::pair<int, int>> getNeighbors(const std::pair<int, int>& pos) const;
    std::vector<std::pair<int, int>> bfsToTarget(
            const std::pair<int, int>& start,
            const std::pair<int, int>& target) const;
    std::pair<int, int> findBestTarget(
            const std::pair<int, int>& current,
            const std::set<std::pair<int, int>>& collected) const;
    void collectResource(const std::pair<int, int>& pos);
};

#endif