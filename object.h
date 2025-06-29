//
// Created by admin on 2025/6/29.
//

#ifndef OBJECT_H
#define OBJECT_H
#include "glm/fwd.hpp"


class Object {
public:
    virtual ~Object() = default;

    virtual void draw(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) const {
        assert(0);
    }
    virtual void draw(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) {
        assert(0);
    }
};



#endif //OBJECT_H
