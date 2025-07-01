//
// Created by admin on 2025/6/29.
//

#ifndef CUBE_H
#define CUBE_H

#include <glad/glad.h>
#include "texture.h"
#include "shader.h"
#include "object.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Cube : public Object{
    static unsigned int VAO, VBO, EBO;
    static Shader *shader;
    static bool INITED;
    static int modelLoc, viewLoc, projectionLoc;
    // const unsigned int VAO;
    const Texture* tex[6];

public:
    static void init(const char *vertexPath = "../shader/cube_shader.vs", const char *fragmentPath = "../shader/cube_shader.fs");
    explicit Cube(const Texture *texture = nullptr) {
        tex[0] = tex[1] = tex[2] = tex[3] = tex[4] = tex[5] = texture;
        assert(INITED);
        glGenVertexArrays(1, const_cast<unsigned int*>(&VAO));
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);         // ⭐ 绑定 VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // ⭐ 绑定 EBO
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void *>(nullptr));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
    Cube(const Texture *texture0, const Texture *texture1, const Texture *texture2, const Texture *texture3, const Texture *texture4, const Texture *texture5) {
        tex[0] = texture0;
        tex[1] = texture1;
        tex[2] = texture2;
        tex[3] = texture3;
        tex[4] = texture4;
        tex[5] = texture5;
        assert(INITED);
        // glGenVertexArrays(1, const_cast<unsigned int*>(&VAO));
        // glBindVertexArray(VAO);
        //
        // glBindBuffer(GL_ARRAY_BUFFER, VBO);         // ⭐ 绑定 VBO
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // ⭐ 绑定 EBO
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void *>(nullptr));
        // glEnableVertexAttribArray(0);
        // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        // glEnableVertexAttribArray(1);
        // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
        // glEnableVertexAttribArray(2);
    }

    void draw(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) const override;
    ~Cube() override;
};

#endif //CUBE_H
