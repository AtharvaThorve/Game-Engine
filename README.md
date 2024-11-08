# Game Engine

This is a C++ based project designed as a Visual Studio solution for a Game Engine. Detailed setup and usage instructions are provided below.

## Installation

1. Open the solution in Visual Studio.
2. In the Solution Explorer (inside Visual Studio), double-click on the `Game-Engine.sln` file.
3. Open the terminal inside Visual Studio and run the following commands:
   * `vcpkg integrate install`
   * `vcpkg install`
4. Build the project.

**Note:** Scaling can be toggled by pressing the `Right Shift` key. By default, scaling is not enabled (constant size).

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

**Note:** We also have added a new type of movement (dash), which can be performed by pressing LShift and any direction key (WASD). The dash can also be performed in diagonal directions by pressing two directions at the same time.
