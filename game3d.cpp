#include <cassert>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <ft2build.h>

#include "boss_strategy.h"
#include "ch.h"

#include FT_FREETYPE_H

#include "cube.h"
#include "shader.h"
#include "stb_image.h"
#include "texture.h"
#include "player.h"
#include "chest.h"
#include "dp.h"
#include "maze.h"
#include "puzzle_solver.h"
#include "slime.h"
constexpr int screenWidth = 1920, screenHeight = 1080;
constexpr int messageBoxSize = 10;

void framebuffer_size_callback(GLFWwindow *window, const int width, const int height) {
    glViewport(0, 0, width, height); // 设置视口区域
}

constexpr float radius = 6.0f; // 摄像机距原点的距离
float yaw = glm::radians(45.0f); // 初始旋转角度
// constexpr auto target = glm::vec3{0, 1.5, 0};
void processInput(GLFWwindow *window, PlayerController *controller) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    static float lastRotateTime = 0.0f;
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        if(glfwGetTime() - lastRotateTime > 0.2){
            if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
                yaw += glm::radians(45.0f); // 每次加 90°
            }
            else{
                yaw += glm::radians(-45.0f); // 每次减 90°
            }
            lastRotateTime = static_cast<float>(glfwGetTime());
        }
    }
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
       glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        const bool W = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS,
                S = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS,
                A = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS,
                D = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
        const auto tpos = controller->getCurrentPos();
        const int u = static_cast<int>(std::round(tpos.first)), v = static_cast<int>(std::round(tpos.second));
        int dir = 0;
        dir += static_cast<int>(std::round(yaw / glm::radians(45.0f)));
        if(W && A) dir += 1;
        else if(W && D) dir += 7;
        else if(S && A) dir += 3;
        else if(S && D) dir += 5;
        else if(W) dir += 0;
        else if(A) dir += 2;
        else if(S) dir += 4;
        else if(D) dir += 6;
        dir %= 8;
        if(dir == 0){
            controller->changeAction({1, u - 1, v});
        }
        else if(dir == 2){
            controller->changeAction({1, u, v - 1});
        }
        else if(dir == 4){
            controller->changeAction({1, u + 1, v});
        }
        else if(dir == 6){
            controller->changeAction({1, u, v + 1});
        }
    }
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        if(controller->isQueueEmpty() && controller->isAvailable()) controller->addAction({2, 0, 0});
    }
}

glm::mat4 billboard(const glm::vec3 &position, const glm::mat4 &view) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    // 去掉 view 矩阵的平移部分（只保留旋转）
    glm::mat4 rotationOnly = glm::mat4(glm::mat3(view));
    model *= glm::transpose(rotationOnly); // 反向旋转（billboard）

    return model;
}

GLFWwindow *window;

void init() {
    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 使用 OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 核心模式

    // 创建窗口
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Maze", nullptr, nullptr);
    if(window == nullptr){
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        assert(0);
    }
    glfwMakeContextCurrent(window);

    // 初始化 GLAD
    if(!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))){
        std::cerr << "Failed to initialize GLAD\n";
        assert(0);
    }

    // 设置视口和回调
    glViewport(0, 0, screenWidth, screenHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Cube::init();
    Slime::init();
    HealthBar::init();
    ch::load();
    ch::init(screenWidth, screenHeight);
}

using Maze = maze::Maze;

