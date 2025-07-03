//
// Created by admin on 2025/7/4.
//

#ifndef GREEDYRUNNER_H
#define GREEDYRUNNER_H

#include <deque>

#include "maze.h"

namespace greedy {

    class GreedyRunner {
        using Maze = maze::Maze;

    private:
        const Maze originMaze;
        std::vector<std::pair<int, int> > path;
        std::vector<std::pair<int, int> > point;
        std::vector<std::vector<std::pair<int, int> > > fa;
        std::vector<std::vector<int> > dep;
        int w;
        bool finish;
        void buildTree(int x, int y);
        void genPath(int x, int y, int u, int v, std::deque<std::pair<int, int>> &ans);

    public:
        explicit GreedyRunner(const Maze &maze) : originMaze(maze),
                                        fa(std::vector(maze.size(), std::vector(maze.size(), std::pair(-1, -1)))),
                                        dep(std::vector(maze.size(), std::vector(maze.size(), 0))),
                                        w(0),
                                        finish(false) {
        }

        bool run();

        [[nodiscard]] std::vector<std::pair<int, int> > getPath() const;

        [[nodiscard]] int getW() const;
    };

} // greedy

#endif //GREEDYRUNNER_H
