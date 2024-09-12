#pragma once
#include "structs.hpp"
#include <vector>

class Entity;

class MovementStep {
public:
	Vector2 direction;
	float duration;
	bool isPause;

	MovementStep(const Vector2& dir, float dur, bool pause = false);
};

class MovementPattern {
private:
	std::vector<MovementStep> steps;
	size_t currentStepIndex;
	float timeElapsed;

public:
	MovementPattern();

	void addStep(const MovementStep& step);

	void update(float deltaTime, Entity& entity);
};