enum class State {
    WALK, FIGHT, GUESS
};
int main() {
    init();
    // OpenGL setting
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    const Texture wallTexture("../texture/wall/wall.png");
    const Texture woodTexture("../texture/wood.png");
    const Texture snowTexture("../texture/snow.png");
    const Texture goldTexture("../texture/gold/gold.png");
    const Texture trapTexture("../texture/trap/trap.png");
    const Texture roadSideTexture("../texture/road/road0.png");
    const Texture roadTopTexture("../texture/road/road1.png");
    const Texture unknownTexture("../texture/unknown/unknown.png");
    const Texture headFrontTexture("../texture/player/head_front.png");
    const Texture headBackTexture("../texture/player/head_back.png");
    const Texture bodyFrontTexture("../texture/player/body_front.png");
    const Texture armSideTexture("../texture/player/arm_side.png");
    const Texture armTopTexture("../texture/player/arm_top.png");
    const Texture armBottomTexture("../texture/player/arm_bottom.png");
    const Texture legSideTexture("../texture/player/leg_side.png");
    const Texture legFrontTexture("../texture/player/leg_front.png");
    const Texture slimeTexture("../texture/slime/inner.png");
    const Texture chestUpSideTexture("../texture/chest/chest_up_side.png");
    const Texture chestDownFrontTexture("../texture/chest/chest_down_front.png");
    const Texture chestDownSideTexture("../texture/chest/chest_down_side.png");
    const Texture chestTopTexture("../texture/chest/chest_top.png");
    const Cube roadCube(&roadSideTexture, &roadSideTexture, &roadSideTexture, &roadSideTexture, &roadTopTexture, nullptr);
    const Cube wallCube(&wallTexture);
    const Cube goldCube(&goldTexture);
    const Cube unknownCube(&unknownTexture);
    const Cube trapCube(&trapTexture);
    const Cube headCube(&headFrontTexture, &headBackTexture, &headBackTexture, &headBackTexture, &headBackTexture, &headBackTexture);
    const Cube bodyCube(&bodyFrontTexture, &armTopTexture, &armTopTexture, &armTopTexture, &armTopTexture, &armTopTexture);
    const Cube armCube(&armSideTexture, &armSideTexture, &armSideTexture, &armSideTexture, &armTopTexture, &armBottomTexture);
    const Cube legCube(&legFrontTexture, &legSideTexture, &legSideTexture, &legSideTexture, &armTopTexture, &armBottomTexture);
    // const Cube chestCube(&chestDownFrontTexture, &chestDownSideTexture, &chestDownSideTexture, &chestDownSideTexture, &chestTopTexture, &chestTopTexture);
    const Cube chestUpCube(&chestUpSideTexture, &chestUpSideTexture, &chestUpSideTexture, &chestUpSideTexture, &chestTopTexture, nullptr);
    const Cube chestDownCube(&chestDownFrontTexture, &chestDownSideTexture, &chestDownSideTexture, &chestDownSideTexture, nullptr, &chestTopTexture);
    Chest chest(&chestUpCube, &chestDownCube);
    const Slime slimeCube(&slimeTexture);
    HealthBar hpBar(1.0f);
    Player player(&headCube, &bodyCube, &armCube, &armCube, &legCube, &legCube);

    std::cout << "Start Guess Test" << std::endl;
    for(int i = 0; i <= 29; ++i){
        std::stringstream ss;
        ss << "../Test_Data/official/guess/pwd_0";
        if(i < 10) ss << 0;
        ss << i;
        ss << ".json";
        const auto [hash, clues] = guess::PuzzleSolver::loadPuzzleData(ss.str());
        guess::PuzzleSolver guessSolver(hash, clues);
        guessSolver.solve();
        const std::string pwd = guessSolver.ans;
        std::cout << "test" << i << ": ";
        assert(!pwd.empty());
        if(guess::PuzzleSolver::sha256(pwd) == hash){
            std::cout << "OK Answer: " << pwd << std::endl;
        }
        else{
            std::cout << "Wrong" << std::endl;
        }
    }
    std::cout << "Start Boss Test" << std::endl;
    for(int i = 1; i <= 9; ++i){
        std::stringstream ss;
        ss << "../Test_Data/official/BOSS/boss_case_";
        ss << i;
        ss << ".json";
        // std::cout << ss.str() << std::endl;
        const auto [bossHPs, skills] = boss::loadBossBattleData(ss.str());
        const int minTurn = boss::loadMinNum(ss.str());
        boss::BossStrategy strategy;
        strategy.init(skills, bossHPs);
        const std::vector<int> plan = strategy.findOptimalSequence(false);
        if(plan.size() <= minTurn){
            std::cout << "OK Answer: " << plan.size() << std::endl;
        }
        else{
            std::cout << "Wrong" << std::endl;
        }
    }

    int n = 15;
    const Maze originMaze = maze::genMaze("../Test_Data/first/dp/hard/maze_15_15_2.json");
//    const Maze originMaze = maze::genMaze(n);
    const auto [bossHPs, skills] = boss::loadBossBattleData("../Test_Data/official/BOSS/boss_case_2.json");
    const auto [hash, clues] = guess::PuzzleSolver::loadPuzzleData("../Test_Data/official/guess/pwd_002.json");

    // 导出地图
    const nlohmann::json json = maze::mazeToJson(originMaze);
    maze::printJson(json, "../Test_Data/first/maze2.json");

    // DP 计算路径
    auto maze = originMaze;
    n = static_cast<int>(originMaze.size());
    dp::DP dpRuner(originMaze);
    dpRuner.run();
    const auto path = dpRuner.getPath();
    const auto W = dpRuner.getValue();

    // 计算 BOSS
    boss::BossStrategy strategy;
    strategy.init(skills, bossHPs);
    std::cout << " Executing Branch and Bound...\n";
    const std::vector<int> plan = strategy.findOptimalSequence(false);
    for(auto x : plan){
        std::cout << x << " ";
    }
    std::cout << std::endl;
    auto skillIt = plan.begin();
    auto currentBossHps = bossHPs;
    auto bossIt = currentBossHps.begin();
    auto currentSkills = skills;
    for(auto &[id, damage, cooldown] : currentSkills){
        cooldown = 0;
    }

    // 猜
    guess::PuzzleSolver guessSolver(hash, clues);
    guessSolver.solve();
    const std::string pwd = guessSolver.ans;
    const int guessTries = guessSolver.tries;

    State state = State::WALK;
    std::vector<std::string> messageL;
    std::vector<std::string> messageR;

    std::cout << "value: " << W % maze::BVAL << std::endl;
    std::vector see(n, std::vector(n, false));
    std::vector vis(n, std::vector(n, false));
    auto pathIt = path.begin();
    auto [x, y] = *pathIt;
    vis[x][y] = true;
    for(int i = x - 1; i <= x + 1; ++i){
        for(int j = y - 1; j <= y + 1; ++j){
            if(i >= 0 && i < n && j >= 0 && j < n) see[i][j] = true;
        }
    }
    PlayerController controller(&player, *pathIt);
    ++pathIt;
    controller.setSpeed(3);
    int w = 0;
    while(!glfwWindowShouldClose(window)){
        processInput(window, &controller);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(state == State::WALK){
            controller.setSpeed(3);
            if(pathIt != path.end() && controller.isAvailable() && controller.isQueueEmpty()){
                if(!vis[x][y]){
                    w += maze[x][y].value > 10000 ? 0 : maze[x][y].value;
                    vis[x][y] = true;
                    assert(maze[x][y].nodeType != maze::NodeType::L && maze[x][y].nodeType != maze::NodeType::B);
                    maze[x][y].nodeType = maze::NodeType::R;
                    maze[x][y].value = 0;
                }
                std::cout << x << " " << y << " " << w << std::endl;
                if(maze[pathIt->first][pathIt->second].nodeType == maze::NodeType::B){
                    state = State::FIGHT;
                    controller.addAction({3, pathIt->first - x, pathIt->second - y});
                }
                else if(maze[pathIt->first][pathIt->second].nodeType == maze::NodeType::L){
                    state = State::GUESS;
                    controller.addAction({3, pathIt->first - x, pathIt->second - y});
                }
                else{
                    controller.addAction({1, pathIt->first, pathIt->second});
                    x = pathIt->first;
                    y = pathIt->second;
                    for(int i = x - 1; i <= x + 1; ++i){
                        for(int j = y - 1; j <= y + 1; ++j){
                            if(i >= 0 && i < n && j >= 0 && j < n) see[i][j] = true;
                        }
                    }
                    ++pathIt;
                }
            }
        }
        else if(state == State::FIGHT){
            controller.setSpeed(0.5);
            static int round = 0;
            if(controller.isAvailable() && controller.isQueueEmpty()){
                if(skillIt != plan.end()){
                    ++round;
                    for(auto &[id, damage, cooldown] : currentSkills){
                        cooldown -= std::min(1, cooldown);
                    }
                    std::cout << std::endl;
                    controller.addAction({2, 0, 0});
                    assert(bossIt != currentBossHps.end());
                    if(currentSkills[*skillIt].cooldown > 0){
                        std::cout << *skillIt << " " << currentSkills[*skillIt].cooldown;
                    }
                    assert(currentSkills[*skillIt].cooldown == 0);
                    *bossIt = std::max(*bossIt - skills[*skillIt].damage, 0);
                    std::stringstream ss;
                    ss << "round" << round << ": use skill: " << *skillIt << " damage: " << skills[*skillIt].damage << " bossHp: " << *bossIt << std::endl;
                    std::string info;
                    std::getline(ss, info);
                    messageL.push_back(info);
                    if(*bossIt == 0) ++bossIt;
                    if(bossIt != currentBossHps.end()){
                        hpBar.setHealth(1.0f * (*bossIt) / bossHPs[bossIt - currentBossHps.begin()]);
                    }
                    currentSkills[*skillIt].cooldown = skills[*skillIt].cooldown + 1;
                    ++skillIt;
                }
                else{
                    assert(bossIt == currentBossHps.end());
                    maze[pathIt->first][pathIt->second].nodeType = maze::NodeType::R;
                    state = State::WALK;
                }
            }
        }
        else{
            assert(state == State::GUESS);
            static int finish = 0;
            controller.setSpeed(0.5);
            if(!finish && controller.isAvailable() && controller.isQueueEmpty()){
                controller.addAction({2, 0, 0});
                std::stringstream ss;
                if(pwd.empty()){
                    ss << "No Answer!";
                    ss << " Tries: " << guessTries;
                    w -= guessTries;
                }
                else{
                    ss << "Answer: " << pwd;
                    ss << " Tries: " << guessTries;
                    ss << " Hash: " << guess::PuzzleSolver::sha256(pwd);
                    w -= guessTries;
                }
                std::string info;
                std::getline(ss, info);
                messageR.push_back(info);
                chest.setTarget(ChestState::OPEN);
                finish = 1;
            }
            if(chest.getState() == ChestState::OPEN){
                chest.setTarget(ChestState::CLOSE);
                finish = 2;
            }
            if(chest.getState() == ChestState::CLOSE && finish == 2){
                maze[pathIt->first][pathIt->second].nodeType = maze::NodeType::R;
                state = State::WALK;
            }
        }

        const auto tmp = controller.getCurrentPos();
        const glm::vec3 target{tmp.second, PLAYER_HEIGHT * 2, tmp.first};
        auto cameraPos = glm::vec3(target.x + radius * std::sin(yaw) * 1.5f, radius * 1.5f,
                                   target.z + radius * std::cos(yaw) * 1.5f);
        auto upDirection = glm::vec3(0.0f, 1.0f, 0.0f);
        const glm::mat4 view = glm::lookAt(cameraPos, target, upDirection);

        const glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f * screenWidth / screenHeight, 0.1f,
                                                      100.0f);

        for(int i = 0; i < n; ++i){
            for(int j = 0; j < n; ++j){
                glm::mat4 model(1.0f);
                model = glm::translate(model, glm::vec3(j, 0, i));
                if(!see[i][j]){
                    unknownCube.draw(model, view, projection);
                    model = glm::translate(model, glm::vec3(0, 1, 0));
                    unknownCube.draw(model, view, projection);
                }
                else if(maze[i][j].nodeType == maze::NodeType::R || maze[i][j].nodeType == maze::NodeType::S || maze[i][j].nodeType == maze::NodeType::E){
                    roadCube.draw(model, view, projection);
                }
                else if(maze[i][j].nodeType == maze::NodeType::G){
                    goldCube.draw(model, view, projection);
                }
                else if(maze[i][j].nodeType == maze::NodeType::T){
                    trapCube.draw(model, view, projection);
                }
                else if(maze[i][j].nodeType == maze::NodeType::W){
                    roadCube.draw(model, view, projection);
                    model = glm::translate(model, glm::vec3(0, 1, 0));
                    wallCube.draw(model, view, projection);
                }
                else if(maze[i][j].nodeType == maze::NodeType::L){
                    roadCube.draw(model, view, projection);
                    model = glm::translate(model, glm::vec3(0, 1, 0));
                    chest.draw(model, view, projection);
                }
                else if(maze[i][j].nodeType == maze::NodeType::B){
                    roadCube.draw(model, view, projection);
                    model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
                    model = glm::translate(model, glm::vec3(0, 1.3f, 0));
                    slimeCube.draw(model, view, projection);

                    glm::mat4 barModel = billboard(glm::vec3(j, 2.0f, i), view);
                    barModel = glm::scale(barModel, glm::vec3(1.5f, 0.2f, 1.0f)); // 血条尺寸
                    hpBar.draw(barModel, view, projection);
                }
                else{
                    std::cerr << maze[i][j].nodeType;
                    assert(0);
                }
            }
        }
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0, 0.85f, 0));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        slimeCube.draw(model, view, projection);

        controller.update(view, projection);

        std::stringstream ss;
        ss << "current value: ";
        ss << w;
        ch::RenderText(ss.str(), screenWidth - 200.0f, screenHeight - 30.0f, 0.5f, glm::vec3(0.5f, 0.8f, 0.2f));
        ch::RenderText("DP demo", 10.0f, screenHeight - 30.0f, 0.5f, glm::vec3(0.3f, 0.7f, 0.9f));
        auto it = messageL.rbegin();
        for(int i = 1; i <= messageBoxSize && it != messageL.rend(); ++i, ++it){
            ch::RenderText(*it, 10.0f, static_cast<float>(screenHeight) - 30.0f - static_cast<float>(i) * 30.0f, 0.5f, glm::vec3(0.9f, 0.7f, 0.3f));
        }
        it = messageR.rbegin();
        for(int i = 1; i <= messageBoxSize && it != messageR.rend(); ++i, ++it){
            ch::RenderText(*it, screenWidth - 800.0f, static_cast<float>(screenHeight) - 30.0f - static_cast<float>(i) * 30.0f, 0.5f, glm::vec3(0.9f, 0.7f, 0.3f));
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}
