//
// Created by admin on 2025/6/30.
//

#ifndef CH_H
#define CH_H
#include <map>

#include "glad/glad.h"
#include "shader.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include <iostream>

#include "glm/ext/matrix_clip_space.hpp"

#include FT_FREETYPE_H

namespace ch{
    struct Character {
        GLuint     TextureID;  // 字形纹理的ID
        glm::ivec2 Size;       // 字形大小
        glm::ivec2 Bearing;    // 从基准线到字形左部/顶部的偏移值
        GLuint     Advance;    // 原点距下一个字形原点的距离
    };

    inline std::map<GLchar, Character> Characters;
    void init();
    inline void load(const char *filePath = "../fonts/arial.ttf") {

        FT_Library ft;
        if (FT_Init_FreeType(&ft))
            std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

        FT_Face face;
        if (FT_New_Face(ft, filePath, 0, &face))
            std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

        FT_Set_Pixel_Sizes(face, 0, 48);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //禁用字节对齐限制
        for (GLubyte c = 0; c < 128; c++)
        {
            // 加载字符的字形
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // 生成纹理
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // 设置纹理选项
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // 储存字符供之后使用
            Character character{
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<GLuint>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<GLchar, Character>(c, character));
        }
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }

    inline GLuint VAO, VBO;
    inline bool INIDED = false;
    inline Shader *chShader = nullptr;

    inline void init(const float screenWidth, const float screenHeight) {
        if(INIDED) return;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        chShader = new Shader("../shader/ch_shader.vs", "../shader/ch_shader.fs");
        glm::mat4 projection = glm::ortho(0.0f, screenWidth, 0.0f, screenHeight);
        chShader->use();
        glUniformMatrix4fv(glGetUniformLocation(chShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        INIDED = true;
    }

    inline void RenderText(std::string text, GLfloat x, const GLfloat y, const GLfloat scale, const glm::vec3 color, const Shader &s = *chShader){
        assert(INIDED);
        // 激活对应的渲染状态
        s.use();
        glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(VAO);

        // 遍历文本中所有的字符
        for (std::string::const_iterator c = text.begin(); c != text.end(); ++c)
        {
            const Character ch = Characters[*c];

            const GLfloat xpos = x + ch.Bearing.x * scale;
            const GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            const GLfloat w = ch.Size.x * scale;
            const GLfloat h = ch.Size.y * scale;
            // 对每个字符更新VBO
            const GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
            };
            // 在四边形上绘制字形纹理
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // 更新VBO内存的内容
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // 绘制四边形
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // 更新位置到下一个字形的原点，注意单位是1/64像素
            x += (ch.Advance >> 6) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

}



#endif //CH_H
