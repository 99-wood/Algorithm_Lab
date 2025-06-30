#include <bits/stdc++.h>
#include "maze.h"
#include "dp.h"
#include "puzzle_solver.h"
#include "boss_strategy.h"
#include "maze_navigator.h"

using Json = nlohmann::json;
using namespace std;
using Maze = maze::Maze;
bool solve() {
    int n = 15;
    const Maze maze = maze::genMaze(n);
    const Json json = mazeToJson(maze);
    maze::printJson(json, "../Test_Data/first/maze2.json");
    n = maze.size();
    for (const auto &row: maze) {
        cout << '[';
        for (const auto &x: row) {
            cout << "\"";
            cout << x << ", " << x.value << "\", ";
        }
        cout << "]";
        cout << endl;
    }
    dp::DP dpRuner(maze);
    dpRuner.run();
    cout << dpRuner.getValue() << endl;
    const auto dpPath = dpRuner.getPath();
    vector vis(n, vector(n, 0));
    int w = 0;
    // cout << dpPath.size() << endl;
    for (const auto [x, y]: dpPath) {
        if (!vis[x][y]) w += maze[x][y].value;
//        cout << x + 1 << " " << y + 1 << " " << w << endl;
        vis[x][y] = true;
    }
    assert(w == dpRuner.getValue());
    w -= maze::LVAL + maze::BVAL;
    cout << w << endl;
//    for(auto [x, y] : dpPath) cout << x + 1 << " " << y + 1 << endl;
    cout << "------------------------------------------------------------------" << endl;
    const auto [bossHPs, skills] = guess::loadBossBattleData("../test.json");

    guess::BossStrategy strategy;
    strategy.init(skills, bossHPs);

    std::cout << " Executing Branch and Bound...\n";
    const std::vector<int> plan = strategy.findOptimalSequence(false);
    for (const auto &step: plan) {
        std::cout << step << "\n";
    }
    cout << "------------------------------------------------------------------" << endl;
    auto [hash, clues] = guess::PuzzleSolver::loadPuzzleData("../test.json");
    guess::PuzzleSolver solver(hash, clues);
    solver.solve();
    const string pwd = solver.ans;
    if (!pwd.empty())
        std::cout << "password: " << pwd << " tries: " << solver.tries << std::endl;
    else
        std::cout << "can not match password" << std::endl;
    cout << "------------------------------------------------------------------" << endl;
    MazeNavigator greedyRunner(maze);
    auto [greedyPath, greedyW] = greedyRunner.findPath();
    cout << greedyW << endl;
    // cout << greedyPath.size() << endl;
    w = 0;
    vector vis_(n, vector(n, false));
    for (const auto [x, y]: greedyPath) {
        if (!vis_[x][y]) w += maze[x][y].value;
//        cout << x + 1 << " " << y + 1 << " " << w << endl;
        vis_[x][y] = true;
    }
    assert(w == greedyW);
    w -= maze::LVAL + maze::BVAL;
    cout << w << endl;
    return true;
}
int main() {
    int t = 1;
    while(t--) solve();
    return 0;
}