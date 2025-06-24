#include <bits/stdc++.h>
#include "maze.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;
using Maze = maze::Maze;
int main() {
    Maze maze = maze::genMaze("../input.json");
    for(const auto &row : maze){
        for(const auto &x : row){
            cout << x << " ";
        }
        cout << endl;
    }

}