#include <bits/stdc++.h>
#include "maze.h"
// #include "dp.h"
#include <conio.h>  // _kbhit() 和 _getch()
#include "maze_navigator.h"

using json = nlohmann::json;
using namespace std;
using Maze = maze::Maze;

int main() {
    Maze maze = maze::genMaze(15);
    const Maze origin = maze;
    MazeNavigator greedyRunner(maze);
    const auto [path, best] = greedyRunner.findPath();
    const int n = static_cast<int>(maze.size());
    vector see(n, vector(n, false));
    int x = 1, y = 1;
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            if(maze[i][j].nodeType == maze::NodeType::S){
                x = i;
                y = j;
            }
        }
    }
    int w = 0;
    for(int i = x - 1; i <= x + 1; ++i){
        for(int j = y - 1; j <= y + 1; ++j){
            if(i >= 0 && i < n && j >= 0 && j < n) see[i][j] = true;
        }
    }
    cout << "Use WASD to move. Press 'q' to quit.\n";
    while (true) {
        const auto enable = [&](int x, int y) {
            return x >= 0 && x < n && y >= 0 && y < n && maze[x][y].nodeType != maze::NodeType::W;
        };
        if (_kbhit()) {
            char ch = _getch();
            if (ch == 'a' && enable(x, y - 1)) y--;
            else if (ch == 'd' && enable(x, y + 1)) y++;
            else if (ch == 'w' && enable(x - 1, y)) x--;
            else if (ch == 's' && enable(x + 1, y)) x++;
            else if (ch == 'q') break;

            system("cls"); // 清屏，仅限 Windows
            if(maze[x][y].nodeType == maze::NodeType::E){
                break;
            }
            w += maze[x][y].value;
            maze[x][y].nodeType = maze::NodeType::R;
            maze[x][y].value = 0;
            for(int i = x - 1; i <= x + 1; ++i){
                for(int j = y - 1; j <= y + 1; ++j){
                    if(i >= 0 && i < n && j >= 0 && j < n) see[i][j] = true;
                }
            }
            for(int i = 0; i < n; ++i){
                for(int j = 0; j < n; ++j){
                    if(i == x && j == y) cout << "! ";
                    else{
                        if(see[i][j]) cout << maze[i][j] << " ";
                        else cout << "* ";
                    }
                }
                cout << endl;
            }
            cout << "Position: (" << x << ", " << y << ") Value: " << w << endl;
        }
    }
    system("cls"); // 清屏，仅限 Windows
    cout << "Your score: " << w << endl;
    cout << "Best score: " << best << endl;
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            cout << origin[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            cout << maze[i][j] << " ";
        }
        cout << endl;
    }
    vector vis(n, vector(n, false));
    for(const auto [x, y] : path){
        vis[x][y] = true;
    }
    cout << endl;
    w = 0;
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            if(vis[i][j]){
                cout << "_ ";
                w += origin[i][j].value;
            }
            else cout << origin[i][j] << " ";
        }
        cout << endl;
    }
    cout << w << endl;
    return 0;
}