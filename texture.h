//
// Created by admin on 2025/6/29.
//

#ifndef TEXTRUE_H
#define TEXTRUE_H

#include "stb_image.h"
#include <glad/glad.h>

class Texture {
    unsigned int ID;
public:
    explicit Texture(const char* filePath);
    ~Texture();
    void use() const;
};



#endif //TEXTRUE_H
