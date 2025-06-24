//
// Created by admin on 2025/6/24.
//

#include "maze.h"

namespace maze{
    std::random_device rd;
    std::mt19937 gen(rd());
    void InitalMaze(Maze &maze,int n){
        //设置边界
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(i%2==0 || j%2==0 ) maze[i][j]=Node{NodeType::W,0};
            }
        }
        //生成出入口
        generatStartOrExit(maze,n,NodeType::S);
        while (!generatStartOrExit(maze,n,NodeType::E));

    }

    int getRand(int min, int max){
        // 验证范围有效性
        if (min > max) {
            throw std::invalid_argument("Invalid range: min cannot be greater than max");
        }
        std::uniform_int_distribution<> dist(min,max);
        int tmp=dist(gen);
        //std::cout<<tmp<<' ';
        return tmp;
    }

    int generateRandomOdd(int min, int max) {
        // 验证范围有效性
        if (min > max) {
            throw std::invalid_argument("Invalid range: min cannot be greater than max");
        }
        std::uniform_int_distribution<> dist(min,max);
        int t =dist(gen);
        int tmp=((t+1)/2)*2-1;
        //std::cout<<tmp<<' '<<t<<' '<<min<<' '<<max<<'\n';
        return tmp;
    }
    int generateRandomEven(int min, int max) {
        // 验证范围有效性
        if (min > max) {
            throw std::invalid_argument("Invalid range: min cannot be greater than max");
        }
        std::uniform_int_distribution<> dist(min,max);
        int tmp=(dist(gen)/2)*2;
        //std::cout<<tmp<<' ';
        return tmp;
    }

    vector<vector<Node>> genMaze(int n){
        Maze maze(n,vector<Node>(n,Node{NodeType::R,0}));
        InitalMaze(maze,n);//生成并初始化，并设置出入口
        Pass p;
        int reflection=(n+1)/2;
        getPassage(p,0,0,reflection-2,reflection-2);//递归生成迷宫
        destroyWall(p,maze);//修改初始迷宫
        generateBoss(maze,n);//生成Boss
        generateLoker(maze,n);//生成机关
        generateTrap(maze,n);//生成陷阱
        generateSource(maze,n);//资源生成
        return maze;
    }

    void getPassage(Pass &result,int x1,int y1,int x2,int y2){
        //最小子问题
        if(x1==x2){
            for(int i=y1;i<y2;i++){
                result.push_back(PointPair(xyPair(x1,i),xyPair(x1,i+1)));
            }
            return;
        }
        if(y1==y2){
            for(int i=x1;i<x2;i++){
                result.push_back(PointPair(xyPair(i,y1),xyPair(i+1,y1)));
            }
            return;
        }
        //正常情况
        //找分割点左上方的位置,可选范围x1~x2-1,y2~y2-1
        //std::cout<<x1<<':'<<y1<<' '<<x2<<':'<<y2<<std::endl;
        int di=getRand(x1,x2-1),dj=getRand(y1,y2-1);
        //std::cout<<di<<':'<<dj<<std::endl;
        //添加断口
        int randomSelection=getRand(0,3),randomDoor;
        if(randomSelection%4!=0){//左
            randomDoor=getRand(y1,dj);
            result.push_back(PointPair(xyPair(di,randomDoor),xyPair(di+1,randomDoor)));
        }
        if(randomSelection%4!=1){//
            randomDoor=getRand(di+1,x2);
            result.push_back(PointPair(xyPair(randomDoor,dj),xyPair(randomDoor,dj+1)));
        }
        if(randomSelection%4!=2){
            randomDoor=getRand(dj+1,y2);
            result.push_back(PointPair(xyPair(di,randomDoor),xyPair(di+1,randomDoor)));
        }
        if(randomSelection%4!=3){
            randomDoor=getRand(x1,di);
            result.push_back(PointPair(xyPair(randomDoor,dj),xyPair(randomDoor,dj+1)));
        }
        getPassage(result,x1  ,y1  ,di,dj);
        getPassage(result,di+1,y1  ,x2,dj);
        getPassage(result,di+1,dj+1,x2,y2);
        getPassage(result,x1  ,dj+1,di,y2);
    }

    void destroyWall(Pass const &result,Maze &maze){
        for(int i=0;i<result.size();i++){
            if(result[i].first.first==result[i].second.first){//行相同，消除竖
                int j1=result[i].first.second,j2=result[i].second.second;
                int ywall=((j1*2+1)+(j2*2+1))/2;
                int xwall=result[i].first.first*2+1;
                maze[xwall][ywall]=Node{NodeType::R,0};
            }else{//列相同，消横
                int i1=result[i].first.first,i2=result[i].second.first;
                int xwall=((i1*2+1)+(i2*2+1))/2;
                int ywall=result[i].first.second*2+1;
                maze[xwall][ywall]=Node{NodeType::R,0};
            }
        }
    }

    void generateSource(Maze &maze,int n){
        int GoldRate=50,
            RandomUpper=100,
            RandomLower=1,
            GoldValueUpper=5,
            GoldValueLower=1;
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(maze[i][j].nodeType==NodeType::R && getRand(RandomLower,RandomUpper)<GoldRate){
                    maze[i][j]=Node{NodeType::G,getRand(GoldValueLower,GoldValueUpper)};
                }
            }
        }
    }

    bool generatStartOrExit(Maze &maze,int n,NodeType t){
        int randomSelect=getRand(0,3);
        int i,j;
        switch (randomSelect){
            case 0:i=0;j=generateRandomOdd(1,n-2);//上
                break;
            case 1:i=generateRandomOdd(1,n-2),j=0;//左
                break;
            case 2:i=n-1;j=generateRandomOdd(1,n-2);//下
                break;
            case 3:i=generateRandomOdd(1,n-2),j=n-1;//右
                break;
            default:assert(0);
        }
        if(t==NodeType::S){
            maze[i][j]=Node{NodeType::S,0};
            return true;
        }else{
            if(maze[i][j].nodeType==NodeType::S)return false;
            else{
                maze[i][j]=Node{NodeType::E,0};
                return true;
            }
        }
    }

    void generateTrap(Maze &maze,int n){
        int TrapMostNum=6,
        TrapNumPos=TrapMostNum,
        TrapValueUpper=6,
        TrapValueLower=3,
        RandomUpper=100,
        RandomLower=1,
        TrapRate=5;
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(maze[i][j].nodeType==NodeType::R && getRand(RandomLower,RandomUpper)<TrapRate && TrapNumPos){
                    maze[i][j]=Node{NodeType::T,0-getRand(TrapValueLower,TrapValueUpper)};
                    TrapNumPos--;
                }
            }
        }
    }
    void generateLoker(Maze &maze,int n){
        int LokerMostNum=2,
        LokerNumPos=LokerMostNum,
        LokerValueUpper=10,
        LokerValueLower=4,
        RandomUpper=100,
        RandomLower=1,
        LokerRate=3;
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(maze[i][j].nodeType==NodeType::R && getRand(RandomLower,RandomUpper)<LokerRate && LokerNumPos){
                    maze[i][j]=Node{NodeType::L,getRand(LokerValueLower,LokerValueUpper)};
                    LokerNumPos--;
                }
            }
        }
    }
    void generateBoss(Maze &maze,int n){
        int BossMostNum=1,
        BossNumPos=BossMostNum,
        BossValueUpper=20,
        BossValueLower=4,
        RandomUpper=100,
        RandomLower=1,
        LokerRate=3;
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(maze[i][j].nodeType==NodeType::R && getRand(RandomLower,RandomUpper)<LokerRate && BossNumPos){
                    maze[i][j]=Node{NodeType::B,getRand(BossValueLower,BossValueUpper)};
                    BossNumPos--;
                }
            }
        }
        if(BossMostNum==BossNumPos){//保证至少一个生成
            generateBoss(maze,n);
        }
    }
    vector<vector<Node>> genMaze(const std::string &fileName) {
        using json = nlohmann::json;
        std::ifstream fin("../input.json");
        if (!fin.is_open()) {
            std::cerr << "无法打开文件 input.json" << std::endl;
            assert(0);
        }

        // 解析 JSON
        json data;
        fin >> data;

        // 读取 maze
        auto maze_json= data["maze"];
        const int n = maze_json.size(), m = maze_json[0].size();
        assert(n > 0 && m > 0);
        Maze maze(maze_json.size(), vector<Node>(maze_json[0].size()));
        for(int i = 0; i < n; ++i){
            for(int j = 0; j < m; ++j){
                maze[i][j].nodeType = stringToNodeType(maze_json[i][j].get<std::string>());
            }
        }
        return maze;
    }
}

