## BIRD GAME 

**Controls**

Arrow keys: Move the bird up, down, left, or right

Right Shift: Toggle scaling to zoom in/out


**Game Objective**

Avoid collisions with the pipes! The bird is affected by gravity, so you will need to keep it in motion and avoid hitting the pipes or the ground. The game ends if a collision is detected.

**Code Overview**

The game is built using SDL2 for rendering and handling user input. The game logic consists of:


1. Entities: Represent the bird and pipes in the game. Each entity has position, velocity, and a rectangular shape for collision detection.
2. Physics System: Simulates gravity affecting the bird.
3. Movement Patterns: Define vertical motion patterns for some pipes.
4. Collision Detection: Checks for collisions between the bird and pipes. Upon collision, the bird changes color, and the game ends.
