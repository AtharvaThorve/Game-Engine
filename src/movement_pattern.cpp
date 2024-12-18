#include "movement_pattern.hpp"
#include "entity.hpp"

MovementStep::MovementStep(const Vector2 &vel, float dur, bool pause) : velocity(vel), duration(dur), isPause(pause) {}

MovementPattern::MovementPattern() : currentStepIndex(0), timeElapsed(0) {}

void MovementPattern::addStep(const MovementStep &step)
{
    steps.push_back(step);
}

void MovementPattern::update(Entity &entity)
{
    if (steps.empty())
        return;

    float deltaTime = entity.deltaTime;
    timeElapsed += deltaTime;
    MovementStep &step = steps[currentStepIndex];

    if (!step.isPause)
    {
        entity.velocity.x = step.velocity.x;
        entity.velocity.y = step.velocity.y;
    }

    if (timeElapsed >= step.duration)
    {
        timeElapsed = 0;
        currentStepIndex = (currentStepIndex + 1) % steps.size();
        entity.velocity.x = 0;
        entity.velocity.y = 0;
    }
}