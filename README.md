# Game Engine

## Table of Contents

1. [Introduction](#introduction)
2. [Installation](#installation)
   - [Installing Dependencies on Linux](#installing-dependencies-on-linux)
   - [Building with Makefile](#building-with-makefile)
3. [Code Structure](#code-structure)
4. [Game Object Model](#game-object-model)
5. [Timeline System](#timeline-system)
6. [Networking Setup](#networking-setup)
   - [Client-Server Architecture](#client-server-architecture)
   - [Peer-to-Peer (P2P) Architecture](#peer-to-peer-p2p-architecture)
7. [Event Management](#event-driven-architecture)
   - [Events Handled](#events-handled)
8. [Replay System](#replay-system)
9. [JavaScript Scripting with V8](#javascript-scripting-with-v8)
   - [Setting Up V8](#setting-up-v8)
   - [Writing JavaScript Scripts](#writing-javascript-scripts)
   - [Example Usage](#example-usage)
10. [Input Handling](#input-handling)
11. [Movement System](#movement-system)
12. [Games](#games)
13. [Future Scope](#future-scope)

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

## Game Object Model

The Game Object Model revolves around the **Entity** class, which serves as the core building block for all game objects. Each entity possesses various attributes such as position, velocity, maximum velocity, acceleration, color, shape, movement pattern, timeline, and flags indicating whether it is affected by gravity or is movable.

### Composition Over Inheritance

The design follows a component-based architecture, emphasizing composition over inheritance. This approach allows for greater flexibility and modularity.

- **Has-A Relationship:**
  - **Shape:** Each entity has a shape, defined by the `Shape` class.
  - **Movement Pattern:** Entities can have movement patterns, defined by the `MovementPattern` class.
  - **Timeline:** Each entity has its own timeline, anchored to the global timeline.

- **Is-A Relationship:**
  - The entity class does not inherit from other classes but interacts with various components to achieve desired behaviors.

### Component Interaction

While entities possess attributes, they do not directly manipulate them. Instead, specialized components handle specific aspects:

- **Physics System:** Applies gravity and other physics-related effects to entities.
- **EntityManager:** Manages entities and updates their movement patterns.
- **Timeline:** Manages time for each entity, allowing for synchronized and independent time flows.

This design ensures that entities remain lightweight and focused on their core responsibilities, while components handle complex behaviors.

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

The replay system enables recording and replaying of gameplay events, allowing players to capture their game sessions and replay them later. This feature is useful for debugging, analyzing player behavior, or creating gameplay videos.

### Implementation Details

The replay system operates by:

- **Event Recording:** Capturing all events during gameplay by registering a wildcard event handler using the `EventManager`. This ensures every event, regardless of type, is recorded.
- **State Saving:** Storing the initial states of all entities at the start of recording. This includes positions, velocities, and any other relevant properties.
- **Event Replaying:** Re-injecting the recorded events into the event queue at their original timestamps relative to the replay start time.
- **State Restoration:** Before playback, entities are reset to their initial recorded states. After replaying, entities can be restored to their final states if needed.

### Key Components

#### `ReplayRecorder` Class

- **Header File:** `replay_recorder.hpp`
- **Source File:** `replay_recorder.cpp`

The `ReplayRecorder` class manages the recording and replaying process.

**Important Methods:**

- `ReplayRecorder(Timeline *timeline, std::vector<std::shared_ptr<EntityManager>> &entityManagers);`
  - Constructor that initializes the recorder with the main `Timeline` and a list of `EntityManager` instances.

- `void on_event(const Event &event);`
  - Handles events related to starting/stopping recording and playback. Also captures events when recording is active.

- `void start_recording();`
  - Begins recording events and saves the initial state of all entities.

- `void stop_recording();`
  - Stops the recording process.

- `void record_event(const Event &event);`
  - Records an individual event by adjusting its timestamp relative to the recording start time.

- `void play_recording();`
  - Initiates playback by restoring entities to their initial states and scheduling recorded events.

- `void replay_complete();`
  - Called when replaying is finished. Restores entities to their states at the end of the recording session.

#### `EventManager` Modifications

- **Files:** `event_manager.hpp` and `event_manager.cpp`

Enhancements to the `EventManager` support the replay functionality:

- **Wildcard Handlers:**
  - Allows the `ReplayRecorder` to listen to all events without needing to register for each specific event type.

- **Replay Mode:**
  - A `replay_only_mode` flag ensures that during replay, only replay events are processed.
  - Live events are ignored to prevent interference with the replayed events.

- **Replay Event Counting:**
  - Keeps track of how many replay events are left to process.
  - Once all replay events have been handled, the system exits replay mode and triggers a `replay_complete` event.

#### `Timeline` Class Usage

- **Files:** `timeline.hpp` and `timeline.cpp`

The `Timeline` class provides accurate timing for event scheduling:

- **Time Management:**
  - Ensures events are replayed at the correct times by using high-resolution timers.
  - Handles pausing and unpausing, which is essential during recording and playback.

- **Synchronization:**
  - Both the recorder and the event manager use the same timeline to maintain synchronization between recording and replaying.

### Usage Instructions

- **Start Recording:**
  - Press the `Enter` key to begin recording gameplay.
  - The system saves the current state of all entities and starts capturing events.

- **Stop Recording:**
  - Press the `Enter` key again to stop recording.
  - Recording can be stopped at any time, and events up to that point are saved.

- **Play Recording:**
  - Press the `Right Shift` key to start playback.
  - Entities are reset to their initial states, and recorded events are replayed in order.

### Important Notes

- **Entity State Serialization:**
  - Entity states are serialized and deserialized to capture their exact conditions during recording.
  - This includes all necessary properties to fully restore an entity's state.

- **Thread Safety:**
  - Mutexes are used in classes like `Timeline` and `EventManager` to ensure thread-safe operations, as event processing and timing can occur across multiple threads.

- **Replay Constraints:**
  - The replay system is designed for single-session use. Recordings are not persisted to disk and will be lost when the application exits.
  - Extending the system to save and load recordings from files would require additional implementation.

### Relevant Files

- `replay_recorder.hpp` and `replay_recorder.cpp`
- `event_manager.hpp` and `event_manager.cpp`
- `timeline.hpp` and `timeline.cpp`

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

## Input Handling

The game engine handles various types of inputs to control entity actions and game events. This section describes the different input types and how they are processed.

### Input Types

1. **Keyboard Inputs:**
   - **Movement:** `W`, `A`, `S`, `D` keys for moving entities up, left, down, and right respectively.
   - **Dash:** `Left Shift` key combined with direction keys for dashing.
   - **Jump:** `Space` key for jumping.
   - **Pause/Unpause:** `ESC` key to toggle pause.
   - **Speed Control:** `P` key to speed up and `M` key to slow down the game.
   - **Recording Controls:** `Enter` key to start/stop recording and `Right Shift` key to play recording.
   - **Script Execution:** `H` key to run the `hello_world` script and `K` key to run the `player` script.

2. **Mouse Inputs:**
   - Currently, mouse inputs are not implemented but can be added for future features like aiming or clicking.

### Input Handling Classes

#### `InputHandler` Class

- **Header File:** `input_handler.hpp`
- **Source File:** `input_handler.cpp`

The `InputHandler` class processes input events and updates entity states accordingly.

**Important Methods:**

- `InputHandler(Timeline *timeline);`
  - Constructor that initializes the handler with the main `Timeline`.

- `void on_event(const Event &event);`
  - Handles input events and updates entity states.

- `void handle_input(std::shared_ptr<Entity> player, size_t input_type, float acceleration_rate, float jump_force, Vector2 dash_vector);`
  - Applies input logic to the specified entity based on the input type and parameters.

#### `Input` Functions

- **Header File:** `input.hpp`
- **Source File:** `input.cpp`

These functions handle the actual input polling and event raising.

**Important Functions:**

- `void doInput(std::shared_ptr<Entity> entity, Timeline *globalTimeline, ScriptManager *sm, float accelerationRate, float dash_speed, float dash_duration);`
  - Polls for input states and raises corresponding events.

- `void raiseMovementEvent(const std::string &input_type, std::shared_ptr<Entity> entity, float rate, Timeline *timeline);`
  - Helper function to raise movement events based on input.

### Usage

The input handling system integrates with various components:

- **Event Management:** Raises events based on input states, which are then processed by the `EventManager`.
- **Movement System:** Updates entity positions and states based on input events.
- **Scripting:** Allows running scripts based on specific key presses.

### Relevant Files

- `input_handler.hpp` and `input_handler.cpp`
- `input.hpp` and `input.cpp`

## Movement System

The movement system in the game engine handles the movement of entities based on user input, predefined patterns, and physics calculations. This system ensures smooth and realistic movement for all game entities.

### Movement Handling

Movement is managed through the `MovementHandler` class, which processes movement-related events and updates entity positions accordingly.

#### Key Components

- **Header File:** `movement_handler.hpp`
- **Source File:** `movement_handler.cpp`

**Important Methods:**

- `MovementHandler(Timeline *timeline);`
  - Constructor that initializes the handler with the main `Timeline`.

- `void on_event(const Event &event);`
  - Handles movement events and updates entity positions.

- `void handle_movement(std::shared_ptr<Entity> entity);`
  - Applies movement logic to the specified entity, including velocity and acceleration updates.

### Movement Patterns

The `MovementPattern` class defines movement patterns for entities, allowing for complex and predefined movement behaviors.

#### Key Components

- **Header File:** `movement_pattern.hpp`
- **Source File:** `movement_pattern.cpp`

**Important Methods:**

- `MovementPattern();`
  - Constructor that initializes an empty movement pattern.

- `void addStep(const MovementStep &step);`
  - Adds a movement step to the pattern.

- `void update(Entity &entity);`
  - Updates the entity's position based on the current movement step and elapsed time.

#### MovementStep Class

The `MovementStep` class represents a single step in a movement pattern, including velocity, duration, and whether the step is a pause.

**Important Methods:**

- `MovementStep(const Vector2 &vel, float dur, bool pause = false);`
  - Constructor that initializes a movement step with the specified velocity, duration, and pause flag.

### Usage

The movement system integrates with various components:

- **Input Handling:** Processes user input to control entity movement.
- **Physics Calculations:** Ensures realistic movement by applying physics-based calculations.
- **Movement Patterns:** Allows entities to follow predefined movement patterns for complex behaviors.

### Relevant Files

- `movement_handler.hpp` and `movement_handler.cpp`
- `movement_pattern.hpp` and `movement_pattern.cpp`

## Games

The game engine supports various games, each available in different branches. The games demonstrate the engine's capabilities and serve as examples for different genres and features.

### Platformer Games

Platformer games are marked with a `4` and support the event-driven architecture but do not include the replay system. These games showcase the engine's ability to handle complex movement patterns, collision detection, and platform-specific mechanics.

### Non-Platformer Games

Non-platformer games, such as brick breaker, snake, and space invaders, are marked with a `5`. These games support the replay system, allowing players to record and replay their gameplay sessions. They demonstrate the engine's versatility in handling different game mechanics and genres.

**Note:** None of the games currently support scripting, but this feature is planned for future updates.

## Future Scope

The primary focus of this project is to continuously improve the game engine by enhancing the current codebase and adding new features. Future developments may include:

1. **Input Abstraction:**
   - Implementing a more flexible input system to support various input devices and configurations.

2. **Enhanced Scripting Support:**
   - Expanding the scripting capabilities to allow for more complex game logic and interactions.
   - Adding support for additional scripting languages beyond JavaScript.

3. **Performance Optimization:**
   - Improving the engine's performance to handle larger and more complex game worlds.

4. **Networking Enhancements:**
   - Enhancing the networking architecture to support more robust and scalable multiplayer experiences.

By focusing on these areas, the game engine aims to provide a powerful and flexible platform for creating a wide range of games.

## Contributors

- [Atharva Thorve](https://github.com/AtharvaThorve)
- [Rushil Patel](https://github.com/rushildpatel)
- [Divit Kalathil](https://github.com/divitkalathil)
