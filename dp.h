//
// Created by admin on 2025/6/24.
//

#ifndef DP_H
#define DP_H
#include <vector>
#include <queue>
#include <set>

#include "maze.h"

namespace dp{
    using Maze = maze::Maze;
    static constexpr std::pair<int, int> direction[] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    class DP {
        const Maze maze;
        bool finish;
        int W;
        const int n, m;
        std::vector<std::pair<int, int> > arrive;
        std::vector<std::pair<int, int> > path;
        std::vector<int> blockVal{0, 0};
        std::vector<std::vector<int> > belong;
        std::vector<std::set<int>> nxt;
        std::pair<int, int> start{-1, -1}, end{-1, -1};

        bool isTree() const;
        bool treeDP();
        bool stateDP();

    public:
        explicit DP(const Maze &maze) : maze(maze), finish(false), W(0), n(static_cast<int>(maze.size())),
                                        m(static_cast<int>(maze[0].size())), belong(n, std::vector(m, -1)) {
            assert(n > 0 && m > 0);
            assert(!maze.empty());
        }

        bool run();
        [[nodiscard]] std::vector<std::pair<int, int> > getPath() const;
        [[nodiscard]] int getValue() const;
    };
} // dp

#endif //DP_H
