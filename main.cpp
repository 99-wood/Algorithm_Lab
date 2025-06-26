#include <bits/stdc++.h>
#include "maze.h"
#include "dp.h"
using json = nlohmann::json;
using namespace std;
using Maze = maze::Maze;
int main() {
    const int n = 1001;
    const Maze maze = maze::genMaze("../test.json");
    for(const auto &row : maze){
        for(const auto &x : row){
            cout << x << " ";
        }
        cout << endl;
    }
    dp::DP dpRuner(maze);
    dpRuner.run();
    cout << dpRuner.getValue() << endl;
    const auto path = dpRuner.getPath();
    vector vis(n, vector(n, false));
    int w = 0;
    for(const auto [x, y] : path){
        if(maze[x][y].nodeType == maze::NodeType::G && !vis[x][y]) w += maze[x][y].value;
        else if(maze[x][y].nodeType == maze::NodeType::T && !vis[x][y]) w += maze[x][y].value;
        vis[x][y] = true;
    }
    cout << w << endl;
    cout << path.size() << endl;
    return 0;
}