# Warre Stoop – Programming 4 Project

This repository contains my custom 2D C++ game engine and game, created for the Programming 4 course at DAE.  
It started from the [Minigin](https://github.com/avadae/minigin) template, but the core systems (game objects, components, scene graph, etc.) have been extended, added or replaced to support my design goals.

The project uses **SDL3** for windowing, input and rendering, and **glm** for vector math.

[![CMake Build](https://github.com/wstoop/Prog4/actions/workflows/cmake.yml/badge.svg)](https://github.com/wstoop/Prog4/actions/workflows/cmake.yml)
[![Emscripten Build](https://github.com/wstoop/Prog4/actions/workflows/emscripten.yml/badge.svg)](https://github.com/wstoop/Pog4/actions/workflows/emscripten.yml)

## Goal

This project was made as the exam assignment for the course Programming 4 at DAE.  
The goal is to recreate a given arcade game, which in my case is **Galaga**, using a self‑built engine.

The design of the engine is based on patterns from *Game Programming Patterns* by Robert Nystrom that were discussed during the course.
