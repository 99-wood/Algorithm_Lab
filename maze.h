//
// Created by admin on 2025/6/24.
//
// auther: FW

#ifndef MAZE_H
#define MAZE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <cassert>
#include <utility>
#include "nlohmann/json.hpp"

namespace maze {
    constexpr int GVAL = 50;
    constexpr int TVAL = -30;
    constexpr int BVAL = 1e8;
    constexpr int LVAL = 1e8;

    using std::vector;
    using std::pair;
    enum class NodeType {
        G, // 金币
        S, // 起点
        E, // 出口
        W, // 墙壁
        R, // 通路
        T, // 陷阱
        L, // 机关
        B, // Boss
        U  // Unknown
    };

    inline NodeType stringToNodeType(const std::string &str) {
        if (str == "G") return NodeType::G;
        if (str == "S") return NodeType::S;
        if (str == "E") return NodeType::E;
        if (str == "#") return NodeType::W;
        if (str == " ") return NodeType::R;
        if (str == "T") return NodeType::T;
        if (str == "L") return NodeType::L;
        if (str == "B") return NodeType::B;
        if (str == "U") return NodeType::U;
        assert(0);
    }

    inline std::ostream &operator<<(std::ostream &os, const NodeType &nodeType) {
        switch (nodeType) {
            case NodeType::G:
                return os << 'G';
            case NodeType::S:
                return os << 'S';
            case NodeType::E:
                return os << 'E';
            case NodeType::W:
                return os << '#';
            case NodeType::R:
                return os << ' ';
            case NodeType::T:
                return os << 'T';
            case NodeType::L:
                return os << 'L';
            case NodeType::B:
                return os << 'B';
            case NodeType::U:
                return os << 'U';
            default:
                assert(0);
        }
        assert(0);
    }

    class Node {
    public:
        NodeType nodeType;
        int value;

        friend std::ostream &operator<<(std::ostream &os, const Node &node) {
            return os << node.nodeType;
        }
    };

    using Maze = vector<vector<Node>>;
    using Pass = vector<pair<pair<int, int>, pair<int, int>>>;
    using PointPair = pair<pair<int, int>, pair<int, int>>;
    using xyPair = pair<int, int>;

    vector<vector<Node>> genMaze(int n);

    vector<vector<Node>> genMaze(const std::string &fileName);


    void initalMaze(Maze &maze, int n);//迷宫初始化,规定奇数行为路,偶数行为墙
    bool generatStartOrExit(Maze &maze, int n, NodeType t);

    int generateRandomOdd(int min, int max);//随机数
    int generateRandomEven(int min, int max);

    int getRand(int min, int max);

    void getPassage(Pass &result, int x1, int y1, int x2, int y2);

    void destroyWall(Pass const &result, Maze &maze);

    void generateSource(Maze &maze, int n);

    void generateTrap(Maze &maze, int n);

    void generateLoker(Maze &maze, int n);

    void generateBoss(Maze &maze, int n);

    using Json = nlohmann::json;

    Json mazeToJson(const Maze &maze);

    bool printJson(const Json &j, const char *fileName);


}

#endif //MAZE_H
