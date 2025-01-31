#pragma once
#include "camera.hpp"
#include "cleanup.hpp"
#include "client.hpp"
#include "collision_handler.hpp"
#include "collision_utils.hpp"
#include "death_handler.hpp"
#include "disconnectHandler.hpp"
#include "draw.hpp"
#include "entity.hpp"
#include "entity_bindings.hpp"
#include "entity_manager.hpp"
#include "event_manager.hpp"
#include "event_manager_bindings.hpp"
#include "init.hpp"
#include "input.hpp"
#include "input_handler.hpp"
#include "movement_handler.hpp"
#include "physics.hpp"
#include "position_handler.hpp"
#include "replay_recorder.hpp"
#include "respawn_handler.hpp"
#include "scaling.hpp"
#include "script_manager.hpp"
#include "server.hpp"
#include "structs.hpp"
#include "timeline.hpp"
#include "v8helpers.hpp"
#include <SDL2/SDL.h>
#include <atomic>
#include <iostream>
#include <libplatform/libplatform.h>
#include <memory>
#include <thread>
#include <v8.h>

// SDL render and window context
extern App *app;

extern Timeline globalTimeline;
extern std::atomic<bool> terminateThreads;

int main(int argc, char *argv[]);