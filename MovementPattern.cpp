#include "MovementPattern.hpp"
#include "entity.hpp"
#include <iostream>

MovementStep::MovementStep(const Vector2& dir, float dur, bool pause) : 
	direction(dir), duration(dur), isPause(pause) {}

MovementPattern::MovementPattern() : 
	currentStepIndex(0), timeElapsed(0) {}

void MovementPattern::addStep(const MovementStep& step) {
	steps.push_back(step);
}

void MovementPattern::update(float deltaTime, Entity& entity) {
	if (steps.empty())
		return;

	timeElapsed += deltaTime;
	MovementStep& step = steps[currentStepIndex];

	if (!step.isPause) {
		entity.patternVelocity.x += step.direction.x * deltaTime;
		entity.patternVelocity.y += step.direction.y * deltaTime;
	}

	if (timeElapsed >= step.duration) {
		timeElapsed = 0;
		currentStepIndex = (currentStepIndex + 1) % steps.size();
		entity.patternVelocity.x = 0;
		entity.patternVelocity.y = 0;
	}
}