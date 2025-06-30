//
// Created by admin on 2025/6/29.
//

#include "player.h"

#include <chrono>

void Player::draw(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) const {
    if(state == State::STAND){
        // 身体
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(0, 0.05f, 0));
            baseModel = glm::scale(baseModel, glm::vec3(1.0f, 1.1f, 0.5f));
            body->draw(model * baseModel, view, projection);
        }

        // 头
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, +1.0f, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.8f, 0.8f, 0.8f));
            head->draw(model * baseModel, view, projection);
        }

        // 右臂
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, +0.5f, 0));
            // baseModel = glm::rotate(baseModel, walkAngle, glm::vec3(-1.0f, 0, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.3f, 1.0f, 0.3f));
            baseModel = glm::translate(baseModel, glm::vec3(-2.0f, -0.5f, 0));
            rightArm->draw(model * baseModel, view, projection);
        }

        // 左臂
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, +0.5f, 0));
            // baseModel = glm::rotate(baseModel, walkAngle, glm::vec3(+1.0f, 0, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.3f, 1.0f, 0.3f));
            baseModel = glm::translate(baseModel, glm::vec3(+2.0f, -0.5f, 0));
            leftArm->draw(model * baseModel, view, projection);
        }

        // 右腿
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, -0.4f, 0));
            // baseModel = glm::rotate(baseModel, walkAngle, glm::vec3(1.0f, 0, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 1.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0.5f, -0.5f, 0));
            rightLeg->draw(model * baseModel, view, projection);
        }

        // 左腿
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, -0.4f, 0));
            // baseModel = glm::rotate(baseModel, walkAngle, glm::vec3(-1.0f, 0, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 1.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(0.5f, -0.5f, 0));
            leftLeg->draw(model * baseModel, view, projection);
        }
    }
    else if(state == State::WALK){
        const auto walkAngle = static_cast<float>(std::sin((glfwGetTime() - startTime) * KV * v) * glm::radians(30.0f));
        // 模拟步伐角度
        // 身体
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(0, 0.05f, 0));
            baseModel = glm::scale(baseModel, glm::vec3(1.0f, 1.1f, 0.6f));
            body->draw(model * baseModel, view, projection);
        }

        // 头
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, +1.0f, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.8f, 0.8f, 0.8f));
            head->draw(model * baseModel, view, projection);
        }

        // 右臂
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, +0.5f, 0));
            baseModel = glm::rotate(baseModel, walkAngle, glm::vec3(-1.0f, 0, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.3f, 1.0f, 0.3f));
            baseModel = glm::translate(baseModel, glm::vec3(-2.0f, -0.5f, 0));
            rightArm->draw(model * baseModel, view, projection);
        }

        // 左臂
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, +0.5f, 0));
            baseModel = glm::rotate(baseModel, walkAngle, glm::vec3(+1.0f, 0, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.3f, 1.0f, 0.3f));
            baseModel = glm::translate(baseModel, glm::vec3(+2.0f, -0.5f, 0));
            leftArm->draw(model * baseModel, view, projection);
        }

        // 右腿
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, -0.4f, 0));
            baseModel = glm::rotate(baseModel, walkAngle, glm::vec3(1.0f, 0, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 1.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0.5f, -0.5f, 0));
            rightLeg->draw(model * baseModel, view, projection);
        }

        // 左腿
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, -0.4f, 0));
            baseModel = glm::rotate(baseModel, walkAngle, glm::vec3(-1.0f, 0, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 1.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(0.5f, -0.5f, 0));
            leftLeg->draw(model * baseModel, view, projection);
        }
    }
    else if(state == State::FIGHT){
        const auto walkAngle = std::fabs(
            static_cast<float>(std::sin((glfwGetTime() - startTime) * KV * v) * glm::radians(50.0f))); // 模拟步伐角度

        // 身体
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(0, 0.05f, 0));
            baseModel = glm::scale(baseModel, glm::vec3(1.0f, 1.1f, 0.5f));
            body->draw(model * baseModel, view, projection);
        }

        // 头
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, +1.0f, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.8f, 0.8f, 0.8f));
            head->draw(model * baseModel, view, projection);
        }

        // 右臂
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, +0.5f, 0));
            baseModel = glm::rotate(baseModel, walkAngle, glm::vec3(-1.0f, 0, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.3f, 1.0f, 0.3f));
            baseModel = glm::translate(baseModel, glm::vec3(-2.0f, -0.5f, 0));
            rightArm->draw(model * baseModel, view, projection);
        }

        // 左臂
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, +0.5f, 0));
            // baseModel = glm::rotate(baseModel, walkAngle, glm::vec3(+1.0f, 0, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.3f, 1.0f, 0.3f));
            baseModel = glm::translate(baseModel, glm::vec3(+2.0f, -0.5f, 0));
            leftArm->draw(model * baseModel, view, projection);
        }

        // 右腿
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, -0.4f, 0));
            // baseModel = glm::rotate(baseModel, walkAngle, glm::vec3(1.0f, 0, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 1.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0.5f, -0.5f, 0));
            rightLeg->draw(model * baseModel, view, projection);
        }

        // 左腿
        {
            glm::mat4 baseModel(1.0f);
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 0.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(-0, -0.4f, 0));
            // baseModel = glm::rotate(baseModel, walkAngle, glm::vec3(-1.0f, 0, 0));
            baseModel = glm::scale(baseModel, glm::vec3(0.5f, 1.5f, 0.5f));
            baseModel = glm::translate(baseModel, glm::vec3(0.5f, -0.5f, 0));
            leftLeg->draw(model * baseModel, view, projection);
        }
    }
    else
        assert(0);
}

