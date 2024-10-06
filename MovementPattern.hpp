#pragma once
#include "structs.hpp"
#include <vector>

class Entity;

class MovementStep {
public:
    Vector2 velocity = { 0, 0 };
    float duration;
    bool isPause;

    MovementStep(const Vector2& vel, float dur, bool pause = false);
};

class MovementPattern {
private:
    std::vector<MovementStep> steps;
    size_t currentStepIndex;
    float timeElapsed;
    
    void addStepsHelper() {}

    template<typename T, typename... Args>
    void addStepsHelper(T&& firstStep, Args&&... restSteps) {
        addStep(std::forward<T>(firstStep));
        addStepsHelper(std::forward<Args>(restSteps)...);
    }

public:
    MovementPattern();

    void addStep(const MovementStep& step);

    void update(Entity& entity);

    template<typename... Args>
    void addSteps(Args&&... args) {
        addStepsHelper(std::forward<Args>(args)...);
    }


};