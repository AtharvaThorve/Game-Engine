# Game Engine

This is a C++ based project designed as a Visual Studio solution for a Game Engine. Detailed setup and usage instructions are provided below.

## Installation

1. Open the solution in Visual Studio.
2. In the Solution Explorer (inside Visual Studio), double-click on the `Game-Engine.sln` file.
3. Open the terminal inside Visual Studio and run the following commands:
   * `vcpkg integrate install`
   * `vcpkg install`
4. Build the project.

**Note:** Scaling can be toggled by pressing the `Left Shift` key. By default, scaling is not enabled (constant size).

## Timeline Controls

The following features are available for controlling the timeline in the game:

1. **Pause/Unpause** can be toggled using the `ESC` key.
2. **Speed up** the game with the `P` key.
3. **Slow down** the game with the `M` key.

## Networking Setup

We support both **client-server** and **P2P** architectures. The required `libzmq.dll` file is already included in the `x64` folder, so there's no need to manually install it. We followed instructions provided here and installed version 4.3.5.

### Client-Server Architecture
1. Start the server by passing `server` as the command-line argument.
2. Start the client by passing `client` as the command-line argument.

### Peer-to-Peer (P2P) Architecture
1. Start a listen-server by passing `client server` or `server client` as the command-line arguments.
2. Start clients by passing `client P2P` or `P2P client` as the command-line arguments.

**Note:** While starting the P2P clients, ensure that the clients are binding and connecting to the correct IP addresses and ports.

## Project Workflow Updates

In this project, we worked primarily with the **server-client** architecture rather than P2P. Additionally, instead of using Visual Studio this time, we switched to **WSL (Windows Subsystem for Linux)** for building and running the project.

### Building with Makefile in WSL

1. To build the project, navigate to the project directory using WSL and use the following commands:
   * `make clean` - Cleans the existing build files.
   * `make` - Compiles the project.

### Running the Server and Client

* Start the server using:
```bash
./main server
```

* Start the client using:
```bash
./main client
```

## Event Management has been added
So we have `event.hpp` that contains generic event related information. We also have `event_manager.hpp` and `event_manager.cpp`, that takes care of queueing and passing events to respective handlers. Finally, we have handlers for different types of events.

Different events being handled:
1. Input
2. Collision
3. Death
4. Respawn
5. Disconnect (Networked)
6. Movement
7. Replay
8. Position (Networked)

**Note:** We also have added a new type of movement (dash), which can be performed by pressing LShift and any direction key (WASD). The dash can also be performed in diagonal directions by pressing two directions at the same time.

## Replay System has been added
So we have `replay_recorder.cpp` that contains the logic for recording and replaying events. We also made changes to `event_manager.cpp` file to be able to register wildcard handlers and process replay events properly.

**Note:** The recording can be started and stopped by pressing the ***Enter/Return*** key and the recording can be played back by pressing the ***RShift*** key.

## JavaScript Scripting with V8

We have integrated the V8 JavaScript engine to allow for scripting using JavaScript. This enables developers to write game logic in JavaScript, which can be loaded and executed at runtime. Additionally, we can pass player shared pointers, player positions, and raise new events from the scripts.

### Setting Up V8

1. Ensure that the V8 engine is included in your project. The necessary files are already included in the `third_party` directory.
2. Include the V8 headers and libraries in your project.
```bash
sudo apt update && sudo apt -y install build-essential libnode-dev
```

### Writing JavaScript Scripts

JavaScript scripts should be placed in the `scripts` directory. Here are examples of two simple JavaScript scripts:

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