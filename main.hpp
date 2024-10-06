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
#include "Camera.hpp"

// SDL render and window context
extern App* app;

extern Timeline globalTimeline;

int main(int argc, char* argv[]);