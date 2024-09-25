#pragma once
#include "structs.hpp"
#include "init.hpp"
#include "draw.hpp"
#include "input.hpp"
#include "Physics.hpp"
#include "Entity.hpp"
#include "cleanup.hpp"
#include <memory>
#include <thread>
#include "SDL2/SDL.h"
#include "scaling.hpp"
#include "EntityManager.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <iostream>
#include "Timeline.hpp"

// SDL render and window context
extern App* app;

//EntityManager entityManager;
extern Timeline globalTimeline;
//PhysicsSystem physicsSystem(0.0f, 10.0f);

int main(int argc, char* argv[]);

void setRenderScale(float scaleX, float scaleY);