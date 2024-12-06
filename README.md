# Game Engine

## Table of Contents

1. [Introduction](#introduction)
2. [Installation](#installation)
   - [Installing Dependencies on Linux](#installing-dependencies-on-linux)
   - [Building with Makefile](#building-with-makefile)
3. [Timeline System](#timeline-system)
4. [Networking Setup](#networking-setup)
   - [Client-Server Architecture](#client-server-architecture)
   - [Peer-to-Peer (P2P) Architecture](#peer-to-peer-p2p-architecture)
5. [Event Management](#event-driven-architecture)
   - [Events Handled](#events-handled)
6. [Replay System](#replay-system)
7. [JavaScript Scripting with V8](#javascript-scripting-with-v8)
   - [Setting Up V8](#setting-up-v8)
   - [Writing JavaScript Scripts](#writing-javascript-scripts)
   - [Example Usage](#example-usage)
8. [Code Structure](#code-structure)
9. [Usage Examples](#usage-examples)
10. [Games](#games)
11. [Future Scope](#future-scope)

## Introduction

This is a C++ project for a Game Engine that can be run on any Linux-based system. The engine supports features such as event management, networking, and scripting with the V8 JavaScript engine.

## Installation

To run this project, ensure that the following dependencies are installed:

- **SDL2**
- **ZeroMQ (ZMQ)**
- **libnode-dev** (for V8 JavaScript engine)

### Installing Dependencies on Linux

1. Update the package list:

   ```bash
   sudo apt update
   ```

2. Install SDL2:

   ```bash
   sudo apt install libsdl2-dev
   ```

3. Install ZeroMQ:

   ```bash
   sudo apt install libzmq3-dev
   ```

4. Install libnode-dev:

   ```bash
   sudo apt install libnode-dev
   ```

### Building with Makefile

1. Navigate to the project directory in your terminal.
2. Use the following commands to build the project:

   ```bash
   make clean   # Cleans the existing build files.
   make         # Compiles the project.
   ```

### Running the Server and Client

- **Start the Server:**

  ```bash
  ./main server
  ```

- **Start the Client:**

  ```bash
  ./main client
  ```

**Note:** Scaling can be toggled by pressing the `Left Shift` key. By default, scaling is not enabled (constant size).

## Timeline System

The Timeline system manages time within the game engine, allowing for precise control over game events, synchronization, and time scaling. It is especially useful for features like pausing the game, time dilation, and syncing events in networked multiplayer environments.

### Implementation Details

The Timeline is implemented using high-resolution timers to accurately track elapsed time. Depending on the compiler and platform, it utilizes appropriate methods:

- **Windows (MSVC Compiler):**
  - Uses the `__rdtsc()` intrinsic to read the CPU's timestamp counter.
- **GCC Compiler:**
  - Uses inline assembly to execute the `rdtsc` instruction directly.

This approach ensures a consistent and high-resolution time source across different systems.

Key aspects of the `Timeline` class:

- **Time Retrieval:** Provides the current time since the timeline started, adjusted for any pauses and time scaling.
- **Pausing and Unpausing:** Allows pausing and resuming the timeline without affecting the overall time flow.
- **Time Scaling (Tic):** Adjusts the speed at which time progresses, enabling effects like slow motion or fast forward.
- **Anchoring:** Timelines can be anchored to other timelines, creating a hierarchy that allows for synchronized or relative time flows.

### Key Methods

- `getTime()`: Returns the current time, accounting for pauses and tics.
- `pause()`: Pauses the timeline.
- `unpause()`: Unpauses the timeline.
- `changeTic(int new_tic)`: Changes the tic value to scale the timeline speed.
- `isPaused()`: Checks if the timeline is currently paused.
- `getTic()`: Retrieves the current tic value.
- `getAnchorTic()`: Retrieves the tic value from the anchor timeline, if any.

### Usage

The Timeline system integrates with various components:

- **Game Loop:** Controls the timing for updates and rendering.
- **Event Scheduling:** Schedules events based on the game time.
- **Animation and Physics:** Ensures smooth animations and physics calculations by providing consistent time steps.
- **Network Synchronization:** Helps synchronize actions between clients and servers in multiplayer modes.

### Relevant Files

- `timeline.hpp`
- `timeline.cpp`

### Timeline Controls

1. **Pause/Unpause:** Toggle with the `ESC` key.
2. **Speed Up:** Press the `P` key.
3. **Slow Down:** Press the `M` key.

## Networking Setup

We support both **client-server** and **Peer-to-Peer (P2P)** architectures. The required `libzmq.dll` file is included in the `x64` folder, so no manual installation is needed.

**Note:** The Peer-to-Peer (P2P) architecture is currently deprecated and not supported, but it remains included in the project for reference.

### Client-Server Architecture

1. **Start the Server:**
   ```bash
   ./main server
   ```
2. **Start the Client:**
   ```bash
   ./main client
   ```

### Peer-to-Peer (P2P) Architecture

1. **Start a Listen-Server:**
   ```bash
   ./main server client
   ```
2. **Start a P2P Client:**
   ```bash
   ./main client P2P
   ```

**Note:** Ensure that clients are binding and connecting to the correct IP addresses and ports when starting P2P clients.

### Network Architecture
![Network Architecture](<Network Architecture.png>)

## Event-Driven Architecture

The game engine utilizes an event-driven architecture to handle various game events efficiently. This design allows for decoupled communication between different parts of the engine, making the system more modular and maintainable.

### Event Manager

The **Event Manager** is responsible for:

- **Queueing Events:** It collects events generated during the game loop.
- **Dispatching Events:** It distributes events to registered handlers based on event types.
- **Managing Handlers:** It allows registering and unregistering of event handlers, including wildcard handlers that can listen to multiple event types.

Relevant files:

- `event_manager.hpp`
- `event_manager.cpp`
- `event.hpp`

### Event Handlers

Event handlers are functions or methods that respond to specific events. Each handler is associated with one or more event types.

#### Events Handled

1. **Input Events:** Handle user inputs such as keyboard and mouse actions.
2. **Collision Events:** Triggered when entities collide within the game environment.
3. **Death Events:** Occur when an entity (e.g., a player or enemy) is destroyed.
4. **Respawn Events:** Manage the logic for respawning entities after death.
5. **Disconnect Events (Networked):** Handle network disconnections of clients.
6. **Movement Events:** Respond to movement commands, including normal movement and dashing.
7. **Replay Events:** Used by the replay system to record and playback gameplay.
8. **Position Events (Networked):** Synchronize entity positions across the network.

**Note:** The dash movement can be performed by pressing `Left Shift` along with any direction key (`W`, `A`, `S`, `D`). Diagonal dashes are possible by pressing two direction keys simultaneously.

### Adding New Events

To integrate new events into the system:

1. **Define the Event Handler:**
   - Define what needs to happen when that event takes place.
2. **Update Event:**
   - If needed update the variant type of event to include different data types that can be passed as parameters.
3. **Register the Handler:**
   - Use the Event Manager to register the new handler for the event type.

### Benefits of Event-Driven Architecture

- **Modularity:** Components can be developed and tested independently.
- **Scalability:** Easy to add new features without affecting existing code.
- **Maintainability:** Simplifies debugging and updates by isolating event logic.

## Replay System

The replay system allows recording and replaying of events.

- **Recording Controls:**
  - Start/Stop Recording: Press the `Enter` key.
  - Playback Recording: Press the `Right Shift` key.

Relevant files include `replay_recorder.cpp` and updates to `event_manager.cpp` to support wildcard handlers and replay events.

## JavaScript Scripting with V8

Integrate the V8 JavaScript engine to enable scripting within the game engine.

### Setting Up V8

1. Ensure the V8 engine is included in the `third_party` directory.
2. Include V8 headers and libraries in your project:
   ```bash
   sudo apt update && sudo apt -y install build-essential libnode-dev
   ```

### Writing JavaScript Scripts

Scripts should be placed in the `scripts` directory. They can access player data and raise events.

#### Example 1: Print Hello

```javascript
// scripts/hello_world.js
print('Hello World from V8 JavaScript Engine!');
```

#### Example 2: Print Player Location and Raise Death Event

```javascript
// scripts/player.js
function runScript() {
    print(player.position.x);
    print(player.position.y);

    const params = {
        player: player,
    };

    raise_event("death", params);
}

runScript();
```

### Example Usage

Here is an example of how to use the JavaScript scripting system in the game engine:

1. Create a JavaScript script in the `scripts` directory.
2. Load and execute the script in your C++ code during the appropriate game events (e.g., start, update, end).
```C++
sm->addScript("player", "scripts/player.js");
```
Make sure there are proper bindings present between the V8 Javascript engine and C++ engine. Refer to the `entity_bindings.cpp` and `event_manager_bindings.cpp` files for more information on this.

By using JavaScript scripting with V8, you can easily modify game behavior without recompiling the entire project, making the development process more efficient and flexible.

## Code Structure

The codebase is organized into several key components:

- **Main Modules**
  - `main.cpp` and `main.hpp`: Entry point of the application.
  - `init.cpp` and `init.hpp`: Initialization routines for the game engine.
  - `cleanup.cpp` and `cleanup.hpp`: Handles resource cleanup.

- **Entities and Components**
  - `Entity.hpp` and `Entity.cpp`: Defines the base entity class.
  - `EntityManager.hpp` and `EntityManager.cpp`: Manages all entities within the game.
  - `MovementPattern.hpp` and `MovementPattern.cpp`: Defines movement patterns for entities.

- **Graphics and Rendering**
  - `draw.cpp` and `draw.hpp`: Functions for rendering entities and shapes.
  - `Shape.hpp` and `Shape.cpp`: Defines geometric shapes used in the game.
  - `Camera.hpp` and `Camera.cpp`: Implements camera functionality.
  - `scaling.hpp` and `scaling.cpp`: Handles scaling transformations.

- **Input and Physics**
  - `input.cpp` and `input.hpp`: Handles user input.
  - `Physics.hpp` and `Physics.cpp`: Implements physics calculations.
  - `collision_utils.hpp` and `collision_utils.cpp`: Contains collision detection utilities.

- **Networking**
  - `Server.hpp` and `Server.cpp`: Server-side networking implementation.
  - `Client.hpp` and `Client.cpp`: Client-side networking implementation.
  - **Note:** The P2P architecture is currently deprecated but remains in the codebase.

- **Event Management**
  - `event.hpp`: Contains generic event-related definitions.
  - `event_manager.hpp` and `event_manager.cpp`: Manages event queueing and dispatching.

- **Scripting**
  - `script_manager.hpp` and `script_manager.cpp`: Manages JavaScript scripting with V8.
  - `v8helpers.hpp` and `v8helpers.cpp`: Helper functions for integrating V8 scripts.

- **Utilities**
  - `.clang-format`: Coding style guidelines for consistent formatting.
  - `defs.hpp`: Contains macro definitions and global constants.
  - `structs.hpp` and `structs.cpp`: Defines common data structures.

## Usage Examples

### Running the Game

To start the game with default settings:
```bash
./main client
```

## Games

Different games are present in different branches. Since the engine was built iteratively, the different games might not include all the functionalities.

**Note:** Games that are marked with 4 are platformer games, that support event driven architecture but not the replay system. Games that are marked with 5 are non platformer games such as brick breaker, snake, space invaders, etc. These games support replay system.

**Note:** None of the games support scripting yet.

## Future scope
1. The main focus of this project would **always** be related to improvement of the engine by improving current codebase or by adding newer features. Features such as:
    1. Input Abstraction
    2. Adding more support for scripting
    3. Adding support for different scripting languages