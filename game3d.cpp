#include <cassert>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <ft2build.h>

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
#include "slime.h"
constexpr int screenWidth = 1280, screenHeight = 720;

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
}

using Maze = maze::Maze;
int main() {
    init();
    ch::load();
    ch::init(screenWidth, screenHeight);
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
    chest.setTarget(ChestState::OPEN);
    const Slime slimeCube(&slimeTexture);
    Player player(&headCube, &bodyCube, &armCube, &armCube, &legCube, &legCube);


    int n = 15;
    const Maze originMaze = maze::genMaze("../Test_Data/first/dp/hard/maze_15_15_2.json");
//    const Maze originMaze = maze::genMaze(n);
    const nlohmann::json json = maze::mazeToJson(originMaze);
    maze::printJson(json, "../Test_Data/first/maze2.json");
    auto maze = originMaze;
    n = static_cast<int>(originMaze.size());
    dp::DP dpRuner(originMaze);
    dpRuner.run();
    const auto path = dpRuner.getPath();
    const auto W = dpRuner.getValue();
    std::cout << "value: " << W % maze::BVAL << std::endl;
    std::vector see(n, std::vector(n, false));
    std::vector vis(n, std::vector(n, false));
    // const std::vector<std::pair<int, int>> path{{0, 0}, {1, 0}, {1, 1}, {2, 1}};
    auto it = path.begin();
    auto [x, y] = *it;
    vis[x][y] = true;
    for(int i = x - 1; i <= x + 1; ++i){
        for(int j = y - 1; j <= y + 1; ++j){
            if(i >= 0 && i < n && j >= 0 && j < n) see[i][j] = true;
        }
    }
    PlayerController controller(&player, *it);
    ++it;
    controller.setSpeed(3);
    int w = 0;
    while(!glfwWindowShouldClose(window)){
        if(chest.getState() != ChestState::HALF){
            if(chest.getState() == ChestState::OPEN) chest.setTarget(ChestState::CLOSE);
            else chest.setTarget(ChestState::OPEN);
        }
        processInput(window, &controller);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(it != path.end() && controller.isAvailable() && controller.isQueueEmpty()){
            controller.addAction({1, it->first, it->second});
            if(!vis[x][y]){
                w += maze[x][y].value > 10000 ? 0 : maze[x][y].value;
                vis[x][y] = true;
                if(maze[x][y].nodeType != maze::NodeType::L && maze[x][y].nodeType != maze::NodeType::B) maze[x][y].nodeType = maze::NodeType::R;
                maze[x][y].value = 0;
            }
            std::cout << x << " " << y << " " << w << std::endl;
            x = it->first;
            y = it->second;
            for(int i = x - 1; i <= x + 1; ++i){
                for(int j = y - 1; j <= y + 1; ++j){
                    if(i >= 0 && i < n && j >= 0 && j < n) see[i][j] = true;
                }
            }
            ++it;
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
        ch::RenderText(ss.str(), 1080.0f, 680.0f, 0.5f, glm::vec3(0.5f, 0.8f, 0.2f));
        ch::RenderText("DP demo", 10.0f, 680.0f, 0.5f, glm::vec3(0.3f, 0.7f, 0.9f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;
}
