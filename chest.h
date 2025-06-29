//
// Created by admin on 2025/6/30.
//

#ifndef CHEST_H
#define CHEST_H

#include "object.h"
#include "cube.h"
#include "GLFW/glfw3.h"

enum class ChestState {
    OPEN, CLOSE, HALF
};

class Chest : public Object {
    const Cube *up, *down;
    using State = ChestState;
    State state;
    float angle;
    State target;
    float lastTime;
    float v;
public:
    explicit Chest(const Cube *up, const Cube *down)
        : up(up), down(down), state(State::CLOSE), angle(glm::radians(90.0f)), target(State::CLOSE), lastTime(static_cast<float>(glfwGetTime())), v(1.0) {
    }
    void draw(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) override;
    void setTarget(State target);

};



#endif //CHEST_H