void Player::setState(const State s) {
    if(s == state) return;
    state = s;
    startTime = static_cast<float>(glfwGetTime());
}

void Player::setSpeed(const float v) {
    this->v = v;
}

float Player::getSpeed() const {
    return v;
}

Player::State Player::getState() const {
    return state;
}

void PlayerController::setSpeed(const float v) const {
    player->setSpeed(v);
}

void PlayerController::addAction(const std::tuple<int, int, int> &op) {
    actionQueue.emplace_back(op);
}

void PlayerController::changeAction(const std::tuple<int, int, int> &op) {
    if(actionQueue.empty()) actionQueue.emplace_back(op);
    else{
        actionQueue.pop_back();
        actionQueue.emplace_back(op);
    }
}

void PlayerController::update(const glm::mat4 &view, const glm::mat4 &projection) {
    constexpr float eps = 1e-3;
    const auto t = static_cast<float>(glfwGetTime());
    if(isAvailable()){
        lastStartTime = t;
        if(actionQueue.empty()){
            currentOption = {-1, -1, -1};
        }
        else{
            currentOption = actionQueue.front();
            actionQueue.pop_front();
        }
    }
    auto genModelMat = [&](const std::pair<float, float> &pos, const std::pair<int, int> &dir) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(pos.second, PLAYER_HEIGHT, pos.first));
        // 向前
        if(dir.first == 1 && dir.second == 0){
            model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 1.0f, 0));
        }
        // 向后
        else if(dir.first == -1 && dir.second == 0){
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1.0f, 0));
        }
        // 向右
        else if(dir.first == 0 && dir.second == 1){
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1.0f, 0));
        }
        // 向左
        else if(dir.first == 0 && dir.second == -1){
            model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0, 1.0f, 0));
        }
        else{
            assert(0);
        }
        return model;
    };
    auto draw = [&]() {
        const auto [op, u, v] = currentOption;
        if(op == -1){
            player->setState(PlayerState::STAND);
            player->draw(genModelMat(pos, direction), view, projection);
            return;
        }
        auto &[pu, pv] = pos;
        //移动
        if(op == 1){
            assert(pu == u || pv == v);
            player->setState(PlayerState::WALK);
            if(pu == u && pv == v){
                player->draw(genModelMat(pos, direction), view, projection);
                return;
            }
            if(pu == u) direction = {0, v > pv ? 1 : -1};
            else direction = {u > pu ? 1 : -1, 0};
            if(player->getSpeed() * (t - lastStartTime) >= static_cast<float>(std::abs(u - pu) + std::abs(v - pv))){
                pu = u;
                pv = v;
                currentPos = pos;
                player->draw(genModelMat(pos, direction), view, projection);
                return;
            }
            std::pair<float, float> tpos{
                static_cast<float>(pu) + static_cast<float>(direction.first) * player->getSpeed() * (t - lastStartTime),
                static_cast<float>(pv) + static_cast<float>(direction.second) * player->getSpeed() * (t - lastStartTime)
            };
            player->draw(genModelMat(tpos, direction), view, projection);
            currentPos = tpos;
            return;
        }
        //攻击
        if(op == 2){
            if((t - lastStartTime) * KV * player->getSpeed() >= std::numbers::pi){
                player->setState(PlayerState::STAND);
            }
            else{
                player->setState(PlayerState::FIGHT);
            }
            player->draw(genModelMat(pos, direction), view, projection);
            return;
        }
        // 转向
        if(op == 3){
            if(u == 1 && v == 0 || u == -1 && v == 0 || u == 0 && v == 1 || u == 0 && v == -1){
            }
            direction = {u, v};
            player->draw(genModelMat(pos, direction), view, projection);
            return;
        }
    };
    draw();
}

bool PlayerController::isAvailable() const {
    const auto [op, u, v] = currentOption;
    const auto [pu, pv] = pos;
    const auto t = static_cast<float>(glfwGetTime());
    if(op == -1) return true;
    //移动
    if(op == 1){
        return pu == u && pv == v;
    }
    //攻击
    if(op == 2){
        return (t - lastStartTime) * KV * player->getSpeed() >= std::numbers::pi && player->getState() == PlayerState::STAND;
    }
    // 转向
    if(op == 3){
        return true;
    }
    assert(0);
}

std::pair<float, float> PlayerController::getCurrentPos() const {
    return currentPos;
}

bool PlayerController::isQueueEmpty() const {
    return actionQueue.empty();
}

void PlayerController::reset(const std::pair<int, int> &pos, const std::pair<int, int> &dir) {
    this->pos = pos;
    currentPos = pos;
    actionQueue.clear();
    currentOption = {-1, -1, -1};
    direction = dir;
}
