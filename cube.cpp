//
// Created by admin on 2025/6/29.
//

#include "cube.h"
unsigned Cube::VBO = 0;
unsigned Cube::EBO = 0;
unsigned Cube::VAO = 0;
int Cube::modelLoc = 0, Cube::viewLoc = 0, Cube::projectionLoc = 0;
bool Cube::INITED = false;
Shader *Cube::shader = nullptr;

void Cube::init(const char *vertexPath, const char *fragmentPath) {
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
    glGenVertexArrays(1, const_cast<unsigned int*>(&VAO));
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    INITED = true;
}

void Cube::draw(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) const {
    glBindVertexArray(VAO);
    shader->use();
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    for(int i = 0; i < 6; ++i){
        if(tex[i] != nullptr) tex[i]->use();
        else glBindTexture(GL_TEXTURE_2D, 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, reinterpret_cast<void *>(sizeof(unsigned int) * 6 * i));
    }

    glBindVertexArray(0);
}

Cube::~Cube() {
    glDeleteVertexArrays(1, &VAO);
}
