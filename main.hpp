#pragma once
#include "camera.hpp"
#include "cleanup.hpp"
#include "client.hpp"
#include "collision_utils.hpp"
#include "draw.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"
#include "init.hpp"
#include "input.hpp"
#include "physics.hpp"
#include "scaling.hpp"
#include "server.hpp"
#include "structs.hpp"
#include "timeline.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <thread>

// SDL render and window context
extern App *app;

extern Timeline globalTimeline;

int main(int argc, char *argv[]);