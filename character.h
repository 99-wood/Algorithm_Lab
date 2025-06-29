//
// Created by admin on 2025/6/29.
//

#ifndef CHARACTER_H
#define CHARACTER_H
#include <queue>

#include "object.h"
#include "cube.h"
#include "GLFW/glfw3.h"

constexpr int KV = 6;

enum class CharacterState {
    STAND, WALK, FIGHT
};

class Character : public Object {
    using State = CharacterState;
    const Cube *head, *body, *leftArm, *rightArm, *leftLeg, *rightLeg;
    // mutable float walkAngle = 0.0f; // 用于动画角度
    float startTime;
    State state;
    float v;

public:
    explicit Character(const Cube *head, const Cube *body, const Cube *leftArm, const Cube *rightArm, const Cube *leftLeg, const Cube *rightLeg)
        : head(head), body(body), leftArm(leftArm), rightArm(rightArm), leftLeg(leftLeg), rightLeg(rightLeg),
          startTime(static_cast<float>(glfwGetTime())), state(State::STAND), v(1.0) {
    }
    explicit Character(const Texture *tex)
        : startTime(static_cast<float>(glfwGetTime())), state(State::STAND), v(1.0) {
        const auto cube = new Cube(tex);
        head = cube, body = cube, leftArm = cube, rightArm = cube, leftLeg = cube, rightLeg = cube;
    }

    void draw(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) const override;

    void setState(State s);

    void setSpeed(float v);

    [[nodiscard]] float getSpeed() const;

    [[nodiscard]] State getState() const;
};

constexpr float PLAYER_HEIGHT = 1.35f;

class CharacterController {
private:
    Character *player;
    std::deque<std::tuple<int, float, float> > actionQueue;
    std::tuple<int, int, int> currentOption;
    std::pair<int, int> pos;
    std::pair<float, float> currentPos;
    std::pair<int, int> direction;
    float lastStartTime;

public:
    explicit CharacterController(Character *player, const std::pair<int, int> &pos,
                                 const std::pair<int, int> &dir = {1, 0}) : player(player), currentOption{-1, -1, -1},
                                 pos(pos), direction(dir),lastStartTime(static_cast<float>(glfwGetTime())) {
        currentPos = pos;
    }

    void setSpeed(float v) const;

    void addAction(const std::tuple<int, int, int> &op);

    void changeAction(const std::tuple<int, int, int> &op);

    void update(const glm::mat4 &view, const glm::mat4 &projection);

    bool isAvailable() const;

    [[nodiscard]] std::pair<float, float> getCurrentPos() const;

    [[nodiscard]] bool isQueueEmpty() const;
};


#endif //CHARACTER_H
