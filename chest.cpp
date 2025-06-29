//
// Created by admin on 2025/6/30.
//

#include "chest.h"

void Chest::draw(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) {
    const float to = target == State::CLOSE ? glm::radians(90.0f) : glm::radians(0.0f);
    const auto t = static_cast<float>(glfwGetTime());
    if(std::fabs(to - angle) <= glm::radians(90.0f) * v * (t - lastTime)){
        angle = to;
        state = target;
    }
    else{
        const float dir = to - angle > 0 ? 1 : -1;
        angle += dir * glm::radians(90.0f) * v * (t - lastTime);
        state = State::HALF;
    }
    lastTime = t;
    // 盖子
    {
        glm::mat4 newModel(1.0);
        newModel = glm::translate(newModel, glm::vec3(0, 0.125f, -0.5f));
        newModel = glm::rotate(newModel, glm::radians(90.0f) - angle, glm::vec3(-1, 0, 0));
        newModel = glm::scale(newModel, glm::vec3(1, 0.375f, 1));
        newModel = glm::translate(newModel, glm::vec3(0, 0.5f, 0.5f));
        newModel = model * newModel;
        up->draw(newModel, view, projection);
    }
    // 盒子
    {
        glm::mat4 newModel(1.0);
        newModel = glm::scale(newModel, glm::vec3(1, 0.625f, 1));
        newModel = glm::translate(newModel, glm::vec3(0, -0.3f, 0));
        newModel = model * newModel;
        down->draw(newModel, view, projection);
    }
}

void Chest::setTarget(const State target) {
    this->target = target;
    return;
}
