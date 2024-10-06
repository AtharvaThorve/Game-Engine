## Game Engine

This is a C++ based Visual Studio solution for Game Engine. More information in writeup

### Installation
1. Open the solution in Visual Studio
2. In the solution explorer (inside Visual Studio) double click on Game-Engine.sln file
3. Open terminal inside Visual Studio and run the following commands
    - `vcpkg integrate install`
    - `vcpkg install`
4. Build the Project

**Note: Scaling can be toggled by pressing the Right Shift key, by default scaling is not enabled *(Constant Size)***

### Timeline
For the different timeline features view the list below:
1. Pausing and Unpausing can be toggled using `ESC` key.
2. Speed up of the game happens with the `p` key.
3. Slow down of the game happens with the `m` key.

### Networking setup
There is no need to install libzmq dll file since it is already present in the x64 folder. But we followed the instructions that were provided [here](https://moodle-courses2425.wolfware.ncsu.edu/mod/page/view.php?id=351117). The version we installed is 4.3.5
<br><br>
We support both client-server and P2P architecture.
1. Client-Server:
    1. First start the server by passing `server` as the command line argument.
    2. While starting the client, you need to pass `client` as the command line argument.
2. P2P:
    1. First start a listen-server, which is started by either passing `client server` or `server client` as the command line arguments.
    2. Then for clients user needs to pass `client P2P` or `P2P client` as the command line arguments.
    3. **Note: While starting P2P client do make sure the clients are binding and connecting to correct IP addresses and ports.**