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


## Game

Simple single player game
Player controls the box on the left side of the screen 

### Goal -
Make the player box reach the green goal on the right side of the screen without touching the floating obstacles

### Input -
Up/W, Down/S, Left/A, Right/D

### Difficulty -
The player box is under downwards and leftwards gravity and the longer player takes more difficult the game becomes. You lose if you touch any of the obstacles.