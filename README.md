## Game Engine Project: Time and Networking Enhancements
## Overview
This project enhances a game engine with advanced time management and networking capabilities. It includes the implementation of a Timeline class for time representation, multithreaded architecture, and networking using the 0MQ library to support client-server and peer-to-peer communications.
## Features
- Timeline Class: Manages time at various scales, allowing for object movement based on elapsed time, pausing, and timeline scaling.
- Networking: Utilizes 0MQ for client-server communication with PUB-SUB, REQ-REP, and PUSH-PULL models.
- Multithreaded Architecture: Distributes tasks across threads to improve performance and responsiveness.
- Asynchronous Server Design: Supports clients running at different speeds without affecting overall performance.
- Peer-to-Peer Networking: Allows direct communication between clients, reducing server load.
## Code Structure
### Main Components
- Timeline globalTimeline: Represents the global timeline for managing time-dependent systems.
- PhysicsSystem physicsSystem: Handles physics calculations such as gravity.
- Server and Client Classes: Manage server-client interactions with start methods for initiating connections.
## Key Functions
- runServer(Server& server): Starts the server to handle client requests.
- runClient(EntityManager& entityManager, EntityManager& clientEntityManager): Connects the client to the server and starts client operations.
- applyGravityOnEntities(PhysicsSystem& physicsSystem, EntityManager& entityManager): Applies gravity to entities continuously.
- doServerEntities(Server& server): Manages server-side entities and updates their states.
- doClientGame(): Initializes SDL, manages client-side entities, handles input, updates entity states, and renders scenes.
