# Snake Game with Replay and Networking

This project is a modern implementation of the classic Snake game with features like **recording and replaying gameplay**, **multiplayer networking** (P2P support), and **entity management**.

---

## **Features**

- **Classic Snake Gameplay**: Navigate the snake to eat food and grow longer while avoiding collisions with the snake's body and game boundaries.
- **Replay System**: Record and replay your gameplay sessions.
- **Multiplayer Support**: Supports P2P networking for multiplayer gaming.
- **Flexible Entity Management**: Entities like the snake, food, walls, and more are managed dynamically with collision handling and event processing.

---

## **Controls**

### **Gameplay**
- **`W`**: Move the snake **up**.
- **`A`**: Move the snake **left**.
- **`S`**: Move the snake **down**.
- **`D`**: Move the snake **right**.

### **Replay and Recording**
- **`Enter`**: Toggle **recording**.
  - Pressing `Enter` starts or stops recording the current gameplay session.
  - When recording starts, a message appears: `Started Recording`.
  - When recording stops, a message appears: `Stopped Recording`.
- **`Right Shift`**: Play the last **recorded replay**.
  - Pressing `Right Shift` replays the most recent recorded session.
  - During replay, no new inputs are processed.

### **Exit**
- **`Esc`**: Close the game.

---

## **Game Rules**

1. **Objective**: Grow your snake by eating red food while avoiding collisions.
2. **Food**: Eating food increases the snake's length, and food reappears at a random position.
3. **Collision**:
   - Colliding with the walls or your own body ends the game.
   - Colliding with food triggers the snake to grow.

---

## **Replay System**

- **Recording**:
  - Press `Enter` to start recording.
  - All gameplay events, including movements, collisions, and interactions, are saved during recording.
  - Press `Enter` again to stop recording.

- **Replay**:
  - Press `Right Shift` to replay the most recent recorded session.
  - The replay shows all recorded gameplay events exactly as they happened.
  - No new inputs are processed during replay.

---

## **Networking**

- **P2P Mode**: When `isP2P` is enabled, the game enters a multiplayer mode, syncing player data and game state over the network.
- **Client-Server Architecture**: Separate `EntityManager` instances manage player and client entities.
- **Networking Thread**: A dedicated thread handles real-time communication between clients.

---

## **How to Build and Run**

### Prerequisites
1. **C++17 or higher**
2. **SDL2**: Ensure SDL2 library is installed on your system.
3. **CMake**: For building the project.

### Steps
1. Clone the repository:
   ```bash
   git clone <repo-url>
   cd <repo-directory>
   ```
2. Build the project using CMake:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
3. Run the game:
   ```bash
   ./SnakeGame
   ```

---

## **Game Architecture**

1. **Entity Management**:
   - All game objects (e.g., snake, food, walls) are represented as `Entity` objects.
   - Dynamic addition and removal of entities during gameplay.

2. **Event Management**:
   - Uses an event-driven model to handle collisions, movements, and custom game events.
   - Replay and recording events are managed through the global event manager.

3. **Collision Handling**:
   - Snake collisions with food, walls, or its own body trigger events.
   - Custom collision handlers are registered for different entity types.

4. **Timeline and Replay**:
   - Game state is recorded along with timestamps using a `globalTimeline`.
   - Events are replayed in sequence to ensure accurate playback.

5. **Networking**:
   - Player and client data are managed in separate `EntityManager` instances.
   - Networking logic runs on a dedicated thread to handle communication without blocking gameplay.

---

Enjoy the game, and have fun with the replay system! 🎮🐍
