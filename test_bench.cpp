#include <bits/stdc++.h>
#include "maze.h"
#include "dp.h"
#include "puzzle_solver.h"
#include "boss_strategy.h"
#include "maze_navigator.h"
#include "SmartRunner.h"

using Json = nlohmann::json;
using namespace std;
using Maze = maze::Maze;
int getPathVal(const std::vector<std::pair<int, int>> &path, const Maze &maze) {
    const int n = maze.size();
    vector vis(n, vector(n, 0));
    int w = 0;
    for (const auto [x, y]: path) {
        if (!vis[x][y]) w += maze[x][y].value;
        //        cout << x + 1 << " " << y + 1 << " " << w << endl;
        vis[x][y] = true;
    }
    w -= maze::BVAL;
    w -= maze::LVAL;
    return w;
}
void printPath(const std::vector<std::pair<int, int>> &path, const Maze &maze){
    const int n = maze.size();
    vector vis(n, vector(n, 0));
    for (const auto [x, y]: path) {
        vis[x][y] = true;
    }
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            if(vis[i][j]) std::cout << "@ ";
            else std::cout << maze[i][j] << " ";
        }
        std::cout << std::endl;
    }
    return;
}

int sumDPVal = 0, sumGreedyVal = 0, sumSmartVal = 0;
// #define RUN_BOSS
// #define RUN_GUESS
bool solve() {
#ifdef RUN_BOSS
    cout << "------------------------------------------------------------------" << endl;
    const auto [bossHPs, skills] = boss::loadBossBattleData("../test.json");

    boss::BossStrategy strategy;
    strategy.init(skills, bossHPs);

    std::cout << " Executing Branch and Bound...\n";
    const std::vector<int> plan = strategy.findOptimalSequence(false);
    for (const auto &step: plan) {
        std::cout << step << "\n";
    }
#endif

#ifdef RUN_GUESS
    cout << "------------------------------------------------------------------" << endl;
    auto [hash, clues] = guess::PuzzleSolver::loadPuzzleData("../test.json");
    guess::PuzzleSolver solver(hash, clues);
    solver.solve();
    const string pwd = solver.ans;
    if (!pwd.empty())
        std::cout << "password: " << pwd << " tries: " << solver.tries << std::endl;
    else
        std::cout << "can not match password" << std::endl;
#endif

    int n = 11, w = 0;
    const Maze maze = maze::genMaze("../Test_Data/last/1_maze_15_15.json");
    const Json json = mazeToJson(maze);
    maze::printJson(json, "../Test_Data/first/maze2.json");
    n = maze.size();
    dp::DP dpRuner(maze);
    dpRuner.run();
    const auto dpPath = dpRuner.getPath();
    w = getPathVal(dpPath, maze);
    assert(w == dpRuner.getValue() - maze::BVAL - maze::LVAL);
    printPath(dpPath, maze);
    cout << "DP value: "<< w << endl;
    sumDPVal += w;
    cout << "------------------------------------------------------------------" << endl;

    MazeNavigator greedyRunner(maze);
    auto [greedyPath, greedyW] = greedyRunner.findPath();
    w = getPathVal(greedyPath, maze);
    assert(w == greedyW - maze::BVAL - maze::LVAL);
    printPath(greedyPath, maze);
    cout << "greedy value: "<< w << endl;
    sumGreedyVal += w;
    cout << "------------------------------------------------------------------" << endl;

    smart::SmartRunner smartRunner(maze);
    smartRunner.run();
    const auto smartPath = smartRunner.getPath();
    w = getPathVal(smartPath, maze);
    assert(w == smartRunner.getW() - maze::BVAL - maze::LVAL);
    printPath(smartPath, maze);
    cout << "smart value: "<< w << endl;
    sumSmartVal += w;

    cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||" << endl;
    return true;
}
int main() {
    constexpr int t = 1;
    for(int i = 0; i < t; ++i) solve();
    std::cout <<  "sum DP value: " << sumDPVal << " " << " average: " << 1.0 * sumDPVal / t << std::endl;
    std::cout <<  "sum greedy value: " << sumGreedyVal << " average: " << 1.0 * sumGreedyVal / t << std::endl;
    std::cout <<  "sum smart value: " << sumSmartVal << " average: " << 1.0 * sumSmartVal / t << std::endl;
    return 0;
}