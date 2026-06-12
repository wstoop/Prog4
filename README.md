# Warre Stoop – Programming 4 Project

This repository contains my custom 2D C++ game engine and game, created for the Programming 4 course at DAE.  
It started from the [Minigin](https://github.com/avadae/minigin) template, but the core systems (game objects, components, scene graph, etc.) have been extended, added or replaced to support my design goals.

The project uses SDL3 for windowing, input, sound and rendering, and glm for vector math. xinput is used for controller input.

[![CMake Build](https://github.com/wstoop/Prog4/actions/workflows/cmake.yml/badge.svg)](https://github.com/wstoop/Prog4/actions/workflows/cmake.yml)
[![Emscripten Build](https://github.com/wstoop/Prog4/actions/workflows/emscripten.yml/badge.svg)](https://github.com/wstoop/Pog4/actions/workflows/emscripten.yml)

## Goal

This project was made as the exam assignment for the course Programming 4 at DAE.  
The goal is to recreate a given arcade game, which in my case is Galaga, using a self‑built engine.

The design of the engine is based on patterns from *Game Programming Patterns* by Robert Nystrom that were discussed during the course.

## Engine Architecture & Custom Design Decisions

Beyond the baseline course requirements, I chose to implement several architectural choices to evolve the engine. My design philosophy was heavily inspired by the Unity Engine, which is the environment I am most comfortable with.

### 1. Architectural Evolution of Features
As my understanding of game patterns progressed during the course, the code naturally evolved:
* Lambda Callbacks: Early features, such as the `HealthComponent`, were built using lambda functions as callbacks to handle state changes. These remain in the codebase as a legacy solution for those specific early systems.
* Event Queue: After learning about the Observer and Event patterns, I stopped using new lambda callbacks. Instead, I implemented a centralized Event Queue to handle communication between systems cleanly, mirroring Unity's event-driven workflow.

### 2. GameObject & Transform Hierarchy
* In contrast to the default template, I refactored the architecture to treat `Transform` strictly as a component.
* Every `GameObject` is now structurally forced to possess exactly one `TransformComponent`, ensuring a clean, uniform approach to handling the scene graph and spatial positioning.

### 3. Tag System
* To manage gameplay logic and object identification efficiently, I implemented a Tag system inspired by Unity.

### 4. Custom SceneBuilder & Input Contexts
To keep level initialization readable and decoupled, I developed a custom `SceneBuilder` class.
* It encapsulates bootstrapping logic (such as spawning backgrounds, establishing HUD modules, and initializing wave managers).
* It automatically manages Input Context shifts between scene transitions using a `SceneInputBinding` payload. This smoothly swaps controller assignments, clears interactive UI states (`UISelection`), and registers/unregisters commands as players shift between menus and gameplay.

### 5. Enemy State Pattern
The enemies in the game utilize a robust State Pattern to handle their behavioral loops:
* Shared States: Common behaviors, such as flying into the scene following pre-calculated curves and maintaining the scrolling formation grid, are shared across different enemy types.
* Custom States: Specialized combat logic (such as diving out of formation or executing unique attack vectors) are custom-written for individual enemy behaviors.

### 6. Macro Game Flow
* The overarching game cycle (handling main menus, level transitions, game-over sequences, and score handovers) is managed by a top-level Game State Machine.

---

## Controls & Gameplay Modes

The game is fully playable using either a Keyboard or a Controller.

### Standard Player Controls
| Action | Keyboard | Controller |
| :--- | :--- | :--- |
| Movement | `W` `A` `S` `D` | D-Pad / Left Thumbstick |
| Shoot | `Spacebar` | `A` Button |

### Versus Mode (Boss Control)
In Versus mode, a second player takes control of a Boss Alien to fight against the classic fighter:
* Dive Bomb Attack: Controller `A` Button
* Tractor Beam Capture: Controller `B` Button
