# Catan in Unreal Engine 5

[![Ask DeepWiki](https://devin.ai/assets/askdeepwiki.png)](https://deepwiki.com/Abdullah-coder2013/Catan-UE5)

I made a version of the board game "Settlers of Catan" from scratch in Unreal Engine 5. I used C++ for the game logic and UMG for the user interface. The Catan game is a lot of fun. I think this version is really cool.

## Features

- **Procedural Board Generation**: When you start a game the board is made randomly. This means that the resource tiles, number tokens and dock placements are all shuffled around for a game every time.

- **Complete Game Flow**: The whole Catan game loop is implemented, including the setup phase where you place your two settlements and roads and the main turn-based gameplay. You get to play the game from start to finish.

- **Core Catan Mechanics**: All the basic rules of Catan are implemented. The Catan game has a lot of rules. I tried to include all of them.

**Resource Management**: You gather resources like Wood, Brick, Sheep, Wheat and Ore based on dice rolls. The Catan game is about managing your resources.

**Building**: You spend resources to build Roads, Settlements and upgrade Settlements to Cities. Building is a part of the Catan game.

- **Comprehensive Trading System**:

You can propose, accept and decline trades with players. Trading is a way to get the resources you need.

You can trade with the bank at a 4:1 ratio or at ratios like 3:1 or 2:1 if you use the docks. The docks are locations on the board that give you better trade ratios.

- **Robber and "7-Roll" Mechanic**: When a 7 is rolled players with than seven cards have to get rid of half of them. The current player then moves the robber to block a resource tile and takes a card from a player next to it. The robber is a part of the Catan game.

- **Full Development Card System**: You can play all the development cards like Knight, Victory Point, Road Building, Year of Plenty and Monopoly. The development cards give you abilities and bonuses.

- **Victory Point Tracking**: The game keeps track of victory points from settlements, cities and Victory Point cards. It also has rules for giving out and taking away the "Longest Road" and "Largest Army" special victory point cards. The goal of the game is to get the most victory points.

- **Dynamic 3D Terrain**: The board is not a grid it is a 3D island landscape made with a procedural mesh. All the board elements like tiles, roads and settlements fit into the terrains shape. The 3D terrain is really cool. Makes the game more fun.

- **UMG UI**: The user interface lets you do all the game actions like building, trading playing development cards and handling game events like the robber. The UI is easy to use. Makes the game more enjoyable.

- **Enhanced Input**: The project uses Unreal Engines Enhanced Input system for player controls. The input system is really responsive. Makes the game more interactive.

- **Cheat Manager**: There is a built-in cheat manager that lets you easily give resources, development cards and victory points for debugging and testing. The cheat manager is really helpful for testing the game.

## Core Systems & Architecture

The project is mostly made with C++. It has a separation of responsibilities:

- **`GameModeCPP`**: This is the part of the game that manages the game state player turns, dice rolls, resource distribution. Enforces the rules of Catan. The GameModeCPP is the core of the game.

- **`BoardManager`**: This makes the game world at the start of the game. It sets up all the `HexTile` `HexVertex` `HexEdge` and `Dock` actors to make an playable board. The BoardManager is responsible for creating the board.

- **`BoardTerrain`**: This generates a 3D island landscape using a mesh. It makes the terrain look smooth and natural. The BoardTerrain is really cool. Makes the game more immersive.

- **`CatanPlayerController`**: This handles player input using the Enhanced Input system. It turns player clicks into game actions like building or placing roads. The CatanPlayerController is responsible for handling player input.

- **Board Actors (`HexTile` `HexVertex` `AHexEdge`)**: These are the actor classes that represent the parts of the game board. They keep their state. Have rules for placement validation. The Board Actors are the building blocks of the game.

- **`DebugUserWidget`**: This is the UMG widget that handles all player-facing UI and interactions. It shows the game state handles button clicks and manages the modals for building, trading and playing development cards. The DebugUserWidget is the UI of the game.

## Getting Started

This project is made with Unreal Engine 5. To get started you need to:

1. Clone the repository to your computer.

2. Make sure you have Unreal Engine 5 and Visual Studio with the "Game Development with C++" workload installed.

3. Right-click the `CATANMegaproject.uproject` file and select "Generate Visual Studio project files".

4. Open the `CATANMegaproject.sln` file in Visual Studio.

5. Set the solution configuration to **Development Editor**. The platform to **Win64**.

6. Build the solution.

7. After it builds successfully you can run the game from Visual Studio by opening the `CATANMegaproject.uproject` file in the Unreal Editor and clicking "Play".
