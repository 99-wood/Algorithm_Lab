//
// Created by admin on 2025/6/30.
//

#include "slime.h"

#include "GLFW/glfw3.h"
unsigned Slime::VBO = 0;
unsigned Slime::EBO = 0;
int Slime::modelLoc = 0, Slime::viewLoc = 0, Slime::projectionLoc = 0;
bool Slime::INITED = false;
Shader *Slime::shader = nullptr;

void Slime::init(const char *vertexPath, const char *fragmentPath) {
    if(INITED) return;
    shader = new Shader(vertexPath, fragmentPath);
    shader->use();
    modelLoc = glGetUniformLocation(shader->ID, "model");
    viewLoc = glGetUniformLocation(shader->ID, "view");
    projectionLoc = glGetUniformLocation(shader->ID, "projection");
    constexpr float vertices[] = {
        // 位置              // 颜色           // 纹理坐标

        // 前面
        -0.5f, -0.5f, 0.5f, 1, 0, 0, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0, 1, 0, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0, 0, 1, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 1, 1, 0, 0.0f, 1.0f,

        // 后面
        -0.5f, -0.5f, -0.5f, 1, 0, 1, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0, 1, 1, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0, 0, 0, 0.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 1, 1, 1, 1.0f, 1.0f,

        // 左面
        -0.5f, -0.5f, -0.5f, 1, 0, 0, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0, 1, 0, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0, 0, 1, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 1, 1, 0, 0.0f, 1.0f,

        // 右面
        0.5f, -0.5f, 0.5f, 1, 0, 1, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0, 1, 1, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0, 0, 0, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1, 1, 1, 0.0f, 1.0f,

        // 上面
        -0.5f, 0.5f, 0.5f, 1, 0, 0, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0, 1, 0, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0, 0, 1, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 1, 1, 0, 0.0f, 1.0f,

        // 下面
        -0.5f, -0.5f, -0.5f, 1, 0, 1, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0, 1, 1, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0, 0, 0, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 1, 1, 1, 0.0f, 1.0f,
    };

    constexpr unsigned int indices[] = {
        // 前面
        0, 1, 2, 2, 3, 0,
        // 后面
        4, 5, 6, 6, 7, 4,
        // 左面
        8, 9, 10, 10, 11, 8,
        // 右面
        12, 13, 14, 14, 15, 12,
        // 上面
        16, 17, 18, 18, 19, 16,
        // 下面
        20, 21, 22, 22, 23, 20
    };
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    INITED = true;
}

void Slime::draw(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) const {
    glBindVertexArray(VAO);
    shader->use();


    shader->setFloat("alpha", 0.7f);
    const auto newModel = glm::translate(glm::mat4(1.0), glm::vec3(0, 0.2 * std::sin(glfwGetTime() * 6), 0)) * model;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(newModel));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    for(int i = 0; i < 6; ++i){
        if(tex[i] != nullptr) tex[i]->use();
        else glBindTexture(GL_TEXTURE_2D, 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, reinterpret_cast<void *>(sizeof(unsigned int) * 6 * i));
    }

    glBindVertexArray(0);
}

Slime::~Slime() {
    glDeleteVertexArrays(1, &VAO);
}
