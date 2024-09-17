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

// SDL render and window context
extern App* app;

int main(int argc, char* argv[]);

void setRenderScale(float scaleX, float scaleY);